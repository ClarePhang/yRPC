#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "rpc.h"

#include "module.h"
#include "client.h"

int main(int argc, char *argv[])
{
    int result = -1;
    ERPC *server = NULL;
    string client_name = string(argv[0]) + string("client");
	string module_name = string(argv[0]) + string("module");
    Client client(client_name);
    Module module(module_name);

    // RPC init
    server = ERPC::getInstance();
    if(NULL == server)
    {
        printf("Get ERPC instance failed!\n");
        return -1;
    }

    module.setRPC(server);

	server->registerService("ModuleInterface", module.ModuleInterface);

    server->start();

    /****************  Client  ****************/
    result = client.startBussiness(server);
    /****************  Client  ****************/

    if(result < 0)
	{
        server->runUntilAskedToQuit(false);
	}
    else
	{
        server->runUntilAskedToQuit(true);
	}

    return 0;
}

