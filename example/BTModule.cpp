/* BTModule.cpp
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-22
 * Author: zhangqiyin/Konishi
 * Email : zhangqiyin@hangsheng.com.cn
 */
#include <stdio.h>
#include <string.h>
#include "BTModule.h"

RPCCore *BTModule::m_rpc = NULL;

BTModule::BTModule()
{
    m_name.clear();
    m_name = string(BTMODULENAME);
}

BTModule::~BTModule()
{
    m_name.clear();
}

void BTModule::setRPC(RPCCore *rpc)
{
    m_rpc = rpc;
}

void BTModule::btPlay(void *arg)
{
    char *response = (char *)"BT Music is playing.";
    
    printf("BT music start play.\n");
    
    m_rpc->setResponse(arg, response, strlen(response));
}

void BTModule::btStop(void *arg)
{
    char *response = (char *)"BT Music is stoping.";
    
    printf("BT music stop play.\n");
    
    m_rpc->setResponse(arg, response, strlen(response));
}

void BTModule::btPrev(void *arg)
{
    char *response = (char *)"Switch previous OK.";
    
    printf("Switch previous BT music.\n");
    
    m_rpc->setResponse(arg, response, strlen(response));
}

void BTModule::btNext(void *arg)
{
    char *response = (char *)"Switch next OK.";
    
    printf("Switch next BT music.\n");

    m_rpc->setResponse(arg, response, strlen(response));
}

