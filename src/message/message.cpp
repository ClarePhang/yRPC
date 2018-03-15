/* message.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-23
 * Author: Konishi
 * Email : konishi5202@163.com
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "message.h"

Message::Message()
{
    m_handler = NULL;
    m_user_data = NULL;
    m_user_data_len = 0;
    m_body_head.clear();
    m_message_head.init();
}

Message::~Message()
{
    m_handler = NULL;
    m_user_data = NULL;
    m_user_data_len = 0;
    m_body_head.clear();
    m_message_head.init();
}

void Message::initLinkRequestMessage(struct timeval *tv, unsigned int msgID, unsigned int statusCode)
{
    m_message_head.initLinkRequestMsg(tv, msgID, statusCode);
}

void Message::initLinkResponseMessage(struct timeval *tv, unsigned int msgID, unsigned int statusCode)
{
    m_message_head.initLinkResponseMsg(tv, msgID, statusCode);
}

void Message::initBeatRequestMessage(struct timeval *tv, unsigned int msgID, unsigned int statusCode)
{
    m_message_head.initBeatRequestMsg(tv, msgID, statusCode);
}

void Message::initBeatResponseMessage(struct timeval *tv, unsigned int msgID, unsigned int statusCode)
{
    m_message_head.initBeatResponseMsg(tv, msgID, statusCode);
}

void Message::initApplyRequestMessage(struct timeval *tv, unsigned int msgID, unsigned int statusCode)
{
    m_message_head.initApplyRequestMsg(tv, msgID, statusCode);
}

void Message::initApplyResponseMessage(struct timeval *tv, unsigned int msgID, unsigned int statusCode)
{
    m_message_head.initApplyResponseMsg(tv, msgID, statusCode);
}

void Message::initObserverRequestMessage(struct timeval *tv, unsigned int msgID, unsigned int statusCode)
{
    m_message_head.initObserverRequestMsg(tv, msgID, statusCode);
}

void Message::initObserverResponseMessage(struct timeval *tv, unsigned int msgID, unsigned int statusCode)
{
    m_message_head.initObserverResponseMsg(tv, msgID, statusCode);
}

void Message::initObserverInvokeMessage(struct timeval *tv, unsigned int msgID, unsigned int statusCode)
{
    m_message_head.initObserverInvokeMsg(tv, msgID, statusCode);
}

void Message::getMessageHeadFromData(const void *data)
{
    m_message_head.DeserializeMessageHead(data);
}

void Message::getMessageHeadFromData(const void *data, MessageHeadStr *msg)
{
    MessageHead::DeserializeMessageHead(data, msg);
}

bool Message::checkOnewayStatus(void)
{
    return m_message_head.getOnewayStatus();
}

void Message::changeOnewayStatus(bool oneway)
{
    m_message_head.setOnewayStatus(oneway);
}

bool Message::checkResponseStatus(void)
{
    return m_message_head.getResponseStatus();
}

void Message::changeResponseStatus(bool response)
{
    m_message_head.setResponseStatus(response);
}

unsigned int Message::getMessageID(void)
{
    return m_message_head.getMessageID();
}

MESSAGE_TYPE Message::getMessageType(void)
{
    return m_message_head.getMessageType();
}

unsigned int Message::getStatusCode(void)
{
    return m_message_head.getStatusCode();
}

struct timeval Message::getTimeoutTV(void)
{
    return m_message_head.getTimeoutTV();
}

void Message::setSender(const string &sender)
{
    m_body_head.setSender(sender);
}

string Message::getSender(void)
{
    return m_body_head.getSender();
}

void Message::setRecver(const string &recver)
{
    m_body_head.setRecver(recver);
}

string Message::getRecver(void)
{
    return m_body_head.getRecver();
}

void Message::setModule(const string &module)
{
    m_body_head.setModule(module);
}

string Message::getModule(void)
{
    return m_body_head.getModule();
}

void Message::setFunction(const string &func)
{
    m_body_head.setFunction(func);
}

string Message::getFunction(void)
{
    return m_body_head.getFunction();
}

void Message::setUserData(void *data, size_t len)
{
    m_user_data = data;
    m_user_data_len = len;
}

void Message::getUserData(void **data, size_t *len)
{
    if(data)
        *data = m_user_data;
    if(len)
        *len = m_user_data_len;
}

void Message::getBodyHead(void *data)
{
    m_body_head.DeserializeBodyHead(data);
}

void Message::setBodyHead(const string &sender, const string recver, const string &module, const string function)
{
    m_body_head.setSender(sender);
    m_body_head.setRecver(recver);
    m_body_head.setModule(module);
    m_body_head.setFunction(function);
}

void Message::setHandler(void *handler)
{
    m_handler = handler;
}

void *Message::getHandler(void)
{
    return m_handler;
}

int Message::mallocBodyData(size_t user_len)
{
    if(0 == user_len)
        return 0;
    
    m_user_data = (void *)malloc(user_len);
    if(NULL == m_user_data)
        return -1;
    
    m_user_data_len = user_len;
    return 0;
}

int Message::mallocBodyData(void *user_data, size_t user_len)
{
    if(0 == user_len)
    {
        m_user_data = NULL;
        m_user_data_len = 0;
    }
    else
    {
        m_user_data = (void *)malloc(user_len);
        if(NULL == m_user_data)
            return -1;
        
        m_user_data_len = user_len;
        memcpy(m_user_data, user_data, user_len);
    }

    return 0;
}

void Message::releaseBodyData(void)
{
    if(m_user_data)
        free(m_user_data);
    m_user_data_len = 0;
}

void Message::viewBodyHead(void)
{
    m_body_head.view();
}

void Message::viewMessageHead(void)
{
    m_message_head.view();
}

void Message::updateBodySize(void)
{
    m_message_head.setBodySize(m_body_head.size() + m_user_data_len);
}

size_t Message::getBodySize(void)
{
    return m_message_head.getBodySize();
}

size_t Message::getBodyHeadSize(void)
{
    return m_body_head.size();
}

size_t Message::getMessageSize(void)
{
    return (MESSAGE_HEAD_SIZE + m_body_head.size() + m_user_data_len);
}

int Message::serializeMessage(void **data, size_t *len)
{
    unsigned char *ptr = NULL;
    size_t data_len = getMessageSize();

    *data = (void *)malloc(data_len);
    if(NULL == *data)
        return -1;
    
    ptr = (unsigned char *)*data;
    m_message_head.SerializeMessageHead((void *)ptr);
    ptr += MESSAGE_HEAD_SIZE;
    m_body_head.SerializeBodyHead((void *)ptr);
    ptr += m_body_head.size();
    if(m_user_data_len)
        memcpy(ptr, m_user_data, m_user_data_len);
    if(len)
        *len = data_len;
    
    return 0;
}

void Message::releaseSerializeMessage(void *data)
{
    if(data)
        free(data);
}

