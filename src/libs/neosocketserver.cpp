#include "neosocketserver.h"
#include "neobaselib.h"
#include "neobasicthread.h"
#include "neodebug.h"
#include "neomemmanager.h"
#include "neolog.h"
#include "neothread.h"

namespace NEOLIB{

namespace
{

#ifndef WIN32

class WorkerThread:public Thread
{
public:
    WorkerThread(NEO_THREAD_CALLBACK callback, ReadWriteParam param):
	  mpParam(param),Thread(callback)
    {
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
        m_pNEOBaseLib = CNEOBaseLibrary::getInstance("neoserver",".","log",NULL);

        m_pNEOBaseLib->m_pMemPool->Register(m_pNEOBaseLib,"NeoServer::m_pNEOBaseLib");

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

        m_pNEOBaseLib->m_pMemPool->RegisterSocket(m_Socket,"NeoServer::m_Socket");

        if(0 > listen(m_Socket,SOCKET_MAX_LISTENER))
        {
            m_pNEOBaseLib->m_pDebug->DebugToFile("Socket listen fail!\r\n");
            ::WIN_LINUX_CloseSocket(m_Socket);
        }

#ifndef WIN32
		m_pNEOBaseLib->m_pTaskPool->RegisterATask(loop,this);
#else
        m_pNEOBaseLib->m_pTaskPool->RegisterATask(accept,this);
        m_pNEOBaseLib->m_pTaskPool->RegisterATask(loop,this);
#endif
    }

    NeoServer::~NeoServer()
    {
         m_pNEOBaseLib->m_pDebug->DebugToFile("~NeoServer\r\n");
         m_pNEOBaseLib->m_pMemPool->UnRegister(m_pNEOBaseLib);
        if(m_pNEOBaseLib)
        {
           delete m_pNEOBaseLib;
           m_pNEOBaseLib=NULL;
        }
        close();
    }

    void NeoServer::init()
    {
#ifdef WIN32
       int err;
       wVersionRequested=MAKEWORD(2,2);
       err=WSAStartup(wVersionRequested,&m_wsaData);
       if(err!=0)
       {
            m_pNEOBaseLib->m_pDebug->DebugToFile("Socket Init fail!\r\n");
       }

       m_hIOPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);  
#endif
    }

    void NeoServer::TCP(string addr, unsigned short port)
    {
        m_Socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        setInetAddr(addr,port);
        if(0 > bind(m_Socket,(struct sockaddr *)(&m_ServerAddr), sizeof(struct sockaddr)))
        {
            m_pNEOBaseLib->m_pDebug->DebugToFile("Socket bind fail!\r\n");
            ::WIN_LINUX_CloseSocket(m_Socket);
        }
    }

    void NeoServer::UDP(string addr, unsigned short port)
    {
        m_Socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        setInetAddr(addr,port);
        if(0 > bind(m_Socket,(struct sockaddr *)(&m_ServerAddr), sizeof(struct sockaddr)))
        {
            m_pNEOBaseLib->m_pDebug->DebugToFile("Socket bind fail!\r\n");
            ::WIN_LINUX_CloseSocket(m_Socket);
        }
    }

