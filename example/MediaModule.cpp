/* MediaModule.h
 * Date  : 2017-11-22
 * Author: Konishi
 * Email : konishi5202@126.com
 */
#include <stdio.h>
#include "MediaModuleBase.h"

MediaModule *MediaModule::m_media = NULL;

MediaModule::MediaModule()
{
    m_media = NULL;
}

MediaModule::~MediaModule()
{
    if(m_media)
        delete m_media;
    m_media = NULL;
}

MediaModule *MediaModule::getInstance(void)
{
    if(m_media)
        return m_media;
    m_media = new MediaModuleBase();
    return m_media;
}


