/* rpc_comm.cpp
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-20
 * Author: zhangqiyin/Konishi
 * Email : zhangqiyin@hangsheng.com.cn
 */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "rpc_comm.h"

#define RPC_DEBUG   printf
#define RPC_INFO    printf
#define RPC_WARN    printf
#define RPC_ERROR   printf

#define tp_tvaddtp(tv, tp, ttp)					\
	do {								\
		(ttp)->tv_sec = (tv)->tv_sec + (tp)->tv_sec;		\
		(ttp)->tv_nsec = ((tv)->tv_usec * 1000)+ (tp)->tv_nsec;       \
		if ((ttp)->tv_nsec >= 1000000000) {			\
			(ttp)->tv_sec++;				\
			(ttp)->tv_nsec -= 1000000000;			\
		}							\
	} while (0)

#define DEFAULT_CHECK_CYCLE   15  // check socket status every 15 second
#define DEFAULT_COMM_TIMEOUT  1   // communication timeout 1 second

EventHandler RPCComm::event_handler = NULL;
struct timeval RPCComm::comm_tv = {DEFAULT_COMM_TIMEOUT, 0};
struct timeval RPCComm::check_tv = {DEFAULT_CHECK_CYCLE, 0};

pthread_cond_t RPCComm::connect_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t RPCComm::connect_mutex = PTHREAD_MUTEX_INITIALIZER;
 
 RPCComm::RPCComm()
{
    comm_base = NULL;
    check_event = NULL;
    accept_base = NULL;
    evcon_listener = NULL;
    comm_thread_id = 0;
    accept_thread_id = 0;
}

 RPCComm::~RPCComm()
{
    comm_base = NULL;
    check_event = NULL;
    accept_base = NULL;
    evcon_listener = NULL;
    comm_thread_id = 0;
    accept_thread_id = 0;
}

 void RPCComm::showVersion(void)
{
    RPC_INFO("RPCCOMM : version: %s\n",RPC_COMM_VERSION);
    RPC_INFO("RPCCOMM : Third software :libevent %s\n", event_get_version());
}

void RPCComm::showMethods(void)
{
    const char **a = event_get_supported_methods();
    RPC_INFO("RPCCOMM : System supported methods:\n");
    for(int i = 0; a[i] != NULL; i++)
        RPC_INFO("  [%s]\n",a[i]);

    RPC_INFO("RPCCOMM : now we use %s\n",event_base_get_method(accept_base));
}

void RPCComm::setCommTime(struct timeval &tv)
{
    if((tv.tv_sec == 0) && (tv.tv_usec == 0))
    {
        comm_tv.tv_sec = DEFAULT_COMM_TIMEOUT;
        comm_tv.tv_usec = 0;
    }
    else
    {
        comm_tv.tv_sec = tv.tv_sec;
        comm_tv.tv_usec = tv.tv_usec;
    }
}

void RPCComm::setCheckTime(struct timeval &tv)
{
    if((tv.tv_sec == 0) && (tv.tv_usec == 0))
    {
        check_tv.tv_sec = DEFAULT_CHECK_CYCLE;
        check_tv.tv_usec = 0;
    }
    else
    {
        check_tv.tv_sec = tv.tv_sec;
        check_tv.tv_usec = tv.tv_usec;
    }
}

