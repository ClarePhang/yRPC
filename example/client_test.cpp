#include <unistd.h>
#include <string.h>

#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "event.h"

#include "rpc_comm.h"
#include "socketbase.h"


SocketBaseOpt socketbase;
RPCComm rpc_communication;

void * server_ptr = NULL;

void timingSender(evutil_socket_t fd, short event, void *arg)
{
    struct timeval tv = {2,0};
    static int msg_num = 1;
    char reply_msg[1000] = {'\0'};
    struct event *base = (struct event *)arg;
    char *str = (char *)"--------------------receive:";
    
    printf("\nSend data timming.\n");
    evtimer_add(base, &tv);

    memcpy(reply_msg, str, strlen(str));
    sprintf(reply_msg + strlen(str), "%d", msg_num);
    rpc_communication.send(server_ptr, reply_msg, strlen(reply_msg));
    msg_num++;
}

int event_handler(unsigned int type, void *fd_ptr, void *data, size_t data_len)
{
    struct timeval tv = {10, 0};
    switch(type)
    {
        case RPCEventRecv:
            printf("test: RPCEventRecv, %p\n", fd_ptr);
            printf("CommDriver : recv %lu, %s .\n",data_len, (char *)data);
            rpc_communication.changeCommTime(tv);
            break;
            
        case RPCEventSend:
            printf("test: RPCEventSend\n");
            break;
            
        case RPCEventConnect:
            printf("test: RPCEventConnect,%p\n", fd_ptr);
            server_ptr = fd_ptr;
            break;
            
        case RPCEventDisconnect:
            printf("test: RPCEventDisconnect,%p\n",fd_ptr);
            server_ptr = NULL;
            break;
            
        case RPCEventCheck:
            printf("test: RPCEventCheck\n");
            break;
            
        case RPCEventRTimeout:
            printf("test: RPCEventRTimeout, %p\n",fd_ptr);
            break;
            
        case RPCEventSTimeout:
            printf("test: RPCEventSTimeout\n");
            break;
            
        default:
            break;
    }
    
    return 0;
}

int main(void)
{
    int ret = -1;    
    struct timeval tv;
    struct event timeout;
    struct event_base *base;
    struct sockaddr_un u_addr;
    struct sockaddr_in s_addr;
    struct sockaddr_in s1_addr;

    socketbase.initSockaddr(s_addr, "127.0.0.1", 25000);
    socketbase.initSockaddr(s1_addr, "14.215.177.38", 2345);
    unlink("/tmp/socket_test");
    socketbase.initSockaddr(u_addr, "/tmp/socket_test");
    
    ret = rpc_communication.create(event_handler, (struct sockaddr *)&u_addr, sizeof(u_addr));
//    ret = rpc_communication.create(event_handler, (struct sockaddr *)&s_addr, sizeof(s_addr));
//    ret = rpc_communication.create(event_handler, (struct sockaddr *)&s1_addr, sizeof(s1_addr));
    if(ret < 0)
    {
        printf("client create failed!\n");
        return -1;
    }

    tv.tv_sec = 0;
    tv.tv_usec = 50*1000;
    ret = rpc_communication.connect((struct sockaddr *)&s_addr, sizeof(s_addr), tv);
    //ret = rpc_communication.connect((struct sockaddr *)&s1_addr, sizeof(s1_addr), tv);
    if(ret < 0)
    {
        printf("connect  server 127.0.0.1 failed!\n");
        return -1;
    }
    
    /* Initalize the event library */
    base = event_base_new();

    /* Initalize one event */
    evtimer_assign(&timeout, base, timingSender, (void*) &timeout);
    evutil_timerclear(&tv);
    tv.tv_sec = 2;
    event_add(&timeout, &tv);
    event_base_dispatch(base);

    return 0;
}


