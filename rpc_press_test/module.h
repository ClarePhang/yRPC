/* MediaModule.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-22
 * Author: zhangqiyin/Konishi
 * Email : zhangqiyin@hangsheng.com.cn
 */
#ifndef MEDIAMODULE_H__
#define MEDIAMODULE_H__
#include <string>

#include "rpc.h"

using namespace std;

class Module
{
public:
    Module();
    Module(string name);
    ~Module();

public:
    void setRPC(ERPC *rpc);
    static void ModuleInterface(void *arg, void *data, size_t len);

private:
    string m_name;
    static ERPC *m_rpc;
    static pthread_mutex_t mutex;
};

#endif // MEDIAMODULE_H__