// para: s_addr can be a sockaddr_in or sockaddr_un, s_len is the length of it.
int RPCComm::create(EventHandler handler, struct sockaddr *s_addr, size_t s_len)
{
    // if env set, then open debug   notice
    event_enable_debug_mode();
    event_enable_debug_logging(0);

    evthread_use_pthreads();  //enable threads

    comm_base = event_base_new();
    if(!comm_base)
    {
        RPC_ERROR("RPCCOMM : new communication_event_base failed!\n");
        goto INIT_EVENT_FAILED;
    }
    
    accept_base = event_base_new();
    if(!accept_base)
    {
        RPC_ERROR("RPCCOMM : new accept_event_base failed!\n");
        goto INIT_EVENT_FAILED;
    }
    
    evthread_make_base_notifiable(comm_base);
    evthread_make_base_notifiable(accept_base);

    check_event = (struct event *)malloc(sizeof(struct event));
    if(!check_event)
    {
        RPC_ERROR("RPCCOMM : new timing check event failed!\n");
        return -1;
    }
    
    evcon_listener = evconnlistener_new_bind(accept_base, listenerCallback,(void *)comm_base,
        LEV_OPT_REUSEABLE|LEV_OPT_REUSEABLE_PORT|LEV_OPT_THREADSAFE|BEV_OPT_CLOSE_ON_FREE,
        -1, s_addr, s_len);
    if(!evcon_listener)
    {
        RPC_ERROR("RPCCOMM : new event_listener failed!\n");
        goto INIT_EVENT_FAILED;
    }

    if(evtimer_assign(check_event, comm_base, cycleCheckCallback, check_event) < 0)
    {
        RPC_ERROR("RPCCOMM : assign event-timer failed!\n");
        goto INIT_EVENT_FAILED;
    }

    if(evtimer_add(check_event, &check_tv) < 0)
    {
        RPC_ERROR("RPCCOMM : add event-timer failed!\n");
        return -1;
    }

    event_handler = handler;

    if(pthread_create(&comm_thread_id, NULL, commEventThread, comm_base) != 0)
	{
		RPC_ERROR("RPCCOMM : pthread_create failed, errno:%d,error:%s.\n", errno, strerror(errno));
		return -1;
	}
    if(pthread_create(&accept_thread_id, NULL, acceptEventThread, accept_base) != 0)
	{
		RPC_ERROR("RPCCOMM : pthread_create failed, errno:%d,error:%s.\n", errno, strerror(errno));
		return -1;
	}
    
    return 0;
    
INIT_EVENT_FAILED:
    destroy();
    return -1;
}

void RPCComm::destroy(void)
{
    if(accept_thread_id != 0)
    {
        event_base_loopexit(accept_base, NULL);
        pthread_join(accept_thread_id, NULL);
        accept_thread_id = 0;
    }
    if(comm_thread_id != 0)
    {
        event_base_loopexit(comm_base, NULL);
        pthread_join(comm_thread_id, NULL);
        comm_thread_id = 0;
    }
    
    if(evcon_listener)
    {
        evconnlistener_free(evcon_listener);
        evcon_listener = NULL;
    }
    if(check_event)
    {
        evtimer_del(check_event);
        free(check_event);
        check_event = NULL;
    }
    if(comm_base)
    {
        event_base_free(comm_base);
        comm_base = NULL;
    }
    if(accept_base)
    {
        event_base_free(accept_base);
        accept_base = NULL;
    }
}

int RPCComm::send(const void *fdptr, const void *data, size_t size)
{
    struct bufferevent * bev = (struct bufferevent *)fdptr;
    
    if(bev)
        return bufferevent_write(bev, data, size);

    return -1;
}

