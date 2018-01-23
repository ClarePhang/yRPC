#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "app.h"
#include "rpc_core.h"

int main(int argc, char *argv[])
{
    APPView app;
    int result = -1;
    RPCCore *server = NULL;

    server = RPCCore::getInstance();
    if(NULL == server)
    {
        printf("malloc RPC core failed!\n");
        return -1;
    }
    
    server->setProcessName(argv[0]);
    server->setConfigProfile(string("../conf/network_building.conf"), "../conf/module_building.conf");
    
    server->start();
    result = app.startBussiness(server);

    if(result < 0)
        server->runUntilAskedToQuit(false);
    else
        server->runUntilAskedToQuit(true);

    return 0;
}

