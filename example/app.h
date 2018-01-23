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

#include "rpc_core.h"

using namespace std;

#define APPNAME  "APPView"

class APPView
{
public:
    APPView();
    ~APPView();

public:
    int startBussiness(RPCCore *rpc);

private:
    static void *appBusiness(void *arg);
    
private:
    string m_name;
    pthread_t thread_id;
    static RPCCore *m_rpc;
};

#endif // APP_H__

