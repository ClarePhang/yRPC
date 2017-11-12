#include <unistd.h>
#include "socketclient.h"

int main(void)
{
    int ret = -1;
    SocketClient client;
    
    //ret = client.connectServer("/tmp/socket_test");
    ret = client.connectServer("127.0.0.1", 25000);
    if(ret < 0)
    {
        printf("connect server failed!\n");
        return -1;
    }

    //while(1)
    //{
    //    sleep(5);
    //}

    return 0;
}


