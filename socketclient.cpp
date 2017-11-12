/* socketserver.cpp
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-09
 * Author: zhangqiyin/Konishi
 * Email : zhangqiyin@hangsheng.com.cn
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//#include <signal.h>

#include "socketclient.h"

volatile SocketMethod SocketClient::socket_method = NOSOCKET;

SocketClient::SocketClient()
{
    client_base = NULL;
}

SocketClient::~SocketClient()
{
    client_base = NULL;
}

int SocketClient::connectServer(const char *server)
{
    struct sockaddr_un su_addr;

    if(initSockaddr(su_addr, server) < 0)
        goto CONNECT_FAILED;

    if(initEventBase() < 0)
        goto CONNECT_FAILED;

    if(startConnect((struct sockaddr *)&su_addr,sizeof(su_addr)) < 0)
        goto CONNECT_FAILED;
    
    //if(createThread() < 0)
    //    goto CONNECT_FAILED;

    socket_method = TCPSOCKET;

    // just for test   notice
    event_base_dispatch(client_base);
    printf("dispath exit.\n");
    event_base_free(client_base);
    
    return 0;

CONNECT_FAILED:
    deinitEventBase();
    return -1;
}

int SocketClient::connectServer(const char *server, unsigned int port)
{
    struct sockaddr_in si_addr;

    if(initSockaddr(si_addr, server, port) < 0)
        goto CONNECT_FAILED;

    if(initEventBase() < 0)
        goto CONNECT_FAILED;

    if(startConnect((struct sockaddr *)&si_addr,sizeof(si_addr)) < 0)
        goto CONNECT_FAILED;

    //if(createThread() < 0)
    //    goto CONNECT_FAILED;

    socket_method = TCPSOCKET;

    // just for test   notice
    event_base_dispatch(client_base);
    printf("dispath exit.\n");
    event_base_free(client_base);
    
    return 0;
    
CONNECT_FAILED:
    deinitEventBase();
    return -1;
}

int SocketClient::initEventBase(void)
{
    // if env set, then open debug   notice
    event_enable_debug_mode();
    event_enable_debug_logging(0);

    evthread_use_pthreads();  //enable threads

    client_base = event_base_new();
    if(!client_base)
    {
        printf("Client : new client_event_base failed!\n");
        return -1;
    }

    evthread_make_base_notifiable(client_base);
    
    return 0;
}

void SocketClient::deinitEventBase(void)
{
    if(client_base)
        client_base = NULL;
}

int SocketClient::startConnect(struct sockaddr *s_addr, size_t s_len)
{
    int result = -1;
    struct bufferevent *bev = NULL;
    struct timeval read_timeout = {5, 0};
    struct timeval write_timeout = {4, 0};

    bev = bufferevent_socket_new(client_base, -1, BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE);
    if(!bev)
    {
        printf("Client : new bufferevent_socket failed!\n");
        goto CONNECT_FAILED;
    }

    bufferevent_setcb(bev, readCallback, writeCallback, eventCallback, bev);
    bufferevent_set_timeouts(bev, &read_timeout, &write_timeout);
    result = bufferevent_socket_connect(bev, s_addr, s_len);
    if( -1 == result)
    {
        printf("Connect failed!\n");
        goto CONNECT_FAILED;
    }

    bufferevent_enable(bev , EV_READ|EV_WRITE);
    
    return 0;

CONNECT_FAILED:
    if(bev)
        bufferevent_free(bev);
    return -1;
}

void SocketClient::readCallback(struct bufferevent *bev, void *user_data)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    size_t sz = evbuffer_get_length(input);
    
    printf("\nClient : readCallback thread: %lu\n",pthread_self());
    printf("Client : bufferevent addr :%p\n",bev);

    if(sz > 0)
    {
        // here use c++ virtual function   notice
        char msg[1024] = {'\0'};
        int readlen = 1024 > sz ? sz : 1024;
        bufferevent_read(bev, msg, readlen);
        printf("Client : recv %d, %s .\n",readlen, msg);
    }
}

void SocketClient::writeCallback(struct bufferevent *bev, void *user_data)
{
    struct evbuffer *output = bufferevent_get_output(bev);

    printf("\nClient : writeCallback thread: %lu\n",pthread_self());
    printf("Client : bufferevent addr :%p\n",bev);
    
    if(evbuffer_get_length(output) == 0)
    {
        printf("Client : Output evbuffer is flushed.\n");
        return;
    }
}

void SocketClient::eventCallback(struct bufferevent *bev, short events, void *user_data)
{
    printf("\nClient : eventCallback thread: %lu\n",pthread_self());
    printf("Client : bufferevent addr :%p\n",bev);

    if(events & BEV_EVENT_EOF)
    {
        printf("Client : Connection closed!\n");
    }
    else if(events & BEV_EVENT_CONNECTED)
    {
        printf("Client : New connection finishi!\n");
        return ;  // normal condition
    }
    else if(events & BEV_EVENT_ERROR)
    {
        printf("Client : Got error on the connection:%s\n",strerror(errno));
        //printf("Client : Got error on the connection:%s\n",evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
    }
    else if(events & BEV_EVENT_TIMEOUT)
    {
        if(events & BEV_EVENT_READING)  // read timeout
        {
            printf("Client : read data from %p timeout!\n", bev);
        }
        else if(events & BEV_EVENT_WRITING)
        {
            printf("Client : write data from %p timeout!\n", bev);
        }
        // if timeout, event will disable read/write,so:
        if(!(bufferevent_get_enabled(bev) & EV_READ))
        {
            printf("Client : reenable %p readable.\n",bev);
            bufferevent_enable(bev, EV_READ);
        }
        if(!(bufferevent_get_enabled(bev) & EV_WRITE))
        {
            printf("Client : reenable %p writeable.\n",bev);
            bufferevent_enable(bev, EV_WRITE);
        }
        return ;  // normal condition
    }
    else
    {
        printf("Client : Got unknown error on the connection:%s\n",strerror(errno));
        //printf("Client : Got unknown error on the connection:%s\n",evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
    }

    if(bev)
        bufferevent_free(bev);
}

