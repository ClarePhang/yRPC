/* rpc_core.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-22
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#ifndef RPC_CORE_H__
#define RPC_CORE_H__
#include <string>
#include <vector>
#include <pthread.h>

#include "queue.h"
#include "uint_hash.h"
#include "threadpool.h"
#include "string_hash.h"
#include "comm_driver.h"
#include "pointer_list.h"
#include "module_config.h"
#include "network_config.h"

using namespace std;

struct WorkerEntry{
    void *message;
    LIST_ENTRY(WorkerEntry) worker_next;
};
LIST_HEAD(WorkerHead, WorkerEntry);

#undef ANALYSERECEIVE
#ifdef ANALYSERECEIVE
enum AnalysePackage
{
    AP_INIT = 0,
    AP_HEAD = 1,
    AP_BODY_HEAD = 2,
    AP_BODY_DATA = 3,
};
#endif

typedef void (*ServerFunction)(void *arg);

class RPCCore
{
public:
    static RPCCore *getInstance(void);
    int setProcessName(const char *process);
    int setConfigProfile(const string &network, const string &module);
    int addService(const char *service, ServerFunction func);
    int setResponse(void *msg, void *response_data, size_t response_len);
    int proxyCall(const string &module, const string &func, void *send, size_t slen, void *recv, size_t &rlen);
    int start(void);
    int runUntilAskedToQuit(bool state);
    
private:
    RPCCore();
    ~RPCCore();

private:
    static void *sendThread(void *arg);
    static unsigned int getFrameID(void);
    static int sendLinkMessage(void *fdp);
    static int recvLinkMessage(void *fdp, void *msg);
    static int errorACKMessage(void *fdp, void *msg);
    static void releaseRPCMessage(void *arg);
    static void signalHandler(int signo);
    static void addSendWorker(void *worker);
    static int analyseReceiveData(void *fdp, const void *data, size_t len);
    static int eventHandler(unsigned int type, void *fd_ptr, void *data, size_t data_len);

private:
    pthread_t m_send_thread_id;

private:
    static RPCCore *m_rpc;
    static string m_process;
    static bool m_run_state;
    static bool m_conf_state;
    static UINTHash m_proxy_hash;
    static StringHash m_func_hash;
    static COMMDriver m_comm_base;
    static ThreadPool *m_threadpool;
    static struct timeval m_conn_tv;
    static struct timeval m_comm_tv;
    static StringHash m_connect_hash;
    static PointerList m_connect_list;
    static ModuleConfig module_config;
    static NetworkConfig network_config;
    static pthread_cond_t m_send_cond;
    static pthread_mutex_t m_send_mutex;
    static struct WorkerHead m_work_head;
    static volatile unsigned int m_frame_id;
};

#endif // RPC_CORE_H__

