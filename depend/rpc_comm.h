/* rpc_comm.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-20
 * Author: zhangqiyin/Konishi
 * Email : zhangqiyin@hangsheng.com.cn
 */

#ifndef RPC_COMM_H__
#define RPC_COMM_H__
#include "event.h"
#include "event2/util.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/thread.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"

#define RPC_COMM_VERSION    "V1.0"

#define RPCEventRecv        0x01
#define RPCEventSend        0x02
#define RPCEventConnect     0x04
#define RPCEventDisconnect  0x08
#define RPCEventCheck       0x10
#define RPCEventRTimeout    0x20
#define RPCEventSTimeout    0x40
#define RPCEventReserved    0x80

typedef int (*EventHandler)(unsigned int type, void *fd_ptr, void *data, size_t data_len);

class RPCComm
{
public:
    RPCComm();
    ~RPCComm();

public:
    void showVersion(void);  // show third software version
    void showMethods(void);  // show the way to deal with file description
    void setCommTime(struct timeval &tv);
    void setCheckTime(struct timeval &tv);

public:
    int create(EventHandler handler, struct sockaddr *s_addr, size_t s_len);
    void destroy(void);
    int send(const void *fdptr, const void *data, size_t size);

public:
    int connect(struct sockaddr *s_addr, size_t s_len, struct timeval &tv);
    void disconnect(void *fdptr);

private:
    static void cycleCheckCallback(evutil_socket_t fd, short event, void *arg);
    static void listenerCallback(struct evconnlistener *listener, evutil_socket_t fd,
                                struct sockaddr *sa, int socklen, void *user_data);
    static void readCallback(struct bufferevent *bev, void *user_data);
    static void writeCallback(struct bufferevent *bev, void *user_data);
    static void eventCallback(struct bufferevent *bev, short events, void *user_data);
    static void *commEventThread(void *arg);
    static void *acceptEventThread(void *arg);
    
private:
    pthread_t comm_thread_id;
    pthread_t accept_thread_id;
    struct event *check_event;
    struct event_base *comm_base;
    struct event_base *accept_base;
    struct evconnlistener *evcon_listener;

private:
    static struct timeval comm_tv;
    static struct timeval check_tv;
    static EventHandler event_handler;
    static pthread_cond_t connect_cond;
    static pthread_mutex_t connect_mutex;
};

#endif // RPC_COMM_H__

