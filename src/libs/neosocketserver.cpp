
#include "neomemmanager.h"
#include "neosocketserver.h"
#include "neobasicthread.h"

namespace NEOLIB{

namespace
{

#ifndef WIN32

class WorkerThread:public Thread
{
public:
    WorkerThread(NEO_THREAD_CALLBACK callback, ReadWriteParam param)
    {
        Thread(callback);
        mpParam = param;
    }

    ReadWriteParam mpParam;
};

//void __stdcall* doReadTask(void *pParam)
void* doReadTask(void *pParam)
{
    int result = 0;
    int total = 0;
    struct epoll_event ev={0};  

    ReadWriteParam pReadWriteParam = ((WorkerThread*)pParam)->mpParam;

    while((result = read(pReadWriteParam.fd,pReadWriteParam.buffer+total, 
        pReadWriteParam.bufsize-1))>0)
    {
        total += result;
    }

    if (result == -1 && errno != EAGAIN)
    {
        printf("doReadTask fail!\r\n");
    }

    if (pReadWriteParam.events & EPOLLOUT)
        return 0;

    ev.data.fd = pReadWriteParam.fd;
    ev.events = pReadWriteParam.events | EPOLLOUT;

    int rtn = epoll_ctl(pReadWriteParam.epollfd, EPOLL_CTL_MOD,pReadWriteParam.fd,&ev);
    if (rtn == -1)
    {
        printf("epoll_ctl in doReadTask fail!\r\n");
    }
}

//void __stdcall* doWriteTask(void *pParam)
void * doWriteTask(void *pParam)
{
    int result = 0;

    ReadWriteParam pReadWriteParam = ((WorkerThread*)pParam)->mpParam;

    int n = pReadWriteParam.bufsize;

    while (n > 0)
    {
        result = write(pReadWriteParam.fd, pReadWriteParam.buffer,n);
        if (result < n)
        {
            if (result == -1 && errno != EAGAIN)
            {
                printf("doWriteTask fail!\r\n");
            }
            break;
        }
        n -= result;
    }
    close(pReadWriteParam.fd);
}

#else

#endif
}

    NeoServer::NeoServer(const string addr, const unsigned short port, const SERVICE_TYPE svctyoe)
    {
        m_pDebug=new CNEOLowDebug( ".","socket_server",NULL,NULL,true);

        init();

        switch((int)svctyoe)
        {
        case 0:
            TCP(addr,port);
            break;
        case 1:
            UDP(addr,port);
            setInetAddr(addr,port);
            break;
        case 2:
            RAW(addr,port);
            setInetAddr(addr,port);
            break;
        case 3:
            LOCAL();
            break;
        }

        if(0 > listen(m_Socket,SOCKET_MAX_LISTENER))
        {
            m_pDebug->DebugToFile("Socket listen fail!\r\n");
            WIN_LINUX_CloseSocket(m_Socket);
        }
    }

    NeoServer::~NeoServer()
    {
        if(m_pDebug)
        {
           delete m_pDebug;
           m_pDebug=NULL;
        }
        m_pDebug->DebugToFile("~NeoServer\r\n");
        WIN_LINUX_CloseSocket(m_Socket);
    }

    void NeoServer::init()
    {
#ifdef WIN32
       int err;
       wVersionRequested=MAKEWORD(2,2);
       err=WSAStartup(wVersionRequested,&m_wsaData);
       if(err!=0)
       {
           m_pDebug->DebugToFile("Socket Init fail!\r\n");
       }
#endif
    }

    void NeoServer::TCP(string addr, unsigned short port)
    {
        m_Socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        setInetAddr(addr,port);
        if(0 > bind(m_Socket,(struct sockaddr *)(&m_ServerAddr), sizeof(struct sockaddr)))
        {
            m_pDebug->DebugToFile("Socket bind fail!\r\n");
            WIN_LINUX_CloseSocket(m_Socket);
        }
    }

    void NeoServer::UDP(string addr, unsigned short port)
    {
        m_Socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        setInetAddr(addr,port);
        if(0 > bind(m_Socket,(struct sockaddr *)(&m_ServerAddr), sizeof(struct sockaddr)))
        {
            m_pDebug->DebugToFile("Socket bind fail!\r\n");
            WIN_LINUX_CloseSocket(m_Socket);
        }
    }