    void NeoServer::RAW(string addr, unsigned short port)
    {
        m_Socket = socket(AF_INET,SOCK_RAW,IPPROTO_IP);
        setInetAddr(addr,port);
        if(0 > bind(m_Socket,(struct sockaddr *)(&m_ServerAddr), sizeof(struct sockaddr)))
        {
            m_pNEOBaseLib->m_pDebug->DebugToFile("Socket bind fail!\r\n");
            ::WIN_LINUX_CloseSocket(m_Socket);
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

     bool NeoServer::accept(void *pThis,int& nStatus)
     {  
        int size = 0;
        NeoServer *tThis = (NeoServer*)pThis;

        tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("accept thread started!\r\n");
#ifndef WIN32
        while((tThis->m_connSocket = ::accept(tThis->m_Socket,(struct sockaddr *)&tThis->m_ClientAddr,(socklen_t*)&size)) > 0)
        {
            makeSocketNonBlocking(tThis->m_connSocket);

            tThis->m_Event.events = EPOLL_ET_IN;
            tThis->m_Event.data.fd = tThis->m_connSocket;

           addEvent(tThis->m_epollFd, tThis->m_connSocket,tThis->m_Event);
        }

        if (-1 == tThis->m_connSocket)
        {
            if(errno != EAGAIN && errno != ECONNABORTED
                && errno != EPROTO && errno != EINTR)
                 tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("accept fail!\r\n");
        }
#else
        
        while(tThis->m_connSocket= ::accept(tThis->m_Socket,(SOCKADDR*)&tThis->m_ClientAddr,&size) > 0) 
        {
            COMPLETIONKEY completionKey;  
            completionKey.s=tThis->m_connSocket;  
            completionKey.clientAddr=tThis->m_ClientAddr;   
            HANDLE h=CreateIoCompletionPort((HANDLE)tThis->m_connSocket,tThis->m_hIOPort,(DWORD)&completionKey,0); 
        }

        if (-1 == tThis->m_connSocket)
        {
            tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("accept fail and quit!\r\n");
        }
#endif
        return true;
     }

     void NeoServer::close()
     {
#ifdef WIN32
        CancelIo(m_hIOPort);  
#endif
        m_pNEOBaseLib->m_pMemPool->CloseSocket(m_Socket);
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

    int NeoServer::addEvent(const int epollfd, const int fd, epoll_event &listened_evnet)
    {
        int rtn = epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&listened_evnet);
        if (rtn==-1)
        {
             printf("addEvent fail!\r\n");
        }
        return rtn;
    }

    int NeoServer::modEvent(const int epollfd, const int fd, epoll_event &listened_evnet)
    {
        int rtn = epoll_ctl(epollfd, EPOLL_CTL_MOD,fd,&listened_evnet);
        if (rtn == -1)
        {
            printf("modEvent fail!\r\n");
        }

        return rtn;
    }

#endif

#ifndef WIN32
     bool NeoServer::loop(void *pThis,int &nStatus)
     {
         NeoServer *tThis = (NeoServer*)pThis;

         tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("NeoServer::loop thread started!\r\n");

         int addrlen = 0;
         int tempfd = 0;
         int numofwaitingfds = 0;

         makeSocketNonBlocking(tThis->m_Socket);

         tThis->m_epollFd = epoll_create(EPOLL_SIZE_HINT);
         if (tThis->m_epollFd == -1)
         {
             tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("m_epollFd create fail!\r\n");
            return false;
         }

         tThis->m_Event.events = EPOLLIN;
         tThis->m_Event.data.fd =  tThis->m_epollFd;
         if (addEvent(tThis->m_epollFd,tThis->m_Socket,tThis->m_Event) == -1)
         {
             tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("addEvent m_Socket fail!\r\n");
             return false;
         }

         for (;;)
         {
            numofwaitingfds = epoll_wait(tThis->m_epollFd, tThis->m_Events, EPOLL_SIZE_HINT, -1);
            if (-1 == numofwaitingfds)
            {
                 tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("epoll_ctl fail!\r\n");
            }

            int i = 0;
            for (;i < numofwaitingfds; ++i)
            {
                tempfd = tThis->m_Events[i].data.fd;
                if (tempfd == tThis->m_Socket)
                {
                    accept((void*)tThis,nStatus);

                    continue;
                }
                else if (tThis->m_Events[i].events & EPOLLIN)
                {
                    tThis->recv(tThis->m_Events[i].events);
                }
                else if (tThis->m_Events[i].events & EPOLLOUT)
                {
                    
                }
            }

         }
		 return true;
     }
#else

     void NeoServer::waitingToclose()
     {
        PostQueuedCompletionStatus(m_hIOPort,0,0,NULL);
        WIN_LINUX_CloseSocket(m_Socket);
     }

     bool NeoServer::loop(void *pThis,int &nStatus)
     {
         NeoServer *tThis = (NeoServer*)pThis;

         tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("NeoServer::loop thread started!\r\n");

         COMPLETIONKEY completionKey;
         DWORD dwNumberOfBytesTransferrd;
         LPOVERLAPPED pOverlapped;
         for(;;)
         {
             bool ret=GetQueuedCompletionStatus(tThis->m_hIOPort,&dwNumberOfBytesTransferrd,(LPDWORD)&completionKey,&pOverlapped,100);
             if(ret&&(&completionKey)&&pOverlapped)
             {
                 //成功
             }
             else
             {
                 int err=WSAGetLastError();
                 if(NULL!=pOverlapped)
                 {
                  //失败的IO操作。

                 }
                 else if(ret==WAIT_TIMEOUT)
                 {
                  //超时。

                 }
             }
         }
     }
#endif
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
        //也可以用task pool处理
#ifndef WIN32   
        ReadWriteParam param;
        param.events = events;
        param.epollfd = m_epollFd;
        WorkerThread *readThread = new WorkerThread(doReadTask,param);
        readThread->start();
#else
        DWORD recvBytes = 0, flags =0;
        IO_OPERATION_DATA *pIoData=new IO_OPERATION_DATA;  
        pIoData->dataBuf.buf = new char[1024];  
        pIoData->dataBuf.len=1014;  
        ZeroMemory(&pIoData->overlapped,sizeof(WSAOVERLAPPED));  
        if(WSARecv(m_connSocket,&(pIoData->dataBuf),1,&recvBytes,&flags,&(pIoData->overlapped),NULL)==SOCKET_ERROR)  
        {   
            if(WSAGetLastError()!=ERROR_IO_PENDING)  
            {  
                return;
            }  
        }  
#endif
    }

    
}