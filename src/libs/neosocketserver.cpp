﻿#include "neosocketserver.h"
#include "neobaselib.h"
#include "neobasicthread.h"
#include "neodebug.h"
#include "neomemmanager.h"
#include "neolog.h"
#include "neothread.h"
#include "neosocketcommon.h"
#include "neoqueue.h"

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

    while((result = read(pReadWriteParam.m_event.data.fd,pReadWriteParam.buffer+total, 
        pReadWriteParam.bufsize))>0)
    {
        total += result;
    }

	printf("recv msg = %s!\r\n",pReadWriteParam.buffer);

    if (result == -1 && errno != EAGAIN)
    {
        printf("doReadTask fail!\r\n");
    }

    if (pReadWriteParam.m_event.events & EPOLLOUT)
	{
		delete(pReadWriteParam.buffer);
        return 0;
	}

    ev.data.fd = pReadWriteParam.m_event.data.fd;
    ev.events = pReadWriteParam.m_event.events | EPOLLOUT;

    int rtn = epoll_ctl(pReadWriteParam.epollfd, EPOLL_CTL_MOD,pReadWriteParam.m_event.data.fd,&ev);
    if (rtn == -1)
    {
        printf("epoll_ctl in doReadTask fail!\r\n");
    }

	delete(pReadWriteParam.buffer);
}

//void __stdcall* doWriteTask(void *pParam)
void * doWriteTask(void *pParam)
{
    int result = 0;
	struct epoll_event ev={0};  

    ReadWriteParam pReadWriteParam = ((WorkerThread*)pParam)->mpParam;

    int n = pReadWriteParam.bufsize;

    while (n > 0)
    {
        result = write(pReadWriteParam.m_event.data.fd, pReadWriteParam.buffer,n);
        if (result < n)
        {
            if (result == -1 && errno != EAGAIN)
            {
                printf("doWriteTask fail!\r\n");
				break;
            }
        }   
		n -= result;
    }

	ev.events = pReadWriteParam.m_event.events & (~EPOLLOUT);

	int rtn = epoll_ctl(pReadWriteParam.epollfd, EPOLL_CTL_MOD,pReadWriteParam.m_event.data.fd,&ev);
    if (rtn == -1)
    {
        printf("epoll_ctl in doWriteTask fail!\r\n");
    }
    //close(pReadWriteParam.m_event.data.fd);
}

#else

