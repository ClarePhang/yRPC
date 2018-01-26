/* MediaModuleImplement.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-22
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#ifndef MEDIAMODULE_IMPLEMENT_H__
#define MEDIAMODULE_IMPLEMENT_H__
#include <string>
#include <pthread.h>

#include "rpc.h"
#include "MediaModule.h"

using namespace std;

Implement MediaModule
{
public:
    MediaModule();
    ~MediaModule();

public:
    void setRPC(ERPC *rpc);
    int startMedisBusiness(void);

public:
    static void mediaControl(void *arg);

private:
    static void *MediaBusinessThread(void *arg);

private:
    string m_name;
    static ERPC *m_rpc;
    pthread_t m_thread_id;
    static unsigned int m_count;
};

#endif // MEDIAMODULE_IMPLEMENT_H__

