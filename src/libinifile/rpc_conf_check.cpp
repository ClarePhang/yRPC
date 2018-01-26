/* rpc_conf_check.cpp
 * DO NOT EDIT THIS FILE.
 * Date  : 2018-01-16
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#include "rpc_conf.h"

int main(int argc, char *argv[])
{
    int ret = -1;
    RPCConfig * rpc_conf = RPCConfig::create();
    
    if(argc == 2)
        ret = rpc_conf->setConfigProfile(argv[1]);
    else
        ret = rpc_conf->setConfigProfile("");
    if(ret < 0)
        return -1;

    rpc_conf->selfCheckValidity();

    return 0;
}


