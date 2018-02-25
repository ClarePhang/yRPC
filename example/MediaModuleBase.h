/* MediaModuleBase.h
 * Date  : 2017-11-22
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#ifndef MEDIAMODULE_BASE_H__
#define MEDIAMODULE_BASE_H__
#include <string>
#include <pthread.h>

#include "rpc.h"
#include "MediaModule.h"

using namespace std;

#define MEDIAMODULENAME  "Media"

class MediaModuleBase : public MediaModule
{
public:
    MediaModuleBase();
    virtual ~MediaModuleBase();

public:  // interface
    virtual int startModule(void);
    virtual int registerStateHandler(MediaStateChanged handler);
    virtual int mediaControl(MediaControlEnum ctl, unsigned int time);

private:  // service
    static void mediaControl(void *msg, void *data, size_t len);
    static void mediaStateChanged(void *msg, void *data, size_t len);

private:  // business thread
    static void *MediaBusinessThread(void *arg);

private:  // serialization function and private function
    static inline ERPC *getHSAEERPCInstance(void);
    static inline void SerialMediaControlStr(void *data, MediaControlStr *mc);
    static inline void DeserialMediaControlStr(void *data, MediaControlStr *mc);

private:
    string m_name;
    static ERPC *m_rpc;
    pthread_t m_thread_id;
    static unsigned int m_count;
    static MediaStateChanged m_observer_handler;
};

#endif // MEDIAMODULE_IMPLEMENT_H__

