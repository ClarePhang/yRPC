#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "rpc.h"
#include "app.h"

int main(int argc, char *argv[])
{
    APPView app;
    int result = -1;
    ERPC *server = NULL;

    server = ERPC::getInstance();
    if(NULL == server)
    {
        printf("malloc RPC core failed!\n");
        return -1;
    }
    server->initRPC(argv[0], "../conf/rpc.conf");

    server->start();

    result = app.startBussiness(server);

    if(result < 0)
        server->runUntilAskedToQuit(false);
    else
        server->runUntilAskedToQuit(true);

    return 0;
}

