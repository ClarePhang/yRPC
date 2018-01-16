/* rpc_comm.cpp
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-20
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
     
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "comm_driver.h"

#include "event.h"
#include "event2/util.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/thread.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"

#define K_DEBUG   printf
#define K_INFO    printf
#define K_WARN    printf
#define K_ERROR   printf

#define DEFAULT_CHECK_CYCLE     15  // check socket status every 15 second
#define DEFAULT_COMM_TIMEOUT    200 // communication timeout 200 ms
#define DEFAULT_CONNECT_TIMEOUT 300 // default connect timeout is 300 ms

#define tp_tvaddtp(tv, tp, ttp)                                 \
    do {                                                        \
        (ttp)->tv_sec = (tv)->tv_sec + (tp)->tv_sec;            \
        (ttp)->tv_nsec = ((tv)->tv_usec * 1000)+ (tp)->tv_nsec; \
        if ((ttp)->tv_nsec >= 1000000000) {                     \
            (ttp)->tv_sec++;                                    \
            (ttp)->tv_nsec -= 1000000000;                       \
        }                                                       \
    } while (0)

#define STATIC  static

STATIC bool cycle_check_flag = false;
STATIC struct event *cycle_check_event = NULL;
STATIC struct event_base *event_main_base = NULL;
STATIC struct event_base *event_accept_base = NULL;
STATIC struct evconnlistener *evcon_listener = NULL;
STATIC struct timeval cycle_check_tv = {DEFAULT_CHECK_CYCLE, 0};
STATIC struct timeval comm_timeout_tv = {0, DEFAULT_COMM_TIMEOUT*1000};

STATIC pthread_cond_t comm_connect_cond = PTHREAD_COND_INITIALIZER;
STATIC pthread_mutex_t comm_connect_mutex = PTHREAD_MUTEX_INITIALIZER;

STATIC CommEventHandler comm_event_handler_ptr = NULL;

STATIC void cycle_check_handler(evutil_socket_t fd, short event, void *arg)
{
    if(!cycle_check_event)
        return ;
    
    evtimer_add(cycle_check_event, &cycle_check_tv);
    
    if(cycle_check_flag && comm_event_handler_ptr)
        comm_event_handler_ptr(COMMEventCheck, NULL, NULL, 0);
}

STATIC void comm_read_handler(struct bufferevent *bev, void *user_data)
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
//            K_DEBUG("COMM : recv %lu, %s .\n",read_len, message);
            if((read_len > 0) && comm_event_handler_ptr)
                comm_event_handler_ptr(COMMEventRecv, (void *)bev, (void *)message, read_len);
            
            message_len -= read_len;
            if(0 == message_len)
                break;
        }while(true);

        free(message);
    }
}

#ifdef USING_WRITE_HANDLER
STATIC void comm_write_handler(struct bufferevent *bev, void *user_data)
{
    struct evbuffer *output = bufferevent_get_output(bev);

    if(evbuffer_get_length(output) == 0)
    {
        K_INFO("COMM : Output evbuffer is flushed.\n");
        
        if(comm_event_handler_ptr)
            comm_event_handler_ptr(COMMEventSend, (void *)bev, NULL, 0);
        
        return;
    }
}
#endif

STATIC void comm_event_handler(struct bufferevent *bev, short events, void *user_data)
{
    if(bev == (struct bufferevent *)user_data)  // connect events
    {
        if(events & BEV_EVENT_CONNECTED)  // connect ok
        {
            K_INFO("COMM : connecting to server OK!\n");

            pthread_mutex_lock(&comm_connect_mutex);
            pthread_cond_signal(&comm_connect_cond);
            pthread_mutex_unlock(&comm_connect_mutex);

            // set communication timeout if needed
            #ifndef USING_COMM_TIMEOUT
            bufferevent_set_timeouts(bev, NULL, NULL);
            #else
            bufferevent_set_timeouts(bev, &comm_timeout_tv, NULL);
            //bufferevent_set_timeouts(bev, &comm_timeout_tv, &comm_timeout_tv);
            #endif
            
            // delete arg on callback
            #ifdef USING_WRITE_HANDLER
            bufferevent_setcb(bev, comm_read_handler, comm_write_handler, comm_event_handler, NULL);
            #else
            bufferevent_setcb(bev, comm_read_handler, NULL, comm_event_handler, NULL);
            #endif
            if(comm_event_handler_ptr)
                comm_event_handler_ptr(COMMEventConnect, (void *)bev, NULL, 1);
            return ;
        }
        else if(events & BEV_EVENT_TIMEOUT)  // connect timeout
        {
            K_ERROR("COMM : connecting to server timeout!\n");
        }
        else
        {
            K_ERROR("COMM : connecting to server error:%s!\n",strerror(errno));
        }
    }
    else // other event in using
    {
        if(events & BEV_EVENT_TIMEOUT)
        {
            if(events & BEV_EVENT_READING)  // read timeout
            {
                K_WARN("COMM : read data from %p timeout!\n", bev);
                if(comm_event_handler_ptr)
                    comm_event_handler_ptr(COMMEventRTimeout, (void *)bev, NULL, 0);

                // if timeout, event will disable read/write,so:
                if(!(bufferevent_get_enabled(bev) & EV_READ))
                    bufferevent_enable(bev, EV_READ);
            }
            else if(events & BEV_EVENT_WRITING)
            {
                K_WARN("COMM : write data from %p timeout!\n", bev);
                if(comm_event_handler_ptr)
                    comm_event_handler_ptr(COMMEventSTimeout, (void *)bev, NULL, 0);
                if(!(bufferevent_get_enabled(bev) & EV_WRITE))
                    bufferevent_enable(bev, EV_WRITE);
            }
            return ;  // timeout in use is normal condition
        }
        else if(events & BEV_EVENT_EOF)
        {
            K_ERROR("COMM : Connection closed!\n");
        }
        else if(events & BEV_EVENT_ERROR)
        {
            K_ERROR("COMM : Got error on the connection:%s\n",strerror(errno));
            //K_ERROR("COMM : Got error on the connection:%s\n",evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
        }
        else
        {
            K_ERROR("COMM : Got unknown error on the connection:%s\n",strerror(errno));
            //K_ERROR("COMM : Got unknown error on the connection:%s\n",evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
        }

        // disconnect or an error
        if(comm_event_handler_ptr)
            comm_event_handler_ptr(COMMEventDisconnect, (void *)bev, NULL, 0);
    }

    if(bev)
    {
        bufferevent_set_timeouts(bev, NULL, NULL);
        bufferevent_free(bev);
    }
}

STATIC void comm_listener_handler(struct evconnlistener *listener, evutil_socket_t fd,
                                  struct sockaddr *sa, int socklen, void *user_data)
{
    int result = -1;
    struct bufferevent *bev = NULL;
    
    K_INFO("COMM : New connecting from %s:%d\n",inet_ntoa(((sockaddr_in *)sa)->sin_addr),
           ntohs(((sockaddr_in *)sa)->sin_port));
    
    bev = bufferevent_socket_new(event_main_base, fd, LEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE);
    if(!bev)
    {
        K_ERROR("COMM : Could not create new bufferevent!\n");
        return ;
    }

    // set communication timeout if needed
    #ifndef USING_COMM_TIMEOUT
    bufferevent_set_timeouts(bev, NULL, NULL);
    #else
    bufferevent_set_timeouts(bev, &comm_timeout_tv, NULL);
    //bufferevent_set_timeouts(bev, &comm_timeout_tv, &comm_timeout_tv);
    #endif
    
    //BEV_OPT_THREADSAFE support
    result = bufferevent_enable(bev, BEV_OPT_THREADSAFE);
    if(result < 0)
    {
        K_ERROR("COMM : enable bufferevent BEV_OPT_THREADSAFE failed!\n");
        bufferevent_free(bev);
        return ;
    }

    K_INFO("COMM : new bufferevent addr :%p\n",bev);
    
    // set readcb, writecb and errcb
    #ifdef USING_WRITE_HANDLER
    bufferevent_setcb(bev, comm_read_handler, comm_write_handler, comm_event_handler, NULL);
    #else
    bufferevent_setcb(bev, comm_read_handler, NULL, comm_event_handler, NULL);
    #endif
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    if(comm_event_handler_ptr)
        comm_event_handler_ptr(COMMEventConnect, (void *)bev, NULL, 0);
}

STATIC void *comm_main_threading(void *arg)
{
    K_INFO("COMM : communication event-loop id:%lu\n",pthread_self());
    
    event_base_dispatch(event_main_base);
    
    K_INFO("COMM : communication event-loop exit.\n");
    pthread_exit(NULL);
}

STATIC void *comm_accept_threading(void *arg)
{
    K_INFO("COMM : accept event-loop id:%lu\n",pthread_self());
    
    event_base_dispatch(event_accept_base);
    
    K_INFO("COMM : accept event-loop exit.\n");
    pthread_exit(NULL);
}

void COMMDriver::version(void)
{
    K_INFO("COMM : version: %s\n",COMM_VERSION);
    K_INFO("COMM : Third software :libevent %s\n", event_get_version());
}

void COMMDriver::methods(void)
{
    const char **a = event_get_supported_methods();
    K_INFO("COMM : System supported methods:\n");
    for(int i = 0; a[i] != NULL; i++)
        K_INFO(" [%d] : %s\n", i, a[i]);
}

void COMMDriver::setTimeout(struct timeval &tv)
{
    if((tv.tv_sec == 0) && (tv.tv_usec == 0))
    {
        comm_timeout_tv.tv_sec = 0;
        comm_timeout_tv.tv_usec = DEFAULT_COMM_TIMEOUT*1000;
    }
    else
    {
        comm_timeout_tv.tv_sec = tv.tv_sec;
        comm_timeout_tv.tv_usec = tv.tv_usec;
    }
}

void COMMDriver::setCyclecheck(struct timeval &tv)
{
    if((tv.tv_sec == 0) && (tv.tv_usec == 0))
    {
        cycle_check_tv.tv_sec = DEFAULT_CHECK_CYCLE;
        cycle_check_tv.tv_usec = 0;
    }
    else
    {
        cycle_check_tv.tv_sec = tv.tv_sec;
        cycle_check_tv.tv_usec = tv.tv_usec;
    }
}

void COMMDriver::cyclecheckEn(bool enable)
{
    cycle_check_flag = enable;
}


// para: s_addr can be a sockaddr_in or sockaddr_un, s_len is the length of it.
int COMMDriver::create(CommEventHandler handler, struct sockaddr *s_addr, size_t s_len)
{
    char *debug_conf = NULL;
    
    debug_conf = getenv(COMM_DEBUE);
    if(debug_conf != NULL)
    {
        event_enable_debug_mode();
        event_enable_debug_logging(EVENT_DBG_ALL);
        //event_enable_debug_logging(EVENT_DBG_NONE);
    }
    
    evthread_use_pthreads();  //enable threads
    
    event_main_base = event_base_new();
    if(!event_main_base)
    {
        K_ERROR("COMM : new communication event base failed!\n");
        goto INIT_EVENT_FAILED;
    }
    
    event_accept_base = event_base_new();
    if(!event_accept_base)
    {
        K_ERROR("COMM : new accept event base failed!\n");
        goto INIT_EVENT_FAILED;
    }
    
    evthread_make_base_notifiable(event_main_base);
    evthread_make_base_notifiable(event_accept_base);

    cycle_check_event = (struct event *)malloc(sizeof(struct event));
    if(!cycle_check_event)
    {
        K_ERROR("COMM : malloc cycle check event failed!\n");
        goto MALLOC_EVENT_FAILED;
        return -1;
    }

    evcon_listener = evconnlistener_new_bind(event_accept_base, comm_listener_handler, NULL,
        LEV_OPT_REUSEABLE|LEV_OPT_REUSEABLE_PORT|LEV_OPT_THREADSAFE|BEV_OPT_CLOSE_ON_FREE,
        -1, s_addr, s_len);
    if(!evcon_listener)
    {
        K_ERROR("COMM : new evconnlistener bind failed!\n");
        goto MALLOC_EVENT_FAILED;
    }

    if(evtimer_assign(cycle_check_event, event_main_base, cycle_check_handler, NULL) < 0)
    {
        K_ERROR("COMM : assign event-timer failed!\n");
        goto MALLOC_EVENT_FAILED;
    }

    if(evtimer_add(cycle_check_event, &cycle_check_tv) < 0)
    {
        K_ERROR("COMM : add event-timer failed!\n");
        goto MALLOC_EVENT_FAILED;
    }

    comm_event_handler_ptr = handler;
    
    if(pthread_create(&comm_main_thread_id, NULL, comm_main_threading, NULL) != 0)
    {
        K_ERROR("COMM : pthread_create failed, errno:%d,error:%s.\n", errno, strerror(errno));
        goto CREATE_THREAD_FAILED;
    }
    if(pthread_create(&comm_accept_thread_id, NULL, comm_accept_threading, NULL) != 0)
    {
        K_ERROR("COMM : pthread_create failed, errno:%d,error:%s.\n", errno, strerror(errno));
        goto CREATE_THREAD_FAILED;
    }

    K_INFO("COMM : we use %s method\n",event_base_get_method(event_accept_base));

    return 0;
CREATE_THREAD_FAILED:
    if(comm_accept_thread_id != 0)
    {
        event_base_loopexit(event_accept_base, NULL);
        pthread_join(comm_accept_thread_id, NULL);
        comm_accept_thread_id = 0;
    }
    if(comm_main_thread_id != 0)
    {
        event_base_loopexit(event_main_base, NULL);
        pthread_join(comm_main_thread_id, NULL);
        comm_main_thread_id = 0;
    }
MALLOC_EVENT_FAILED:
    if(evcon_listener)
    {
        evconnlistener_free(evcon_listener);
        evcon_listener = NULL;
    }
    if(cycle_check_event)
    {
        //evtimer_del(cycle_check_event);  //here not need
        free(cycle_check_event);
        cycle_check_event = NULL;
    }
INIT_EVENT_FAILED:
    if(event_main_base)
    {
        event_base_free(event_main_base);
        event_main_base = NULL;
    }
    if(event_accept_base)
    {
        event_base_free(event_accept_base);
        event_accept_base = NULL;
    }
    return -1;
}

void COMMDriver::destroy(void)
{
    if(comm_accept_thread_id != 0)
    {
        event_base_loopexit(event_accept_base, NULL);
        pthread_join(comm_accept_thread_id, NULL);
        comm_accept_thread_id = 0;
    }
    if(comm_main_thread_id != 0)
    {
        event_base_loopexit(event_main_base, NULL);
        pthread_join(comm_main_thread_id, NULL);
        comm_main_thread_id = 0;
    }
 
    if(evcon_listener)
    {
        evconnlistener_free(evcon_listener);
        evcon_listener = NULL;
    }
    if(cycle_check_event)
    {
        evtimer_del(cycle_check_event);
        free(cycle_check_event);
        cycle_check_event = NULL;
    }
    
    if(event_main_base)
    {
        event_base_free(event_main_base);
        event_main_base = NULL;
    }
    if(event_accept_base)
    {
        event_base_free(event_accept_base);
        event_accept_base = NULL;
    }
}

int COMMDriver::connect(struct sockaddr *s_addr, size_t s_len, struct timeval &tv, void **fdptr)
{
    int result = -1;
    struct timespec outtime;
    struct bufferevent *bev = NULL;
    struct timeval c_tv = {0, 20*1000};  // add 20 ms for connect wait

    bev = bufferevent_socket_new(event_main_base, -1, BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE);
    if(!bev)
    {
        K_ERROR("COMM : new bufferevent_socket failed!\n");
        goto CONNECT_FAILED;
    }

    if((tv.tv_sec == 0) && (tv.tv_usec == 0))
        tv.tv_usec = DEFAULT_CONNECT_TIMEOUT*1000;
    
    bufferevent_setcb(bev, NULL, NULL, comm_event_handler, bev);
    result = bufferevent_socket_connect(bev, s_addr, s_len);
    if( -1 == result)
    {
        K_ERROR("COMM : Connect to %s failed, %s!\n", inet_ntoa(((sockaddr_in *)s_addr)->sin_addr), evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
        goto CONNECT_FAILED;
    }
    bufferevent_set_timeouts(bev, &tv, NULL);
    bufferevent_enable(bev , EV_READ|EV_WRITE);

    // wait connect back
    pthread_mutex_lock(&comm_connect_mutex);
    clock_gettime(CLOCK_REALTIME,&outtime);
    evutil_timeradd(&tv, &c_tv, &c_tv);
    tp_tvaddtp(&c_tv, &outtime, &outtime);
    result = pthread_cond_timedwait(&comm_connect_cond, &comm_connect_mutex, &outtime);
    pthread_mutex_unlock(&comm_connect_mutex);
    if(ETIMEDOUT == result)
    {
        K_ERROR("COMM : connect to %s timeout!\n",inet_ntoa(((sockaddr_in *)s_addr)->sin_addr));
        goto CONNECT_FAILED;
    }
    else if(result < 0)
    {
        K_ERROR("COMM : connect to %s error, %s!\n",inet_ntoa(((sockaddr_in *)s_addr)->sin_addr), strerror(errno));
        goto CONNECT_FAILED;
    }

    if(fdptr)
        *fdptr = (void *)bev;
    
    return 0;

CONNECT_FAILED:
    return -1;
}

void COMMDriver::disconnect(void *fdptr)
{
    struct bufferevent *bev = (struct bufferevent *)fdptr;
    if(bev)
    {
        bufferevent_set_timeouts(bev, NULL, NULL);
        bufferevent_free(bev);
    }
}

int COMMDriver::send(const void *fdptr, const void *data, size_t size)
{
    struct bufferevent * bev = (struct bufferevent *)fdptr;
    
    if(bev)
    {
        //K_DEBUG("COMM : send %lu, %s\n", size, (char *)data);
        return bufferevent_write(bev, data, size);
    }
    else
    {
        K_WARN("COMM : send bufferevent is NULL!\n");
    }
    
    return -1;
}

