/* rpc.cpp
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-22
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#include <string>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <execinfo.h>

#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "rpc.h"
#include "proxy.h"
#include "message.h"
#include "bodydata.h"

using namespace std;

#define RPC_DEBUG   printf
#define RPC_INFO    printf
#define RPC_WARN    printf
#define RPC_ERROR   printf

#define RPCNOSPECIFYSERVICE     0x0001   // no service

string HSAERPC::process;
StringHash HSAERPC::func_hash;
StringHash HSAERPC::process_hash;
UINTHash HSAERPC::proxy_hash;
list<void *>  HSAERPC::m_list;
bool HSAERPC::initflag = false;
COMMDriver HSAERPC::rpc_comm_base;
struct timeval HSAERPC::conntv;
struct timeval HSAERPC::commtv;
SocketBaseOpt HSAERPC::socket_base;
struct WorkerHead HSAERPC::w_head;
unsigned int HSAERPC::frame_id = 1;
ThreadPool *HSAERPC::rpc_threadpool = NULL;
ModuleConfig HSAERPC::module_config;
NetworkConfig HSAERPC::network_config;
volatile bool HSAERPC::run_flag = false;
pthread_cond_t HSAERPC::send_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t HSAERPC::send_mutex = PTHREAD_MUTEX_INITIALIZER;

HSAERPC::HSAERPC()
{
    m_list.clear();
    process.clear();
    func_hash.clear();
    proxy_hash.clear();
    LIST_INIT(&w_head);
    process_hash.clear();
    memset(&commtv, 0, sizeof(struct timeval));
    memset(&conntv, 0, sizeof(struct timeval));
}

HSAERPC::~HSAERPC()
{
    m_list.clear();
    process.clear();
    func_hash.clear();
    proxy_hash.clear();
    LIST_INIT(&w_head);
    process_hash.clear();
    memset(&commtv, 0, sizeof(struct timeval));
    memset(&conntv, 0, sizeof(struct timeval));
}

int HSAERPC::setProcessName(const char *process)
{
    if(process[0] == '.')
        this->process = &process[2];
    else
        this->process = process;

    return 0;
}

int HSAERPC::setConfigPath(const string &network, const string &module)
{
    int ret = -1;
    ret = network_config.setConfigProfile(network);
    if(ret == 0)
        ret = module_config.setConfigProfile(module);
    if(ret == 0)
        initflag = true;
    return ret;
}

int HSAERPC::addService(const char *server, ServerFunction func)
{
    string key(server);
    if(func_hash.find(key) != NULL)
    {
        RPC_ERROR("RPC : %s server has been exsit!\n", server);
        return -1;
    }
    return func_hash.insert(key, (void *)func);
}

int HSAERPC::setResponseMsg(void *arg,void *response, size_t len)
{
    int result = -1;
    unsigned int slen;
    BodyData bodydata;
    MessageStr *resmsg = NULL;
    MessageStr *message = (MessageStr *)arg;

    if(message == NULL)
    {
        RPC_ERROR("RPC : message arg can't be NULL!\n");
        return -1;
    }
    if(!initflag)
        return -1;
    
    bodydata.getUserBodyData(arg,&slen);

    result = mallocApplyResponseMsg(&resmsg, NULL, message->message_id);
    if(result != 0)
        return -1;
    result = bodydata.mallocBodyData(&resmsg->body_data, response, len);
    if(result != 0)
    {
        releaseMessage(resmsg);
        return -1;
    }
    setBodyData(resmsg, resmsg->body_data, bodydata.size());
    
    if(bodydata.getSender() == this->process) // module in current process
    {
        Proxy *proxy_impl = (Proxy *)this->proxy_hash.find(resmsg->message_id);
        if(proxy_impl == NULL)
        {
            releaseRPCMessage((void *)resmsg);
            return -1;
        }

        proxy_impl->setResponseMsg((void *)resmsg);
        proxy_impl->wakeup();
    }
    else
    {
        struct WorkerEntry *worker = (struct WorkerEntry *)malloc(sizeof(struct WorkerEntry));
        if(worker == NULL)
        {
            RPC_ERROR("RPC : malloc WorkerEntry struct failed!\n");
            releaseRPCMessage((void *)resmsg);
            return -1;
        }
        worker->message = (void *)resmsg;
        addSendWorker((void *)worker);
    }

    return 0;
}

int HSAERPC::proxyCall(const string &module, const string &func, void *send, size_t slen, void *recv, size_t &rlen)
{
    int result = -1;
    unsigned int len;
    void *ptr = NULL;
    SocketStruct addr;
    BodyData bodydata;
    string module_process;
    unsigned int frame = 0;
    Proxy proxy_impl;
    MessageStr *message = NULL;
    MessageStr *response = NULL;
    ServerFunction func_ptr = NULL;

    // 0. judge process was exit
    if(!initflag)
        return -1;
    if(!run_flag)
    {
        usleep(300*1000);
        return -1;
    }
    
    // 1. first refer Module-Process table
    result = module_config.referModule(module, module_process);
    if(result < 0)
    {
        RPC_ERROR("RPC : current system does't has %s module, please check!\n", module.c_str());
        return -1;
    }

    // 2. refer process ip addr, contain self
    result = network_config.getNetworkConfig(module_process, addr);
    if(result < 0)
    {
        RPC_ERROR("RPC : %s process does't not have ip-addr config, please check!\n", process.c_str());
        return -1;
    }

    // 3.init message
    frame = getFrameID();
    result = mallocApplyRequestMsg(&message, NULL, frame);
    if(result != 0)
        return -1;
    bodydata.setModule(module);
    bodydata.setFunction(func);
    bodydata.setSender(this->process);
    result = bodydata.mallocBodyData(&message->body_data, send, slen);
    if(result != 0)
    {
        releaseMessage(message);
        return -1;
    }
    setBodyData(message, message->body_data, bodydata.size());
    
    // 4. judge whether self-process
    if(module_process == this->process) // module in current process
    {
        // 4.1.1 refer server function
        func_ptr = (ServerFunction)func_hash.find(func);
        if(func_ptr == NULL)
        {
            RPC_ERROR("RPC : %s module does't has %s function, please check!\n", module.c_str(), func.c_str());
            releaseRPCMessage((void *)message);
            return -1;
        }

        // 4.1.2 add worker for deal with
        result = rpc_threadpool->addWork(LowPriority, func_ptr, (void *)message, releaseRPCMessage);
        if(result < 0)
        {
            releaseRPCMessage((void *)message);
            return -1;
        }
    }
    else
    {
        // 4.2 add send worker for remote call
        struct WorkerEntry *worker = (struct WorkerEntry *)malloc(sizeof(struct WorkerEntry));
        if(worker == NULL)
        {
            RPC_ERROR("RPC : malloc WorkerEntry struct failed!\n");
            releaseRPCMessage((void *)message);
            return -1;
        }
        worker->message = (void *)message;
        addSendWorker((void *)worker);
    }

    // 5.proxy wait response
    result = proxy_impl.init();
    if(result != 0)
    {
        RPC_ERROR("RPC : init proxy service failed!\n");
        return -1;
    }
    proxy_hash.insert(frame ,(void *)&proxy_impl);
    result = proxy_impl.wait(commtv);
    proxy_hash.remove(frame);
    response = (MessageStr *)proxy_impl.getResponseMsg();
    proxy_impl.destroy();

    // 6.if response return, then get data
    if((result == 0) && response)
    {
        bodydata.clear();
        if(response->status_code != 0)
        {
            switch(response->status_code)
            {
                case RPCNOSPECIFYSERVICE:
                    RPC_ERROR("RPC : %s module does't has %s function, please check!\n", module.c_str(), func.c_str());
                    break;
                default:
                    break;
            }
        }
        else
        {
            ptr = bodydata.getUserBodyData((void *)response, &len);
            if(len > rlen)
            {
                len = rlen;
                RPC_WARN("RPC : recv data buffer not enough!\n");
            }
            memcpy(recv, ptr, len);
        }
    }
    if(response)
        releaseRPCMessage((void *)response);

    return result;
}

int HSAERPC::start(void)
{
    int result = -1;
    int fix,dyn,queue;
    SocketStruct socket_str;
    struct sockaddr_un u_addr;
    struct sockaddr_in s_addr;

    if(!initflag)
        return -1;
    
    // 1.find local socket config
    result = network_config.getNetworkConfig(this->process, socket_str);
    if(result < 0)
    {
        run_flag = false;
        RPC_ERROR("RPC : could not find %s process configuration!\n", process.c_str());
        return -1;
    }

    // 2.set communicate timeout
    network_config.getConnectTimeout(&conntv);
    if((conntv.tv_sec == 0) && (commtv.tv_usec == 0))
    {
        conntv.tv_sec = TIMEOUTDEFAULTSEC;
        conntv.tv_usec = TIMEOUTDEFAULTUSEC;
    }
    network_config.getCommunicateTimeout(&commtv);
    if((commtv.tv_sec == 0) && (commtv.tv_usec == 0))
    {
        commtv.tv_sec = TIMEOUTDEFAULTSEC;
        commtv.tv_usec = TIMEOUTDEFAULTUSEC;
    }
    rpc_comm_base.setTimeout(commtv);

    // 3.create socket base thread from
    if(socket_str.port == 0) // use localsocket
    {
        unlink(socket_str.ipaddr.c_str());
        socket_base.initSockaddr(u_addr, socket_str.ipaddr.c_str());
        result = rpc_comm_base.create(eventHandler, (struct sockaddr *)&u_addr, sizeof(u_addr));
    }
    else
    {
        socket_base.initSockaddr(s_addr, socket_str.ipaddr.c_str(), socket_str.port);
        result = rpc_comm_base.create(eventHandler, (struct sockaddr *)&s_addr, sizeof(s_addr));
    }
    if(result != 0)
    {
        run_flag = false;
        return result;
    }
    
    // 4.create threadpool
    fix = module_config.getFixThreadNum();
    if(fix <= 0)
        fix = FIXTHREADDEFAULTNUM;
    dyn = module_config.getDynThreadNum();
    if(dyn <= 0)
        dyn = DYNTHREADDEFAULTNUM;
    queue = module_config.getMaxQueueNum();
    if(queue <= 0)
        queue = MAXQUEUEDEFAULTNUM;
    rpc_threadpool = ThreadPool::getInstance();
    if(rpc_threadpool == NULL)
    {
        RPC_ERROR("RPC : new threadpool failed!\n");
        run_flag = false;
        return -1;
    }
    result = rpc_threadpool->create(fix, dyn, queue);
    if(result == 0)
    {
        run_flag = true;
        if(pthread_create(&sendThreadID, NULL, sendThread, NULL) != 0)
        {
            run_flag = false;
            RPC_ERROR("RPC : %s: pthread_create failed, errno:%d,error:%s.\n", __FUNCTION__, errno, strerror(errno));
            return -1;
        }
    }
    else
    {
        run_flag = false;
    }
    
    return result;
}

int HSAERPC::runUntilAskedToQuit(bool flag)
{
    list<void *>::iterator it;

    if(!initflag)
    {
        RPC_ERROR("RPC : have't init configuration file!");
        exit(-1);
    }
    if(!run_flag)
    {
        exit(-1);
    }
    
    if(signal(SIGINT,signalHandler) == SIG_ERR)
        RPC_ERROR("RPC : register SIGINT failed!");
    if(signal(SIGTERM,signalHandler) == SIG_ERR)
        RPC_ERROR("RPC : register SIGTERM failed!");
    if(signal(SIGALRM,signalHandler) == SIG_ERR)
        RPC_ERROR("RPC : register SIGALRM failed!");
    if(signal(SIGSEGV,signalHandler) == SIG_ERR)
        RPC_ERROR("RPC : register SIGSEGV failed!");

    run_flag = flag;
    while(run_flag)
    {
        sleep(1);
    }

    // exit send thread
    pthread_cond_signal(&send_cond);
    pthread_join(sendThreadID, NULL);
    
    for(it = m_list.begin(); it != m_list.end(); it++)
        rpc_comm_base.disconnect(*it);
    m_list.clear();
    rpc_threadpool->destroy();
    rpc_comm_base.destroy();

    exit(0);
}

void HSAERPC::addSendWorker(void *worker)
{
    pthread_mutex_lock(&send_mutex);
    LIST_INSERT_HEAD(&w_head, (struct WorkerEntry *)worker, worker_next);
    pthread_cond_signal(&send_cond);
    pthread_mutex_unlock(&send_mutex);
}

void *HSAERPC::sendThread(void *arg)
{
    int result = -1;
    string process;
    SocketStruct dest;
    BodyData bodydata;
    unsigned int slen;
    char *send = NULL;
    int connect_count = 0;
    MessageStr *msg = NULL;
    void *connectfd = NULL;
    void *process_fd = NULL;
    list<void *>::iterator it;

    struct sockaddr_un u_addr;
    struct sockaddr_in s_addr;
    struct WorkerEntry *worker = NULL;
    
    RPC_INFO("RPC : send thread running, id:%lu\n",pthread_self());
    
    while(run_flag)
    {
        pthread_mutex_lock(&send_mutex);
        while((LIST_FIRST(&w_head) == NULL) && run_flag)
        {
            pthread_cond_wait(&send_cond, &send_mutex);
        }
        
        if(!run_flag)
        {
            pthread_mutex_unlock(&send_mutex);
            break;
        }
        assert(LIST_FIRST(&w_head) != NULL);
        pthread_mutex_unlock(&send_mutex);

        worker = LIST_FIRST(&w_head);
        LIST_REMOVE(worker, worker_next);
        assert(worker->message != NULL);

        process.clear();
        connect_count = 0;
        bodydata.clear();
        msg = (MessageStr *)worker->message;
        bodydata.getUserBodyData(worker->message, &slen);
        if(!judgeResponse(msg))
            module_config.referModule(bodydata.getModule(), process);
        else
            process = bodydata.getSender();
        
REPEAT_CONNECT:
        connect_count++;
        if(!run_flag)
            break;
        process_fd = process_hash.find(process);
        if(process_fd == NULL)  // connect not exsit
        {
            if((CONNECTRETRYDEFAULT+1) == connect_count)
                goto FREE_MEMORY;
            
            network_config.getNetworkConfig(process, dest);
            if(dest.port == 0)
            {
                socket_base.initSockaddr(u_addr, dest.ipaddr.c_str());
                result = rpc_comm_base.connect((struct sockaddr *)&u_addr, sizeof(u_addr), conntv, &connectfd);
            }
            else
            {
                socket_base.initSockaddr(s_addr, dest.ipaddr.c_str(), dest.port);
                result = rpc_comm_base.connect((struct sockaddr *)&s_addr, sizeof(s_addr), conntv, &connectfd);
            }
            if(result < 0)
                goto REPEAT_CONNECT;

            // wait send link message
            result = sendLinkMessage(connectfd);
            if(result != 0)
            {
                for(it = m_list.begin(); it != m_list.end(); it++)
                {
                    if(connectfd == *it)
                    {
                        m_list.remove(connectfd);
                        rpc_comm_base.disconnect(connectfd);
                        break;
                    }
                }
                goto REPEAT_CONNECT;
            }
            // link message ok, then send user data
            goto REPEAT_CONNECT;
        }

        // start send data
        result = serializeMessage(&send, msg, &slen);
        if(result != 0)
            goto REPEAT_CONNECT;
        if(process_fd)
            rpc_comm_base.send(process_fd, send, slen);
        releaseSerializeMessage(send);
        send = NULL;
        
FREE_MEMORY:
        if(worker->message)
            releaseRPCMessage(worker->message);
        free(worker);
        worker = NULL;
        msg = NULL;
        dest.port = 0;
        dest.ipaddr.clear();
    }
    
    RPC_INFO("RPC : send thread exit.\n");
    
    pthread_exit(NULL);
}

int HSAERPC::sendLinkMessage(void *fdp)
{
    int result = -1;
    BodyData bodydata;
    char *send = NULL;
    unsigned int rlen;
    unsigned int slen;
    unsigned int frame;
    Proxy proxy_impl;
    MessageStr *message = NULL;
    MessageStr *response = NULL;

    // 1.malloc link message
    frame = getFrameID();
    result = mallocLinkRequestMsg(&message, NULL, frame);
    if(result != 0)
        return -1;
    bodydata.setSender(process);
    result = bodydata.mallocBodyData(&message->body_data, NULL, 0);
    if(result != 0)
    {
        releaseMessage(message);
        return -1;
    }
    setBodyData(message, message->body_data, bodydata.size());
    
    // 2.send link data
    result = serializeMessage(&send, message, &slen);
    if(result != 0)
    {
        releaseRPCMessage(message);
        return -1;
    }
    rpc_comm_base.send(fdp, send, slen);
    releaseRPCMessage(message);
    
    // 3.proxy wait response
    result = proxy_impl.init();
    if(result != 0)
    {
        RPC_ERROR("RPC : init proxy service failed!\n");
        return -1;
    }
    proxy_hash.insert(frame ,(void *)&proxy_impl);
    result = proxy_impl.wait(commtv);
    proxy_hash.remove(frame);
    response = (MessageStr *)proxy_impl.getResponseMsg();
    proxy_impl.destroy();

    // 4.if response return, then get data
    if((result == 0) && response)
    {
        bodydata.clear();
        bodydata.getUserBodyData((void *)response, &rlen);
        // 5.insert process hash
        process_hash.insert(bodydata.getSender(), fdp);
    }
    if(response)
        releaseRPCMessage((void *)response);
    
    return result;
}

int HSAERPC::recvLinkMessage(void *fdp, void *msg)
{
    int result = -1;
    BodyData bodydata;
    char *send = NULL;
    unsigned int slen;
    MessageStr *message = NULL;
    
    result = mallocLinkResponseMsg(&message, NULL, ((MessageStr *)msg)->message_id);
    if(result != 0)
        return -1;
    bodydata.setSender(process);
    result = bodydata.mallocBodyData(&message->body_data, NULL, 0);
    if(result != 0)
    {
        releaseMessage(message);
        return -1;
    }
    setBodyData(message, message->body_data, bodydata.size());
    
    // 2.send link data
    result = serializeMessage(&send, message, &slen);
    if(result != 0)
    {
        releaseRPCMessage(message);
        return -1;
    }
    rpc_comm_base.send(fdp, send, slen);
    releaseRPCMessage(message);
    
    return result;
}

int HSAERPC::errorACKMessage(void *fdp, void *msg)
{
    int result = -1;
    BodyData bodydata;
    char *send = NULL;
    unsigned int slen;
    MessageStr *message = NULL;
    
    result = mallocApplyResponseMsg(&message, NULL, ((MessageStr *)msg)->message_id);
    if(result != 0)
        return -1;
    
    bodydata.setSender(process);
    result = bodydata.mallocBodyData(&message->body_data, NULL, 0);
    if(result != 0)
    {
        releaseMessage(message);
        return -1;
    }
    setStatusCode(message, RPCNOSPECIFYSERVICE);
    setBodyData(message, message->body_data, bodydata.size());
    
    result = serializeMessage(&send, message, &slen);
    if(result != 0)
    {
        releaseRPCMessage(message);
        return -1;
    }
    rpc_comm_base.send(fdp, send, slen);
    releaseRPCMessage(message);
    
    return result;
}

unsigned int HSAERPC::getFrameID(void)
{
    return frame_id++;
}

void HSAERPC::releaseRPCMessage(void *arg)
{
    BodyData bodydata;
    MessageStr *message = (MessageStr *)arg;
    if(message)
        bodydata.releaseBodyData(message->body_data);
    releaseMessage(message);
}

int HSAERPC::recvData(void *fd_ptr, const void *data, size_t data_len)
{
    int result = -1;
    BodyData bodydata;
    void *old_link = NULL;
    MessageStr *msg = NULL;
    ServerFunction func_ptr = NULL;
    unsigned char *ptr = (unsigned char *)data;

    result = mallocMessage(&msg);
    if(result < 0)
        return -1;
    
    result = getHeadFromData(msg, data);
    if(result < 0)
    {
        releaseMessage(msg);
        return -1;
    }

    result = bodydata.mallocRecvBodyData(&msg->body_data, &ptr[MESSAGE_HEAD_SIZE], msg->body_size);
    if(result < 0)
    {
        releaseMessage(msg);
        return -1;
    }
    bodydata.getUserBodyData((void *)msg, NULL);

    switch(msg->message_type)
    {
        case MT_LINK_PK:
            if(!judgeResponse(msg))
            {
                old_link = process_hash.find(bodydata.getSender());
                if(old_link)
                {
                    process_hash.remove(bodydata.getSender());
                    rpc_comm_base.disconnect(old_link);
                    m_list.remove(old_link);
                }
                process_hash.insert(bodydata.getSender(), fd_ptr);
                recvLinkMessage(fd_ptr, (void *)msg);
                releaseRPCMessage((void *)msg);
            }
            else
            {
                Proxy *proxy_impl = (Proxy *)proxy_hash.find(msg->message_id);
                if(proxy_impl == NULL)
                {
                    releaseRPCMessage((void *)msg);
                    return -1;
                }

                proxy_impl->setResponseMsg((void *)msg);
                proxy_impl->wakeup();
            }
            break;
            
        case MT_BEAT_PK:
            releaseRPCMessage((void *)msg);
            break;
            
        case MT_APPLY_PK:
            if(!judgeResponse(msg))
            {
                func_ptr = (ServerFunction)func_hash.find(bodydata.getFunction());
                if(func_ptr == NULL)
                {
                    RPC_ERROR("RPC : %s module does't has %s function, please check!\n", bodydata.getModule().c_str(), bodydata.getFunction().c_str());
                    errorACKMessage(fd_ptr, msg);
                    releaseRPCMessage((void *)msg);
                    return -1;
                }

                result = rpc_threadpool->addWork(LowPriority, func_ptr, (void *)msg, releaseRPCMessage);
                if(result < 0)
                {
                    releaseRPCMessage((void *)msg);
                    return -1;
                }
            }
            else
            {
                Proxy *proxy_impl = (Proxy *)proxy_hash.find(msg->message_id);
                if(proxy_impl == NULL)
                {
                    releaseRPCMessage((void *)msg);
                    return -1;
                }

                proxy_impl->setResponseMsg((void *)msg);
                proxy_impl->wakeup();
            }
            break;
            
        default:
            releaseRPCMessage((void *)msg);
            break;
    }
    
    return 0;
}

int HSAERPC::eventHandler(unsigned int type, void *fd_ptr, void *data, size_t data_len)
{
    switch(type)
    {
        case COMMEventRecv:
            recvData(fd_ptr, data, data_len);
            break;
            
        case COMMEventSend:
            break;
            
        case COMMEventConnect:
            m_list.push_back(fd_ptr);
            break;
            
        case COMMEventDisconnect:
            m_list.remove(fd_ptr);
            process_hash.remove(fd_ptr);
            break;
            
        case COMMEventCheck:
            break;
            
        case COMMEventRTimeout:
            break;
            
        case COMMEventSTimeout:
            break;
            
        default:
            break;
    }
    
    return 0;
}

void HSAERPC::signalHandler(int signo)
{
    int size,i;
    char **strings;
    void *array[150];

    switch (signo)
    {
        case SIGINT:
        case SIGTERM:
            /* release system resource, and exit(0) */
            RPC_WARN("\n\nRPC : receive exit signal.\n");
            run_flag = false;
            break;

        case SIGALRM:
            break;

        case SIGSEGV:
            RPC_ERROR("RPC : Segment fault:\n");
            size = backtrace(array,150);
            strings = backtrace_symbols(array,size);
            for(i = 0;i < size;i++)
                RPC_ERROR(" %d deep : %s\n",i,strings[i]);
            free(strings);
            RPC_ERROR("RPC : exited!\n");
            exit(-1);
            break;

        default:
            break;
    }
}

