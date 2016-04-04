#ifndef NEOSOCKETSERVER

#define NEOSOCKETSERVER
#include "neoindex.h" 
//#include "neolock.h"
#include <string>
#include <set>
/*
    以下变量线程见共享的风险
    WIN_LINUX_SOCKET m_Socket;
    WIN_LINUX_SOCKET m_connSocket;
    sockaddr_in m_ServerAddr;
    sockaddr_in m_ClientAddr;
*/
using namespace std;

namespace NEOLIB {

class CNEOBaseLibrary;
class CClient;


#ifndef WIN32

typedef struct
{
	int epollfd;
	epoll_event m_event;
    int bufsize;
    char *buffer;
}ReadWriteParam;

#else
typedef struct _completionKey   
{  
    SOCKET s;  
    SOCKADDR_IN clientAddr;  
}COMPLETIONKEY,*PCOMPLETIONKEY;  

#endif


class NeoServer{
public:

    friend class CClient;

    NeoServer(const string addr, const unsigned short port, const SERVICE_TYPE svctype=(SERVICE_TYPE)0);
    ~NeoServer();
    
    bool init();
    void TCP(string addr, unsigned short port);
    void UDP(string addr, unsigned short port);
    void RAW(string addr, unsigned short port);
    void LOCAL();

#ifndef WIN32
    static int makeSocketNonBlocking (int sfd);
    static int addEvent(const int epollfd, const int fd, epoll_event &listened_evnet);
    static int modEvent(const int epollfd,const int fd, epoll_event &listened_evnet);
#else
    void waitingToclose();
#endif

    static bool loop(void *pThis,int &nStatus);

#ifndef WIN32
    /*bool setEpollEvents(WIN_LINUX_SOCKET s,epoll_event &listened_evnet)
    {
        if ((listened_evnet.events & EPOLLOUT) == 0)
            listened_evnet.events |= EPOLLOUT;
        else 
            listened_evnet.events |= EPOLLIN;
        modEvent(m_epollFd,s,listened_evnet);
        return true;
    }*/
    bool send(ReadWriteParam& param);
    bool send(const WIN_LINUX_SOCKET s,const SERVICE_TYPE svctype = ::TCP);
    static bool sendTask(void *pThis,int &nStatus);
    bool recv(epoll_event evt, const SERVICE_TYPE svctype = ::TCP);
    static bool recvTask(void *pThis,int &nStatus);
#else
    static bool sendTask(void *pThis,int &nStatus);
#endif

    void close();

    bool doSend(CClient* pClient);

    CClient* findClientBySocket(const WIN_LINUX_SOCKET s);

private:
    void setInetAddr(string addr, unsigned short port);
    
    static bool accept(void *pThis,int &nStatus);
    bool sendToAppQueue(const char *szData,int nDataLen);
    
    //CMutexLock m_Lock;
    bool serverSwitch;
    set<CClient*> g_clientManager;
    CNEOBaseLibrary *m_pNEOBaseLib;
#ifdef WIN32
    bool m_bSocketInitFlag;
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
