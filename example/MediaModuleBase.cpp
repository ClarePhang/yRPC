/* MediaModuleBase.cpp
 * Date  : 2017-11-22
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "MediaModuleBase.h"

ERPC *MediaModuleBase::m_rpc = NULL;
unsigned int MediaModuleBase::m_count = 0;
MediaStateChanged MediaModuleBase::m_observer_handler = NULL;

MediaModuleBase::MediaModuleBase()
{
    m_rpc = NULL;
    m_name.clear();
    m_name = string(MEDIAMODULENAME);
}

MediaModuleBase::~MediaModuleBase()
{
    m_rpc = NULL;
    m_name.clear();
}

/*-------------------------- Interface ----------------------------*/
int MediaModuleBase::startModule(void)
{
    if(NULL == getHSAEERPCInstance())
        return -1;

    m_rpc->registerService("mediaControl", mediaControl);
    m_rpc->createObserved("mediaState");
    
    if(pthread_create(&m_thread_id, NULL, MediaBusinessThread, NULL) != 0)
        return -1;
    
    return 0;
}

int MediaModuleBase::registerStateHandler(MediaStateChanged handler)
{
    int result = -1;
    
    if(NULL == getHSAEERPCInstance())
        return -1;
    
    result = m_rpc->registerObserver(MEDIAMODULENAME, "mediaState", mediaStateChanged);
    if(result)
        return result;

    m_observer_handler = handler;
    return 0;
}

int MediaModuleBase::mediaControl(MediaControlEnum ctl, unsigned int time)
{
    int result = -1;
    MediaControlStr mc;
    size_t recv_len = 6;
    unsigned char send[6];
    unsigned char recv[6];

    if(NULL == getHSAEERPCInstance())
        return -1;
    
    mc.control_way = ctl;
    mc.control_para = time;
    SerialMediaControlStr((void *)send, &mc);
    result = m_rpc->proxyCall(MEDIAMODULENAME, string("mediaControl"), (void *)send, 5, (void *)recv, &recv_len);
    
    return result;
}

/*-------------------------- Implement ----------------------------*/
void MediaModuleBase::mediaControl(void *msg, void *data, size_t len)
{
    MediaControlStr mc;
    unsigned char response = 0;

    DeserialMediaControlStr(data, &mc);

    switch(mc.control_way)
    {
        case stopMedia:
            printf("stop media.\n");
            break;

        case playMedia:
            printf("play media.\n");
            break;
            
        case pauseMedia:
            printf("pause media.\n");
            break;
            
        case prevMedia:
            //printf("switch media to previous.\n");
            break;
            
        case nextMedia:
            //printf("switch media to next.\n");
            break;
            
        case speedMedia:
            printf("speed media.\n");
            break;
            
        case backMedia:
            printf("back media.\n");
            break;
            
        case seekMedia:
            printf("Seek media to %u\n", mc.control_para);
            break;
            
        default:
            break;
    }

    m_rpc->setResponse(msg, (void *)&response, 1);
}

void MediaModuleBase::mediaStateChanged(void *msg, void *data, size_t len)
{
    if(m_observer_handler)
        m_observer_handler(data, len);
}

/*-------------------------- business thread ----------------------------*/
void *MediaModuleBase::MediaBusinessThread(void *arg)
{
    while(1)
    {
        sleep(5);
        m_rpc->invokeObserver("mediaState", NULL, 0);
    }

    pthread_exit(NULL);
}

/*-------------------------- serialization thread ----------------------------*/
ERPC *MediaModuleBase::getHSAEERPCInstance(void)
{
    if(NULL == m_rpc)
        m_rpc = ERPC::getInstance();
    return m_rpc;
}

void MediaModuleBase::SerialMediaControlStr(void *data, MediaControlStr *mc)
{
    unsigned char *ptr = (unsigned char *)data;
    ptr[0] = (unsigned char)(mc->control_way);
    ptr[1] = (unsigned char)(mc->control_para>>24 & 0xFF);
    ptr[2] = (unsigned char)(mc->control_para>>16 & 0xFF);
    ptr[3] = (unsigned char)(mc->control_para>>8  & 0xFF);
    ptr[4] = (unsigned char)(mc->control_para     & 0xFF);
}

void MediaModuleBase::DeserialMediaControlStr(void *data, MediaControlStr *mc)
{
    unsigned char *ptr = (unsigned char *)data;
    mc->control_way = MediaControlEnum(ptr[0]);
    mc->control_para = (ptr[1]<<24 | ptr[2]<<16 | ptr[3]<<8 | ptr[4]);
}