int RPCComm::connect(struct sockaddr *s_addr, size_t s_len, struct timeval &tv)
{
    int result = -1;
    struct timespec outtime;
    struct bufferevent *bev = NULL;
    struct timeval c_tv = {0, 20*1000};  // add 20 ms for connect wait

    bev = bufferevent_socket_new(comm_base, -1, BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE);
    if(!bev)
    {
        RPC_ERROR("RPCCOMM : new bufferevent_socket failed!\n");
        goto CONNECT_FAILED;
    }

    bufferevent_setcb(bev, readCallback, NULL, eventCallback, bev);
//    bufferevent_setcb(bev, readCallback, writeCallback, eventCallback, bev);
    bufferevent_set_timeouts(bev, &tv, NULL);
    result = bufferevent_socket_connect(bev, s_addr, s_len);
    if( -1 == result)
    {
        RPC_ERROR("RPCCOMM : Connect to %s failed, %s!\n", inet_ntoa(((sockaddr_in *)s_addr)->sin_addr), evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
        goto CONNECT_FAILED;
    }
    bufferevent_enable(bev , EV_READ|EV_WRITE);

    // wait connect back
    pthread_mutex_lock(&connect_mutex);
    clock_gettime(CLOCK_REALTIME,&outtime);
    evutil_timeradd(&tv, &c_tv, &c_tv);
    tp_tvaddtp(&c_tv, &outtime, &outtime);
    result = pthread_cond_timedwait(&connect_cond, &connect_mutex, &outtime);
    pthread_mutex_unlock(&connect_mutex);
    if(ETIMEDOUT == result)
    {
        RPC_ERROR("RPCCOMM : connect to %s timeout!\n",inet_ntoa(((sockaddr_in *)s_addr)->sin_addr));
        goto CONNECT_FAILED;
    }
    else if(result < 0)
    {
        RPC_ERROR("RPCCOMM : connect to %s error, %s!\n",inet_ntoa(((sockaddr_in *)s_addr)->sin_addr), strerror(errno));
        goto CONNECT_FAILED;
    }

    return 0;

CONNECT_FAILED:
    return -1;
}

void RPCComm::disconnect(void *fdptr)
{
    struct bufferevent *bev = (struct bufferevent *)fdptr;
    if(bev)
        bufferevent_free(bev);
}

void RPCComm::cycleCheckCallback(evutil_socket_t fd, short event, void *arg)
{
    struct event *base = (struct event *)arg;
    
    evtimer_add(base, &check_tv);
    
    if(event_handler)
        event_handler(RPCEventCheck, NULL, NULL, 0);
}

void RPCComm::listenerCallback(struct evconnlistener *listener, evutil_socket_t fd,
                               struct sockaddr *sa, int socklen, void *user_data)
{
    int result = -1;
    struct bufferevent *bev = NULL;
    struct event_base *base = (struct event_base *)user_data;

    RPC_INFO("RPCCOMM : New connecting from %s:%d\n",inet_ntoa(((sockaddr_in *)sa)->sin_addr),
           ntohs(((sockaddr_in *)sa)->sin_port));

    bev = bufferevent_socket_new(base, fd, LEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE);
    if(!bev)
    {
        RPC_ERROR("RPCCOMM : Could not create new bufferevent!\n");
        return ;
    }

    // set communication timeout if needed
    bufferevent_set_timeouts(bev, &comm_tv,NULL);
    //bufferevent_set_timeouts(bev, &comm_tv,&comm_tv);

    //BEV_OPT_THREADSAFE support
    result = bufferevent_enable(bev, BEV_OPT_THREADSAFE);
    if(result < 0)
    {
        RPC_ERROR("RPCCOMM : enable bufferevent BEV_OPT_THREADSAFE failed!\n");
        bufferevent_free(bev);
        return ;
    }

    RPC_INFO("RPCCOMM : new bufferevent addr :%p\n",bev);
    // set readcb, writecb and errcb
    bufferevent_setcb(bev, readCallback, NULL, eventCallback, NULL);
    //bufferevent_setcb(bev, readCallback, writeCallback, eventCallback, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    if(event_handler)
        event_handler(RPCEventConnect, (void *)bev, NULL, 0);
}

void RPCComm::readCallback(struct bufferevent *bev, void *user_data)
{
    size_t read_len = 0;
    char *message = NULL;
    struct evbuffer *input = bufferevent_get_input(bev);
    size_t message_len = evbuffer_get_length(input);
    
    while(message_len > 0)
    {
        message = (char *)malloc(message_len);
        if(!message)
            continue;

        do
        {
            read_len = bufferevent_read(bev, message, message_len);
            RPC_DEBUG("RPCCOMM : recv %lu, %s .\n",read_len, message);
            if((read_len > 0) && event_handler)
                event_handler(RPCEventRecv, (void *)bev, (void *)message, read_len);

            message_len -= read_len;
            
            if(0 == message_len)
                break;
        }while(true);

        free(message);
    }
}

void RPCComm::writeCallback(struct bufferevent *bev, void *user_data)
{
    struct evbuffer *output = bufferevent_get_output(bev);

    if(evbuffer_get_length(output) == 0)
    {
        RPC_INFO("RPCCOMM : Output evbuffer is flushed.\n");
        
        if(event_handler)
            event_handler(RPCEventSend, (void *)bev, NULL, 0);
        
        return;
    }
}

void RPCComm::eventCallback(struct bufferevent *bev, short events, void *user_data)
{
    if(bev == (struct bufferevent *)user_data)  // connect action
    {
        if(events & BEV_EVENT_CONNECTED)  // connect ok
        {
            RPC_INFO("RPCCOMM : connecting to server OK!\n");

            pthread_mutex_lock(&connect_mutex);
            pthread_cond_signal(&connect_cond);
            pthread_mutex_unlock(&connect_mutex);

            // set communication timeout if needed
            bufferevent_set_timeouts(bev, &comm_tv, NULL);
            //bufferevent_set_timeouts(bev, &comm_tv, &comm_tv);

            // delete arg on callback
            bufferevent_setcb(bev, readCallback, NULL, eventCallback, NULL);
            
            if(event_handler)
                event_handler(RPCEventConnect, (void *)bev, NULL, 0);
            return ;
        }
        else if(events & BEV_EVENT_TIMEOUT)  // connect timeout
        {
            RPC_ERROR("RPCCOMM : connecting to server timeout!\n");
        }
        else
        {
            RPC_ERROR("RPCCOMM : connecting to server error:%s!\n",strerror(errno));
        }
        if(bev)
        {
            bufferevent_set_timeouts(bev, NULL, NULL);
            bufferevent_free(bev);
        }
        return ;
    }

    if(events & BEV_EVENT_TIMEOUT)
    {
        if(events & BEV_EVENT_READING)  // read timeout
        {
            RPC_WARN("RPCCOMM : read data from %p timeout!\n", bev);
            if(event_handler)
                event_handler(RPCEventRTimeout, (void *)bev, NULL, 0);

            // if timeout, event will disable read/write,so:
            if(!(bufferevent_get_enabled(bev) & EV_READ))
                bufferevent_enable(bev, EV_READ);
        }
        else if(events & BEV_EVENT_WRITING)
        {
            RPC_WARN("RPCCOMM : write data from %p timeout!\n", bev);
            if(event_handler)
                event_handler(RPCEventSTimeout, (void *)bev, NULL, 0);
            if(!(bufferevent_get_enabled(bev) & EV_WRITE))
                bufferevent_enable(bev, EV_WRITE);
        }
        return ;  // normal condition
    }

    else if(events & BEV_EVENT_EOF)
    {
        RPC_ERROR("RPCCOMM : Connection closed!\n");
    }
    else if(events & BEV_EVENT_ERROR)
    {
        RPC_ERROR("RPCCOMM : Got error on the connection:%s\n",strerror(errno));
        //RPC_ERROR("RPCCOMM : Got error on the connection:%s\n",evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
    }
    else
    {
        RPC_ERROR("RPCCOMM : Got unknown error on the connection:%s\n",strerror(errno));
        //RPC_ERROR("RPCCOMM : Got unknown error on the connection:%s\n",evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
    }

    // disconnect or an error
    if(event_handler)
        event_handler(RPCEventDisconnect, (void *)bev, NULL, 0);
    
    if(bev)
        bufferevent_free(bev);
}

void *RPCComm::commEventThread(void *arg)
{
    struct event_base *base = (struct event_base *)arg;
    
    RPC_INFO("RPCCOMM : communication event-loop id:%lu\n",pthread_self());
    RPC_INFO("RPCCOMM : communication event-base addr = 0x%p\n",base);
    
    event_base_dispatch(base);
    
    RPC_INFO("RPCCOMM : communication event-loop exit.\n");
    pthread_exit(NULL);
}

void *RPCComm::acceptEventThread(void *arg)
{
    struct event_base *base = (struct event_base *)arg;
    
    RPC_INFO("RPCCOMM : accept event-loop id:%lu\n",pthread_self());
    RPC_INFO("RPCCOMM : accept event-base addr = 0x%p\n",base);
    
    event_base_dispatch(base);
    
    RPC_INFO("RPCCOMM : accept event-loop exit.\n");
    pthread_exit(NULL);
}