    void NeoServer::RAW(string addr, unsigned short port)
    {
        m_Socket = socket(AF_INET,SOCK_RAW,IPPROTO_IP);
        setInetAddr(addr,port);
        if(0 > bind(m_Socket,(struct sockaddr *)(&m_ServerAddr), sizeof(struct sockaddr)))
        {
            m_pDebug->DebugToFile("Socket bind fail!\r\n");
            WIN_LINUX_CloseSocket(m_Socket);
        }
    }

    void NeoServer::LOCAL()
    {
        m_Socket = socket(AF_UNIX,SOCK_DGRAM,IPPROTO_TCP);
    }

     void NeoServer::setInetAddr(string addr, unsigned short port)
     {
        m_ServerAddr.sin_family=AF_INET;
        m_ServerAddr.sin_addr.s_addr= inet_addr(addr.c_str());
        m_ServerAddr.sin_port=htons(port);
     }

     void NeoServer::accept(int& size)
     {  
#ifndef WIN32
        while((m_connSocket = ::accept(m_Socket,(struct sockaddr *)&m_ClientAddr,(socklen_t*)&size)) > 0)
        {
            makeSocketNonBlocking(m_connSocket);

            m_Event.events = EPOLL_ET_IN;
            m_Event.data.fd = m_connSocket;

            addEvent(m_connSocket,m_Event);
        }

        if (-1 == m_connSocket)
        {
            if(errno != EAGAIN && errno != ECONNABORTED
                && errno != EPROTO && errno != EINTR)
                m_pDebug->DebugToFile("accept fail!\r\n");
        }
#endif
     }

#ifndef WIN32
     int NeoServer::makeSocketNonBlocking (int sfd)   
     {   
         int flags, s;   
    
          //得到文件状态标志   
         flags = fcntl (sfd, F_GETFL, 0);   
         if (flags == -1)   
         {   
           perror ("fcntl");   
           return -1;   
         }   
    
         //设置文件状态标志   
         flags |= O_NONBLOCK;   
         s = fcntl (sfd, F_SETFL, flags);   
         if (s == -1)   
         {   
             perror ("fcntl");   
             return -1;   
         }   
    
         return 0;   
     }   

    int NeoServer::addEvent(const int fd, epoll_event &listened_evnet)
    {
        int rtn = epoll_ctl(m_epollFd,EPOLL_CTL_ADD,fd,&listened_evnet);
        if (rtn==-1)
        {
            m_pDebug->DebugToFile("addEvent fail!\r\n");
        }
        return rtn;
    }

    int NeoServer::modEvent(const int fd, epoll_event &listened_evnet)
    {
        int rtn = epoll_ctl(m_epollFd, EPOLL_CTL_MOD,fd,&listened_evnet);
        if (rtn == -1)
        {
            m_pDebug->DebugToFile("modEvent fail!\r\n");
        }

        return rtn;
    }

#endif

     void NeoServer::loop()
     {
#ifndef WIN32
         int addrlen = 0;
         int tempfd = 0;
         int numofwaitingfds = 0;

         makeSocketNonBlocking(m_Socket);

         m_epollFd = epoll_create(EPOLL_SIZE_HINT);
         if (m_epollFd == -1)
         {
            m_pDebug->DebugToFile("m_epollFd create fail!\r\n");
            return;
         }

         m_Event.events = EPOLLIN;
         m_Event.data.fd = m_epollFd;
         if (addEvent(m_Socket,m_Event) == -1)
         {
            return;
         }

         for (;;)
         {
            numofwaitingfds = epoll_wait(m_epollFd, m_Events, EPOLL_SIZE_HINT, -1);
            if (-1 == numofwaitingfds)
            {
                m_pDebug->DebugToFile("epoll_ctl fail!\r\n");
            }

            int i = 0;
            for (;i < numofwaitingfds; ++i)
            {
                tempfd = m_Events[i].data.fd;
                if (tempfd == m_Socket)
                {
                    accept(addrlen);

                    continue;
                }
                else if (m_Events[i].events & EPOLLIN)
                {
                    recv(m_Events[i].events);
                }
                else if (m_Events[i].events & EPOLLOUT)
                {
                    
                }
            }

         }
#endif
     }

    void NeoServer::send(ReadWriteParam& param)
    {

        //也可以用task pool处    理
#ifndef WIN32   
        WorkerThread *readThread = new WorkerThread(doWriteTask,param);
        readThread->start();
#else
#endif
    }
    void NeoServer::recv(unsigned int events)
    {
        //也可以用task pool处    理
#ifndef WIN32   
        ReadWriteParam param;
        param.events = events;
        param.epollfd = m_epollFd;
        WorkerThread *readThread = new WorkerThread(doReadTask,param);
        readThread->start();
#else
#endif
    }

    
}