/* rpc.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-22
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#ifndef RPC_H__
#define RPC_H__
#include <list>
#include <string>
#include <pthread.h>

#include "queue.h"
#include "uint_hash.h"
#include "socketbase.h"
#include "threadpool.h"
#include "comm_driver.h"
#include "string_hash.h"
#include "module_config.h"
#include "network_config.h"

using namespace std;

#define CONNECTRETRYDEFAULT     3

#define FIXTHREADDEFAULTNUM     5
#define DYNTHREADDEFAULTNUM     8
#define MAXQUEUEDEFAULTNUM      150

#define TIMEOUTDEFAULTSEC       0
#define TIMEOUTDEFAULTUSEC      30*1000

struct WorkerEntry{
    void *message;
    LIST_ENTRY(WorkerEntry) worker_next;
};
LIST_HEAD(WorkerHead, WorkerEntry);

typedef void (*ServerFunction)(void *arg);

class HSAERPC
{
public:
    HSAERPC();
    ~HSAERPC();

public:
    int setProcessName(const char *process);
    int setConfigPath(const string &network, const string &module);
    int addService(const char *server, ServerFunction func);
    int setResponseMsg(void *arg,void *response, size_t len);
    int proxyCall(const string &module, const string &func, void *send, size_t slen, void *recv, size_t &rlen);
    int start(void);
    int runUntilAskedToQuit(bool flag);

private:
    void addSendWorker(void *worker);
    
private:
    static void *sendThread(void *arg);
    static int sendLinkMessage(void *fdp);
    static int recvLinkMessage(void *fdp, void *msg);
    static int errorACKMessage(void *fdp, void *msg);
    static unsigned int getFrameID(void);
    static void releaseRPCMessage(void *arg);
    static int recvData(void *fd_ptr, const void *data, size_t data_len);
    static int eventHandler(unsigned int type, void *fd_ptr, void *data, size_t data_len);
    static void signalHandler(int signo);
    
private:
    pthread_t sendThreadID;
    
private:
    static bool initflag;
    static string process;
    static list<void *> m_list;
    static StringHash func_hash;
    static UINTHash proxy_hash;
    static COMMDriver rpc_comm_base;
    static unsigned int frame_id;
    static struct timeval commtv;
    static struct timeval conntv;
    static volatile bool run_flag;
    static StringHash process_hash;
    static struct WorkerHead w_head;
    static ThreadPool *rpc_threadpool;
    static SocketBaseOpt socket_base;
    static ModuleConfig module_config;
    static NetworkConfig network_config;
    static pthread_cond_t send_cond;
    static pthread_mutex_t send_mutex;
};

#endif  // RPC_H__

