#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "rpc.h"
#include "MediaModuleImplement.h"

int main(int argc, char *argv[])
{
    MediaModule media;
    ERPC *server = NULL;

    // RPC init
    server = ERPC::getInstance();
    if(NULL == server)
    {
        printf("malloc RPC core failed!\n");
        return -1;
    }
    
    server->initRPC(argv[0], "../conf/rpc.conf");

    media.setRPC(server);
    server->registerService("mediaControl", media.mediaControl);
    server->createObserver("mediaState");

    server->start();
    media.startMedisBusiness();
    server->runUntilAskedToQuit(true);

    return 0;
}

