/* Module.cpp
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-22
 * Author: zhangqiyin/Konishi
 * Email : zhangqiyin@hangsheng.com.cn
 */
#include <stdio.h>
#include "module.h"
#include <pthread.h>

ERPC *Module::m_rpc = NULL;
pthread_mutex_t Module::mutex = PTHREAD_MUTEX_INITIALIZER;

#define MODULENAME "module_name"

Module::Module()
{
    m_name.clear();
    m_name = string(MODULENAME);
}

Module::Module(string name)
{
    m_name.clear();
    m_name = string(name);
}

Module::~Module()
{
    m_name.clear();
}

void Module::setRPC(ERPC *rpc)
{
    m_rpc = rpc;
}

//static unsigned int count = 0;

void Module::ModuleInterface(void *arg, void *data, size_t len)
{
#if 1
    //pthread_mutex_lock(&mutex);
    m_rpc->setResponse(arg, data, len);
    //pthread_mutex_unlock(&mutex);
#else
    char *response = (char *)"ModuleInterface response.";
    m_rpc->setResponse(arg, response, strlen(response));
#endif
}
