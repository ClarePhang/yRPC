#include <unistd.h>
#include "socketclient.h"

#include "event.h"

void timingSender(evutil_socket_t fd, short event, void *arg)
{
    struct timeval tv = {2,0};
    struct event *base = (struct event *)arg;
    
    printf("\nSend data timming.\n");
    evtimer_add(base, &tv);
}

int main(void)
{
    int ret = -1;
    struct timeval tv;
    struct event timeout;
    struct event_base *base;
    SocketClient client;
    
    //ret = client.connectServer("/tmp/socket_test");
    ret = client.connectServer("127.0.0.1", 25000);
    if(ret < 0)
    {
        printf("connect server failed!\n");
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


