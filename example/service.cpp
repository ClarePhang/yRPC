#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "rpc.h"
#include "MediaModule.h"

int main(int argc, char *argv[])
{
    ERPC *server = NULL;
    MediaModule *media = NULL;

    // 1.get RPC instance
    server = ERPC::getInstance();
    if(NULL == server)
    {
        printf("malloc RPC core failed!\n");
        return -1;
    }
    // 2.get Module instance
    media = MediaModule::getInstance();
    if(NULL == media)
    {
        printf("malloc Media Module pointer failed!\n");
        return -1;
    }
    
    // 3.start RPC framework
    server->start();
    
    // 4.start module business
    media->startModule();
    
    // 5.go into monitor
    server->runUntilAskedToQuit(true);

    return 0;
}

