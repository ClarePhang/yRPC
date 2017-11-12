#include <unistd.h>
#include "socketserver.h"

int main(void)
{
    int ret = -1;
    SocketServer server;
    
    //ret = server.serverCreate("/tmp/socket_test");
    ret = server.serverCreate("127.0.0.1", 25000);
    if(ret < 0)
    {
        printf("server create failed!\n");
        return -1;
    }

    while(1)
    {
        sleep(5);
    }

    return 0;
}

