/* rpc_core.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-22
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#ifndef RPC_CORE_H__
#define RPC_CORE_H__
#include <string>
#include <pthread.h>

#include "rpc.h"
#include "queue.h"
#include "rpc_conf.h"
#include "uint_hash.h"
#include "threadpool.h"
#include "string_hash.h"
#include "comm_driver.h"
#include "pointer_list.h"
#include "rpc_observer.h"

using namespace std;

struct SendListEntry{
    void *message;
    LIST_ENTRY(SendListEntry) next;
};
LIST_HEAD(SendListHead, SendListEntry);

class RPCCore : public ERPC
{
public:
    static RPCCore *getInstance(void);
    virtual int initRPC(const string &process_name, const string &conf_path = "");
    virtual int registerService(const string &service, ServiceHandler func);
    virtual int unregisterService(const string &service);
    virtual int setResponse(void *msg, void *response_data, size_t response_len);
    virtual int proxyCall(const string &module, const string &func, void *send, size_t slen, void *recv, size_t *rlen, struct timeval *tv = NULL);
    virtual int start(void);
    virtual int runUntilAskedToQuit(bool state);

public: // observer function
    virtual int createObserver(const string &observer);
    virtual int destroyObserver(const string &observer);
    virtual int invokeObserver(const string &observer, void *data, size_t len);
    virtual int registerObserver(const string &module, const string &observer, ObserverHandler func, struct timeval *tv = NULL);
    virtual int unregisterObserver(const string &module, const string &observer, struct timeval *tv = NULL);

private:
    RPCCore();
    virtual ~RPCCore();

private:
    static void signalHandler(int signo);
    static void callBusinessHandler(void *msg);
    static int registerObserverHandler(void *fdp, void *msg);
    static int unregisterObserverHandler(void *fdp, void *msg);
    static int commEventHandler(unsigned int type, void *fdp, void *data, size_t len);

private:
    static int requestLink(void *fdp, void *msg);
    static int responseLink(void *fdp, void *msg);
    static int insertSenderNode(void *message);
    static int connectToProcess(string process, void **connect_fd);
    static void *RPCCoreThread(void *arg);
    
    static int wakeupOneThread(void *msg);
    static int responseErrorCodeACK(void *fdp, void *msg);
    
    static unsigned int getFrameID(void);
    static void releaseRPCMessage(void *arg);
    
    static int analyseReceiveData(void *fdp, const void *data, size_t len);
    
private:
    static bool m_run_state;
    static bool m_conf_state;
    static RPCCore *m_rpc_core;
    pthread_t m_core_thread_id;
    static string m_process_name;
    static COMMDriver m_comm_base;
    static ThreadPool *m_threadpool;
    static struct timeval m_conn_tv;
    static struct timeval m_comm_tv;
    static pthread_cond_t m_send_cond;
    static pthread_mutex_t m_send_mutex;
    static pthread_mutex_t m_link_mutex;
    static pthread_mutex_t m_frame_mutex;
    static struct SendListHead m_send_head;
    static RPCConfig *m_conf_file;
    static ProcessConfig m_process_conf;
    static UINTHash m_proxy_hash;
    static StringHash m_service_func_hash;
    static StringHash m_observer_func_hash;
    static ObserverHash m_observer_connect_hash;
    static StringHash m_process_connect_hash;
    static PointerList m_process_connect_list;
    static volatile unsigned int m_frame_id;
};

#endif // RPC_CORE_H__

