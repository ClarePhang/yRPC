#include <unistd.h>
#include <string.h>
#include "socketserver.h"

#include "event.h"

SocketServer client;

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
    client.sendData("server", reply_msg, strlen(reply_msg));
    msg_num++;
}

int main(void)
{
    int ret = -1;
    struct timeval tv;
    struct event timeout;
    struct event_base *base;

    //ret = client.serverCreate("/tmp/socket_test"); 
    ret = client.serverCreate("127.0.0.1", 25001);
    if(ret < 0)
    {
        printf("client create failed!\n");
        return -1;
    }
    
    //ret = client.connectServer("/tmp/socket_test");
    ret = client.connectServer("127.0.0.1", 25000);
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


