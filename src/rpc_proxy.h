/* rpc_proxy.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-20
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#ifndef RPC_PROXY_H__
#define RPC_PROXY_H__
#include <pthread.h>
#include <sys/time.h>

class RPCProxy
{
public:
    RPCProxy();
    ~RPCProxy();

    int init(void);
    void destroy(void);
    void lock(void);
    void unlock(void);
    void wakeup(void);
    int wait(struct timeval &tv);
    
    void setRequestMsg(void *msg);
    void *getRequestMsg(void);
    void setResponseMsg(void *msg);
    void *getResponseMsg(void);
    
private:
    void *request;
    void *response;
    pthread_attr_t attr;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
};

#endif // RPC_PROXY_H__