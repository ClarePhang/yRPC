/* APP.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-22
 * Author: zhangqiyin/Konishi
 * Email : zhangqiyin@hangsheng.com.cn
 */
#ifndef APP_H__
#define APP_H__
#include <string>
#include <pthread.h>

#include "rpc.h"

using namespace std;

#define APPNAME  "Client"

class Client
{
public:
    Client();
    Client(string name);
    ~Client();

public:
    int startBussiness(ERPC *rpc);

private:
    static void *appBusiness(void *arg);

private:
    static string m_name;
    pthread_t thread_id;
    static ERPC *m_rpc;
};

#endif // APP_H__

