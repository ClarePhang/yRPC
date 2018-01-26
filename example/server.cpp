#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "rpc.h"
#include "BTModule.h"
#include "MediaModule.h"

int main(int argc, char *argv[])
{
    BTModule btmodule;
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
    server->registerService("mediaPlay", media.mediaPlay);
    server->registerService("mediaStop", media.mediaStop);
    server->registerService("mediaPrev", media.mediaPrev);
    server->registerService("mediaNext", media.mediaNext);
    server->createObserver("mediaState");

    btmodule.setRPC(server);
    server->registerService("btPlay", btmodule.btPlay);
    server->registerService("btStop", btmodule.btStop);
    server->registerService("btPrev", btmodule.btPrev);
    server->registerService("btNext", btmodule.btNext);

    server->start();
    media.startMedisService();
    server->runUntilAskedToQuit(true);

    return 0;
}

