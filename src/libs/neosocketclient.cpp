
#include "neosocketclient.h"
#include "neobaselib.h"
#include "neobasicthread.h"
#include "neodebug.h"
#include "neomemmanager.h"
#include "neolog.h"
#include "neothread.h"
#include "neoqueue.h"

namespace NEOLIB{

class CClient;

NeoClient::NeoClient(const string addr, const unsigned short port,const SERVICE_TYPE svctype):
    clientSwitch(true)
{
    m_pNEOBaseLib = CNEOBaseLibrary::getInstance("neoclient",".","clientlog",NULL);

    m_pNEOBaseLib->m_pMemPool->Register(m_pNEOBaseLib,"NeoClient::m_pNEOBaseLib");

    setInetAddr(addr,port);

    if(init(svctype))
    {
        doConnection();
        doRecv();
        doSend();
    }
}

NeoClient::~NeoClient()
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

bool NeoClient::init(const SERVICE_TYPE svctype)
{
#ifdef WIN32
   {
       m_bSocketInitFlag=false;
       int err;
       wVersionRequested=MAKEWORD(2,2);
       err=WSAStartup(wVersionRequested,&m_wsaData);
       if(err!=0)
       {
          m_pNEOBaseLib->m_pDebug->DebugToFile("Socket Init fail!\n");
       }
       else
       {
          m_bSocketInitFlag=true;
       }
    }
#endif

    if(svctype == SERVICE_TYPE::TCP)
        m_Socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    else if(svctype == SERVICE_TYPE::UDP)
        m_Socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    else if(svctype == SERVICE_TYPE::RAW)
        m_Socket = socket(AF_INET,SOCK_RAW,IPPROTO_IP);
    else if(svctype == SERVICE_TYPE::LOCAL)
        m_Socket = socket(AF_UNIX,SOCK_DGRAM,IPPROTO_TCP);

    m_pNEOBaseLib->m_pMemPool->RegisterSocket(m_Socket);
    return true;
}

void NeoClient::setInetAddr(string addr, unsigned short port)
{
    m_ServerAddr.sin_family=AF_INET;
    m_ServerAddr.sin_addr.s_addr= inet_addr(addr.c_str());
    m_ServerAddr.sin_port=htons(port);
}

void NeoClient::close()
{
    clientSwitch = false;
    m_pNEOBaseLib->m_pMemPool->CloseSocket(m_Socket);
#ifdef WIN32
    {
       if(m_bSocketInitFlag)
       {
          if(LOBYTE(&m_wsaData,wVersionRequested)!=2 || HIBYTE(&m_wsaData,wVersionRequested)!=2)
          {
             WSACleanup();
          }
          m_bSocketInitFlag=false;
       }
    }
#endif
}

bool NeoClient::doConnection()
{
    m_pNEOBaseLib->m_pTaskPool->RegisterATask(connTask,this);
    return true;
}

bool NeoClient::doSend()
{
    m_pNEOBaseLib->m_pTaskPool->RegisterATask(myTask,this);
    return true;
}
bool NeoClient::doRecv()
{
    m_pNEOBaseLib->m_pTaskPool->RegisterATask(recvTask,this);
    return true;
}

bool NeoClient::sendToAppQueue(const char *szData,int nDataLen)
{
    if( m_pNEOBaseLib->m_pMemQueue->ICanWork())
        m_pNEOBaseLib->m_pMemQueue->AddLast(szData, nDataLen);
    else
        return false;
    return true;
}

bool NeoClient::connTask(void *pThis,int &nStatus)
{
    bool needContinue = false;
    NeoClient *tThis = (NeoClient*)pThis;

    if(connect(tThis->m_Socket, (struct sockaddr*)&tThis->m_ServerAddr, sizeof(tThis->m_ServerAddr)) < 0)
    {
        tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("connected to server fail!\r\n");
        ::WIN_LINUX_CloseSocket(tThis->m_Socket);
        NEOMinSleep();
        needContinue = true;
    }

    return needContinue;
}

bool NeoClient::recvTask(void *pThis,int &nStatus)
{
    bool needContinue = false;
    int recvRet = 0;
    NeoClient *tThis = (NeoClient*)pThis;

    tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("receive task started!\r\n");
    while(tThis->clientSwitch)
    {
        char buffer[NEO_CLIENT_RECEIVE_BUFFER_SIZE] = {0};
#ifdef WIN32
        while((recvRet = recv(tThis->m_Socket, buffer, 1024, 0)) > 0)
        {
            tThis->m_pNEOBaseLib->m_pDebug->DebugToFile(
                            "NeoClient::loop got msg:msg=%s,recvsize=%d,buffersize=1024!,\r\n",
                            buffer,recvRet);
            tThis->sendToAppQueue(buffer,recvRet);
            tThis->m_pNEOBaseLib->m_pMemQueue->PrintInside();
        }//while

        //server socket closed
        if (0 == recvRet)
        {
            tThis->close();
        }
        else
        {
            int err=WSAGetLastError();
            tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("recv err=%d",err);
			continue;
        }

#else

#endif
    }//while(tThis->clientSwitch)

    return needContinue;
}

bool NeoClient::myTask(void *pThis,int &nStatus)
{
    bool needContinue = false;
    int sendRet = 0;
    NeoClient *tThis = (NeoClient*)pThis;

    tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("my task started!\r\n");

#ifdef WIN32
    if((sendRet = send(tThis->m_Socket, "i am a client", 14, 0)) < 0)
    {
            int err=WSAGetLastError();
            tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("send err=%d",err);
            return true;
    }

#else

#endif

    return needContinue;
}

};