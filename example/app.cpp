#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "rpc.h"
#include "MediaModuleInterface.h"

MediaModule media;

void stateChange(void *data, size_t len)
{
    if(data)
        printf("stateChanged : %s\n", (char *)data);
    else
        printf("state changed.\n");
}

void *business_thread(void *arg)
{
    int result = -1;
    
    sleep(1);

    media.mediaControl(playMedia, 9);
    
    result = media.registerStateHandler(stateChange);
    if(result == 0)
        printf("register state change handler OK.\n");
    else
        printf("register state change handler failed.\n");
    
    while(true)
    {
        sleep(10);
        result = media.mediaControl(stopMedia, 9);
        if(result == 0)
        {
            printf("Control media ok\n");
        }
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

