/* rpc_proxy.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-20
 * Author: zhangqiyin/Konishi
 * Email : zhangqiyin@hangsheng.com.cn
 */
#ifndef RPC_PROXY_H__
#define RPC_PROXY_H__
#include <pthread.h>

class ProxyInterface {
public:
    ProxyInterface();
    ~ProxyInterface();

    int init(void);
    void destroy(void);    
    int wait(struct timeval &tv);
    void wakeup(void);

    void setRequestMsg(void *msg);
    void *getRequestMsg(void);
    void setResponseMsg(void *msg);
    void *getResponseMsg(void);
    
private:
    void *request;
    void *response;
    pthread_cond_t cond;
    pthread_attr_t attr;
    pthread_mutex_t mutex;
};

#endif // RPC_PROXY_H__