#endif
}

    NeoServer::NeoServer(const string addr, const unsigned short port, const SERVICE_TYPE svctyoe):
        serverSwitch(true)
    {
        m_pNEOBaseLib = CNEOBaseLibrary::getInstance("neoserver",".","serverlog",NULL);

        m_pNEOBaseLib->m_pDebug->DebugToFile("NeoServer\r\n");

        m_pNEOBaseLib->m_pMemPool->Register(m_pNEOBaseLib,"NeoServer::m_pNEOBaseLib");

        if (!init())
            return;
#ifdef WIN32
        SYSTEM_INFO tSysteminfo;
        GetSystemInfo(&tSysteminfo); 
        for(int i = 0;i< tSysteminfo.dwNumberOfProcessors * 2 ; ++ i) 
        {
            m_pNEOBaseLib->m_pTaskPool->RegisterATask(loop,this);
        }
#endif

        if(svctyoe == ::TCP)
        {
            TCP(addr,port);
        }
        else if(svctyoe == ::UDP)
        {
            UDP(addr,port);
        }
        else if(svctyoe == ::RAW)
            RAW(addr,port);
        else if(svctyoe == ::LOCAL)
            LOCAL();

        m_pNEOBaseLib->m_pMemPool->RegisterSocket(m_Socket,"NeoServer::m_Socket");

        if(0 > listen(m_Socket,SOCKET_MAX_LISTENER))
        {
            m_pNEOBaseLib->m_pDebug->DebugToFile("Socket listen fail!\r\n");
            ::WIN_LINUX_CloseSocket(m_Socket);
			return;
        }

#ifndef WIN32
        m_pNEOBaseLib->m_pTaskPool->RegisterATask(loop,this);
#else
        m_pNEOBaseLib->m_pTaskPool->RegisterATask(accept,this);
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

    bool NeoServer::init()
    {
#ifdef WIN32
       int err;
       m_bSocketInitFlag = false;
       wVersionRequested=MAKEWORD(2,2);
       err=WSAStartup(wVersionRequested,&m_wsaData);
       if(err!=0)
       {
            m_pNEOBaseLib->m_pDebug->DebugToFile("Socket Init fail!\r\n");
            return false;
       }
       else
       {
          m_bSocketInitFlag=true;
       }

       m_hIOPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);  
       if (INVALID_HANDLE_VALUE == m_hIOPort)
       {
            m_pNEOBaseLib->m_pDebug->DebugToFile("m_hIOPort Init fail!\r\n");
            return false;
       }
#endif
       return true;
    }

    void NeoServer::TCP(string addr, unsigned short port)
    {
        m_Socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if (m_Socket == WIN_LINUX_InvalidSocket)
        {
             m_pNEOBaseLib->m_pDebug->DebugToFile("socket create failed!\r\n");
             return;
        }

        setInetAddr(addr,port);
        if(0 != bind(m_Socket,(struct sockaddr *)(&m_ServerAddr), sizeof(struct sockaddr)))
        {
            m_pNEOBaseLib->m_pDebug->DebugToFile("Socket bind fail!\r\n");
            ::WIN_LINUX_CloseSocket(m_Socket);
            return;
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
        m_ServerAddr.sin_addr.s_addr= htonl(INADDR_ANY);
        //m_ServerAddr.sin_addr.s_addr= inet_addr(addr.c_str());
        m_ServerAddr.sin_port=htons(port);
     }

     bool NeoServer::accept(void *pThis,int& nStatus)
     {  
        NeoServer *tThis = (NeoServer*)pThis;

        bool needContinue = false;
        int size = sizeof(tThis->m_ClientAddr);
        
        tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("accept thread started!\r\n");
#ifndef WIN32
        while((tThis->m_connSocket = ::accept
			(tThis->m_Socket,(struct sockaddr *)&tThis->m_ClientAddr,(socklen_t*)&size)) > 0)
        {

            if(tThis->m_connSocket == WIN_LINUX_InvalidSocket)
                continue;
	    
            tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("got a client %s,%d!\r\n",
                inet_ntoa(tThis->m_ClientAddr.sin_addr),tThis->m_ClientAddr.sin_port);

            makeSocketNonBlocking(tThis->m_connSocket);

            tThis->m_Event.events = EPOLL_ET_IN;
            tThis->m_Event.data.fd = tThis->m_connSocket;
            
            CClient*pClient=new CClient(tThis->m_connSocket,tThis->m_ClientAddr);
            //pClient->setDataBuffer();

            if(-1 != addEvent(tThis->m_epollFd, tThis->m_connSocket,tThis->m_Event))
                tThis->g_clientManager.insert(pClient);
        }

        if (-1 == tThis->m_connSocket)
        {
            if(errno != EAGAIN && errno != ECONNABORTED
                && errno != EPROTO && errno != EINTR)
                 tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("accept fail!\r\n");
            needContinue = true;
        }
#else
        
        while(tThis->serverSwitch) 
        {
            tThis->m_connSocket=WSAAccept(tThis->m_Socket,(SOCKADDR*)&tThis->m_ClientAddr,&size,0,NULL);
            if(tThis->m_connSocket == WIN_LINUX_InvalidSocket)
                continue;

            CClient*pClient=new CClient(tThis->m_connSocket,tThis->m_ClientAddr);
            pClient->setDataBuffer();

            HANDLE h=CreateIoCompletionPort((HANDLE)tThis->m_connSocket,tThis->m_hIOPort,(ULONG_PTR)pClient,0);
            if (NULL == h)
            {
                tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("CreateIoCompletionPort failed!\r\n");
                delete(pClient);
                continue;
            }

            tThis->g_clientManager.insert(pClient);
            if(!pClient->Recv())
            {
                if(!tThis->g_clientManager.empty())
                    tThis->g_clientManager.erase(pClient);
                delete(pClient);
            }

            tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("got a client %s,%d!\r\n",
                inet_ntoa(tThis->m_ClientAddr.sin_addr),tThis->m_ClientAddr.sin_port);

            
        }

        if (-1 == tThis->m_connSocket)
        {
            tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("accept fail and quit!\r\n");
            needContinue = true;
        }
#endif
        return needContinue;
     }

     void NeoServer::close()
     {
        set<CClient*>::const_iterator iter;
        for(iter = g_clientManager.begin() ; iter != g_clientManager.end() ; ++iter)
        {
             g_clientManager.erase(*iter);
             delete(*iter);
        }

        serverSwitch = false;
        m_pNEOBaseLib->m_pMemPool->CloseSocket(m_Socket);
#ifdef WIN32
        CancelIo(m_hIOPort);  
        if(m_bSocketInitFlag)
        {
            if(LOBYTE(&m_wsaData,wVersionRequested)!=2 || HIBYTE(&m_wsaData,wVersionRequested)!=2)
            {
                WSACleanup();
            }
            m_bSocketInitFlag=false;
        }
#endif
     }

bool NeoServer::doSend(CClient* pClient)
{
    m_pNEOBaseLib->m_pTaskPool->RegisterATask(sendTask,pClient);
    return true;
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
         bool needContinue = false;
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
             return true;
         }

         tThis->m_Event.events = EPOLLIN;
         tThis->m_Event.data.fd =  tThis->m_Socket;
         if (addEvent(tThis->m_epollFd,tThis->m_Socket,tThis->m_Event) == -1)
         {
             tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("addEvent m_Socket fail!\r\n");
             return true;
         }

         while(tThis->serverSwitch)
         {
            //tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("epoll_wait loop!\r\n");
            numofwaitingfds = epoll_wait(tThis->m_epollFd, tThis->m_Events, EPOLL_SIZE_HINT, -1);
            if (-1 == numofwaitingfds)
            {
                 tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("epoll_wait fail!\r\n");
                 continue;
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
                    tThis->recv(tThis->m_Events[i],::TCP);
                    tThis->send(tThis->m_Events[i].data.fd);
                    //tThis->setEpollEvents(tThis->m_Events[i]);
                                        
                }
                else if (tThis->m_Events[i].events & EPOLLOUT)
                {
                    /*ReadWriteParam param;
					memcpy(&param.m_event,&tThis->m_Events[i],sizeof(tThis->m_Events[i]));
					param.epollfd = tThis->m_epollFd;
					param.buffer = "I am the server";
					param.bufsize = 16;
					tThis->send(param);*/
                    //tThis->send(tThis->m_Events[i].data.fd);
                    //tThis->setEpollEvents(tThis->m_Events[i]);
                }
            }

         }
         return needContinue;
     }
