#ifndef NEOSOCKETSERVER

#define NEOSOCKETSERVER
#include "neoindex.h" 
#include <string>
/*
    以下变量线程见共享的风险
    WIN_LINUX_SOCKET m_Socket;
    WIN_LINUX_SOCKET m_connSocket;
    sockaddr_in m_ServerAddr;
    sockaddr_in m_ClientAddr;
*/
using namespace std;

#ifndef WIN32
extern int errno;
#endif

namespace NEOLIB {

class CNEOBaseLibrary;

typedef struct
{
    int fd;
    int bufsize;
    unsigned int events;
    int epollfd; 
    void *buffer;
}ReadWriteParam;

#ifndef WIN32
extern int errno;
#else
typedef struct _completionKey   
{  
    SOCKET s;  
    SOCKADDR_IN clientAddr;  
}COMPLETIONKEY,*PCOMPLETIONKEY;  

typedef struct _io_operation_data  
{  
    WSAOVERLAPPED overlapped;  
    WSABUF dataBuf;   
}IO_OPERATION_DATA;  

#endif
class NeoServer{
public:
    NeoServer(const string addr, const unsigned short port, const SERVICE_TYPE svctype=(SERVICE_TYPE)0);
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
#else
    void waitingToclose();
#endif
    static bool loop(void *pThis,int &nStatus);
    void send(ReadWriteParam& param);
    void recv(unsigned int events);

    void close();

private:
    void setInetAddr(string addr, unsigned short port);
    
    static bool accept(void *pThis,int &nStatus);

    CNEOBaseLibrary *m_pNEOBaseLib;
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA m_wsaData;
    HANDLE m_hIOPort;

#endif
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