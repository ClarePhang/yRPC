#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "rpc.h"
#include "MediaModuleInterface.h"

MediaModule media;

void *business_thread(void *arg)
{
    while(true)
    {
        sleep(10);
        media.mediaControl(playMedia, 9);
    }
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int result = -1;
    ERPC *server = NULL;
    pthread_t business_id;

    server = ERPC::getInstance();
    if(NULL == server)
    {
        printf("malloc RPC core failed!\n");
        return -1;
    }

    server->initRPC(argv[0], "../conf/rpc.conf");

    media.setRPC(server);
    
    server->start();

    result = pthread_create(&business_id, NULL, business_thread, NULL);
    if(result != 0)
        server->runUntilAskedToQuit(false);
    else
        server->runUntilAskedToQuit(true);

    return 0;
}

