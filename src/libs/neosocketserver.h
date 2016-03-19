﻿#ifndef NEOSOCKETSERVER

#define NEOSOCKETSERVER
#include "neoindex.h" 
#include <string>

using namespace std;

#ifndef WIN32
extern int errno;
#endif

namespace NEOLIB {

typedef enum 
{
    TCP = 0,
    UDP,
    RAW,
    LOCAL

}SERVICE_TYPE;

typedef struct
{
    int fd;
    int bufsize;
    unsigned int events;
    int epollfd; 
    void *buffer;
}ReadWriteParam;

class CNEOLowDebug;
class NeoServer{
public:
	NeoServer(const string addr, const unsigned short port, const SERVICE_TYPE svctyoe=(SERVICE_TYPE)0);
    ~NeoServer();
    
    void init();
    void TCP(string addr, unsigned short port);
    void UDP(string addr, unsigned short port);
    void RAW(string addr, unsigned short port);
    void LOCAL();
#ifndef WIN32
    static int makeSocketNonBlocking (int sfd);
    int addEvent(const int fd, epoll_event &listened_evnet);
    int modEvent(const int fd, epoll_event &listened_evnet);
#endif
    void loop();
    void send(ReadWriteParam& param);
    void recv(unsigned int events);

private:
    void setInetAddr(string addr, unsigned short port);
    
    void accept(int& size);


#ifdef WIN32
    WORD wVersionRequested;
    WSADATA m_wsaData;
#endif
	CNEOLowDebug *m_pDebug;
    WIN_LINUX_SOCKET m_Socket;
    WIN_LINUX_SOCKET m_connSocket;
    sockaddr_in m_ServerAddr;
    sockaddr_in m_ClientAddr;
    

#ifndef WIN32
    struct epoll_event m_Event;   
    struct epoll_event m_Events[EPOLL_MAX_EVENTS]; 
    int m_epollFd;
#endif
};

}

#endif