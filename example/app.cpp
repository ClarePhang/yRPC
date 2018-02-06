#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "rpc.h"
#include "MediaModule.h"

MediaModule *media = NULL;

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
    float timeuse = 0.0;
    unsigned int count = 0;
    struct timeval start, end;
    
    sleep(1);
    printf("11111111111\n");
    media->mediaControl(playMedia, 9);
    printf("22222222222\n");
    result = media->registerStateHandler(stateChange);
    if(result == 0)
        printf("register state change handler OK.\n");
    else
        printf("register state change handler failed.\n");
    
    while(true)
    {
        sleep(5);
        result = media->mediaControl(stopMedia, 9);
        if(result == 0)
        {
            printf("Control media ok\n");
        }

        gettimeofday(&start, NULL);
        for(count = 0; count < 10000; count++)
        {
            media->mediaControl(prevMedia, 3);
        }
        gettimeofday(&end, NULL);
        timeuse = (1000000*(end.tv_sec - start.tv_sec) + (float)(end.tv_usec - start.tv_usec))/1000;
        printf("--------- 1000 times call -----------\n");
        printf("--------- timeuse:%2.03f ms ---------\n",timeuse);
    }
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int result = -1;
    ERPC *server = NULL;
    pthread_t business_id;

    // 1. get RPC instance
    server = ERPC::getInstance();
    if(NULL == server)
    {
        printf("malloc RPC core failed!\n");
        return -1;
    }

    // 2. get Module instance
    media = MediaModule::getInstance();
    if(NULL == media)
    {
        printf("malloc media module failed!\n");
        return -1;
    }
    
    // 3. init RPC
    server->initRPC(argv[0], "../conf/rpc.conf");

    // 4.start RPC framework
    server->start();

    // 5.start app business
    result = pthread_create(&business_id, NULL, business_thread, NULL);
    if(result != 0)
        server->runUntilAskedToQuit(false);
    else
        server->runUntilAskedToQuit(true);

    return 0;
}