#else

     void NeoServer::waitingToclose()
     {
        PostQueuedCompletionStatus(m_hIOPort,0,0,NULL);
        WIN_LINUX_CloseSocket(m_Socket);
     }

     bool NeoServer::loop(void *pThis,int &nStatus)
     {
         bool needContinue = false;
         NeoServer *tThis = (NeoServer*)pThis;

         tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("NeoServer::loop thread started!\r\n");

         CClient *pClient = NULL; 
         DWORD dwNumberOfBytesTransferrd = 0;
         LPOVERLAPPED pOverlapped = NULL;

         while(tThis->serverSwitch)
         {
             bool ret=GetQueuedCompletionStatus(tThis->m_hIOPort,&dwNumberOfBytesTransferrd,(ULONG_PTR *)&pClient,&pOverlapped,WSA_INFINITE);
             if(ret&&(&pClient)&&pOverlapped)
             {
                 if (dwNumberOfBytesTransferrd == 0xFFFFFFFF)
                 {
                    continue;
                 }

                 IO_OPERATION_DATA*pIO=(IO_OPERATION_DATA*)pOverlapped;
                 //成功
                 switch(pIO->IoType)
                 {
                 case IORead:
                    if (0 == dwNumberOfBytesTransferrd)
                    {
                        //client关闭socket
                        if(!tThis->g_clientManager.empty())
                        tThis->g_clientManager.erase(pClient);
                        delete(pClient);
                        break;
                    }
                    
                    //数据送入队列
                    tThis->m_pNEOBaseLib->m_pDebug->DebugToFile(
                    "NeoServer::loop got msg:msg=%s,dwNumberOfBytesTransferrd=%d,pIO->len=%d,pIO->dataBuf.len=%d!,\r\n",
                    pIO->dataBuf.buf,dwNumberOfBytesTransferrd,pIO->len,pIO->dataBuf.len);
                    tThis->sendToAppQueue(pIO->dataBuf.buf,dwNumberOfBytesTransferrd);
                    tThis->m_pNEOBaseLib->m_pMemQueue->PrintInside();

                    //发送回应数据
                    //pClient->setDataBuffer("i am the server",16,IOWrite);
                    //pClient->Send();
                    tThis->doSend(pClient);

                    //重新触发报文异步接收
                    pClient->setDataBuffer();
                    if(!pClient->Recv())
                    {
                        if(!tThis->g_clientManager.empty())
                            tThis->g_clientManager.erase(pClient);
                        delete(pClient);
                    }

                    break;
                 case IOWrite:
                     tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("NeoServer::send finish!\r\n");
                     break;
                 case IOLogOut:
                     break;
                 default:
                     break;
                 }
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

                 if(!tThis->g_clientManager.empty())
                            tThis->g_clientManager.erase(pClient);
                 if (pClient!=NULL)
                    delete(pClient);
             }
         }//for

         return needContinue;
     }
