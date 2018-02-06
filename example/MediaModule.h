/* MediaModule.h
 * Date  : 2017-11-22
 * Author: Konishi
 * Email : konishi5202@126.com
 */
#ifndef MEDIAMODULE_H__
#define MEDIAMODULE_H__

#include "rpc.h"

using namespace std;

typedef void (*MediaStateChanged)(void *data, size_t len);

enum MediaControlEnum{
    stopMedia = 0,
    playMedia = 1,
    pauseMedia = 2,
    prevMedia = 3,
    nextMedia = 4,
    speedMedia = 5,
    backMedia = 6,
    seekMedia = 7
};

typedef struct {
    MediaControlEnum control_way;
    unsigned int control_para; // speed/back second, or seek to where
}MediaControlStr;

class MediaModule
{    
public:
    MediaModule();
    virtual ~MediaModule();

public:
    static MediaModule *getInstance(void);
    virtual int startModule(void) = 0;
    virtual int registerStateHandler(MediaStateChanged handler) = 0;
    virtual int mediaControl(MediaControlEnum ctl, unsigned int time) = 0;

private:
    static MediaModule *m_media;
};


#endif // MEDIAMODULE_INTERFACE_H__

