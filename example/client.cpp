#include <string>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "rpc.h"
#include "app.h"

int main(int argc, char *argv[])
{
    APPView app;
    HSAERPC server;
    int result = -1;

    server.setProcessName(argv[0]);
    server.setConfigPath(string("../conf/network_building.conf"), "../conf/module_building.conf");
    
    server.start();
    result = app.startBussiness(&server);

    if(result < 0)
        server.runUntilAskedToQuit(false);
    else
        server.runUntilAskedToQuit(true);

    return 0;
}