#endif

#ifndef WIN32   
namespace
{
    typedef struct _epoll_thread_param
    {
        SERVICE_TYPE svctype;
        WIN_LINUX_SOCKET s;
        NeoServer *pThis;
        void *pData;
    }EpollThreadParam;
}

    bool NeoServer::setEpollEvents(epoll_event listened_evnet)
    {
        struct epoll_event ev = {0};
        //if ((listened_evnet.events & EPOLLOUT) == 0)
            ev.events = listened_evnet.events | EPOLLOUT;
            ev.data.fd = listened_evnet.data.fd;
        //else 
            //listened_evnet.events &= (~EPOLLOUT);
        if (-1 == modEvent(m_epollFd,listened_evnet.data.fd,ev))
            return false;
        return true;
    }

    bool NeoServer::send(ReadWriteParam& param)
    {

        //也可以用task pool处    理

        WorkerThread *readThread = new WorkerThread(doWriteTask,param);
        readThread->start();
        return true;

    }
    bool NeoServer::send(const WIN_LINUX_SOCKET s,const SERVICE_TYPE svctype)
    {
        int sendBufferSize = 0;
        IO_OPERATION_DATA *pIOData = new IO_OPERATION_DATA;
        pIOData->IoType=IOWrite;
        if (sendBufferSize > NEO_CLIENT_RECEIVE_BUFFER_SIZE)
        {
            pIOData->len = 16;
            pIOData->buffersize = NEO_CLIENT_RECEIVE_BUFFER_SIZE;
        }
        else
        {
            pIOData->len = 16;
            pIOData->buffersize = 16;
        }
	pIOData->dataBuf = "I am the server";

        EpollThreadParam *param = new EpollThreadParam;
        param->svctype = svctype;
        param->s = s;
        param->pThis = this;
        param->pData = (void*)pIOData;

        return m_pNEOBaseLib->m_pTaskPool->RegisterATask(sendTask,param);
    }

    bool NeoServer::sendTask(void *pThis,int &nStatus)
    {
        bool needContinue = false;
        int result = 0;
        int totalsent = 0;

        SERVICE_TYPE svctype = ((EpollThreadParam*)pThis)->svctype;
        NeoServer *tThis = (NeoServer*)((EpollThreadParam*)pThis)->pThis;
        IO_OPERATION_DATA *pIOData = (IO_OPERATION_DATA*)((EpollThreadParam*)pThis)->pData;
        WIN_LINUX_SOCKET s = ((EpollThreadParam*)pThis)->s;

        if(svctype == ::TCP)
        {
            int n = pIOData->len;

            while (n > 0)
            {
                result = write(s,pIOData->dataBuf+totalsent,n);
                if (result < n)
                {
                    if (result == -1 && errno != EAGAIN)
                    {
                        printf("CClient::Send fail!\r\n");
                        needContinue = false;
		        break;
                    }
                }  
                totalsent += result;
	        n -= result;
            }
        }
        else if(svctype == ::UDP)
        {
            CClient *pClient = tThis->findClientBySocket(s);
            result = sendto(s, pIOData->dataBuf, pIOData->len, 0, (struct sockaddr *)&(pClient->m_addr), sizeof(sockaddr_in));  
            if (result < 0)  
            {  
		needContinue = false;
            }  
        }
        delete(pThis);
        //delete(pIOData.dataBuf);
        return needContinue;
    }

    bool NeoServer::recv(epoll_event evt,const SERVICE_TYPE svctype)
    {
        //线程直接理理
        /*ReadWriteParam param;
		param.epollfd = m_epollFd;
        memcpy(&param.m_event,&evt,sizeof(evt));
		param.buffer = new char[NEO_SERVER_RECEIVE_BUFFER_SIZE];
		param.bufsize = NEO_SERVER_RECEIVE_BUFFER_SIZE;
        WorkerThread *readThread = new WorkerThread(doReadTask,param);
        readThread->start();*/
        EpollThreadParam *param = new EpollThreadParam;
        param->svctype = svctype;
        param->s = evt.data.fd;
        param->pThis = this;

        return m_pNEOBaseLib->m_pTaskPool->RegisterATask(recvTask,param);
        
    }

    bool NeoServer::recvTask(void *pThis,int &nStatus)
    {
        bool needContinue = false;
        int addrsize = sizeof(sockaddr_in);
        int result = 0;
        
        SERVICE_TYPE svctype = ((EpollThreadParam*)pThis)->svctype;
        NeoServer *tThis = (NeoServer*)((EpollThreadParam*)pThis)->pThis;
        WIN_LINUX_SOCKET s = ((EpollThreadParam*)pThis)->s;

        IO_OPERATION_DATA IoRecv;
        IoRecv.IoType=IORead;
        IoRecv.len = 0;
        IoRecv.dataBuf = new char[NEO_SERVER_RECEIVE_BUFFER_SIZE];
        IoRecv.buffersize = NEO_SERVER_RECEIVE_BUFFER_SIZE;

        tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("NeoServer::recvTask started!\r\n");
        if(svctype == ::TCP)
        {
            while((result = read(s,IoRecv.dataBuf+IoRecv.len, 
                IoRecv.buffersize))>0)
            {
                IoRecv.len += result;
            }

        }
        else if(svctype == ::UDP)
        {
            CClient *pClient = tThis->findClientBySocket(s);
            result = recvfrom(s,IoRecv.dataBuf,IoRecv.len,0,(struct sockaddr *)&(pClient->m_addr),(socklen_t*)&addrsize);
        }
        
        printf("recv msg = %s,len=%d!\r\n",IoRecv.dataBuf,IoRecv.len);

        if ((result == -1 && errno != EAGAIN) || 0 == IoRecv.len)
        {
            printf("CClient::Recv fail! or client closed\r\n");
            CClient *pClient = tThis->findClientBySocket(s);
            if(!tThis->g_clientManager.empty() && NULL != pClient)
            {
                tThis->g_clientManager.erase(pClient);
                delete(pClient);
                pClient = NULL;
            }
            needContinue = false;
        }
        else
        {
            tThis->sendToAppQueue(IoRecv.dataBuf,IoRecv.len);
            tThis->m_pNEOBaseLib->m_pMemQueue->PrintInside();
        }

        delete(pThis);
        delete(IoRecv.dataBuf);
        
        return needContinue;
    }
#endif
   
bool NeoServer::sendToAppQueue(const char *szData,int nDataLen)
{
    if( m_pNEOBaseLib->m_pMemQueue->ICanWork())
        m_pNEOBaseLib->m_pMemQueue->AddLast(szData, nDataLen);
    else
        return false;
    return true;
}

#ifdef WIN32
bool NeoServer::sendTask(void *pThis,int &nStatus)
{
    bool needContinue = false;
    int sendRet = 0;
    CClient *tThis = (CClient*)pThis;


    tThis->setDataBuffer("i am the server",16,IOWrite);
    needContinue = tThis->Send();

    return needContinue;
}
#endif

CClient* NeoServer::findClientBySocket(const WIN_LINUX_SOCKET s)
{
    CClient* client = NULL;
    set<CClient*>::const_iterator iter;
    for(iter = g_clientManager.begin() ; iter != g_clientManager.end() ; ++iter)
    {
        if((*iter)->m_s == s)
        {
            client =  *iter;  
            break;
        }
    }
    
    return client;
}

}//NEOLIB
