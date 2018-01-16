#include <string>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "rpc.h"
#include "BTModule.h"
#include "MediaModule.h"

int main(int argc, char *argv[])
{
    HSAERPC server;
    BTModule btmodule;
    MediaModule media;

    // RPC init
    server.setProcessName(argv[0]);
    server.setConfigPath(string("../conf/network_building.conf"), "../conf/module_building.conf");

    media.setRPC(&server);
    server.addService("mediaPlay", media.mediaPlay);
    server.addService("mediaStop", media.mediaStop);
    server.addService("mediaPrev", media.mediaPrev);
    server.addService("mediaNext", media.mediaNext);

    btmodule.setRPC(&server);
    server.addService("btPlay", btmodule.btPlay);
    server.addService("btStop", btmodule.btStop);
    server.addService("btPrev", btmodule.btPrev);
    server.addService("btNext", btmodule.btNext);

    server.start();
    server.runUntilAskedToQuit(true);

    return 0;
}

