/* socketbase.h
 * DO NOT EDIT THIS FILE.
 * Date  : 2017-11-09
 * Author: zhangqiyin/Konishi
 * Email : zhangqiyin@hangsheng.com.cn
 */

#ifndef SOCKET_BASE_H__
#define SOCKET_BASE_H__

class SocketBaseOpt
{
public:
    SocketBaseOpt(){};
    ~SocketBaseOpt(){};
    
public:
    int initSockaddr(struct sockaddr_un &s_addr, const char *path);
    int initSockaddr(struct sockaddr_in &s_addr, const char *ip, unsigned int port);
};

#endif  // SOCKET_BASE_H__

