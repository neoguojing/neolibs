
#include "neosocketclient.h"
#include "neobaselib.h"
#include "neobasicthread.h"
#include "neodebug.h"
#include "neomemmanager.h"
#include "neolog.h"
#include "neothread.h"
#include "neosocketcommon.h"

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
        m_pNEOBaseLib->m_pTaskPool->RegisterATask(recvTask,this);
        NEOMinSleep();
        m_pNEOBaseLib->m_pTaskPool->RegisterATask(myTask,this);
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
    int err;
    wVersionRequested=MAKEWORD(2,2);
    err=WSAStartup(wVersionRequested,&m_wsaData);
    if(err!=0)
    {
        m_pNEOBaseLib->m_pDebug->DebugToFile("Socket Init fail!\r\n");
        return false;
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
    ::WIN_LINUX_CloseSocket(m_Socket);
}

#ifndef WIN32
    bool NeoClient::send(const SERVICE_TYPE svctype, char *buf, int len)
    {
        if((int)svctype == 0)
        {
            if(0 > write(m_Socket, buf,len))
                return false;
        }
        else if((int)svctype == 1)
        {
           int rtn = sendto(m_Socket, buf, len, 0, (struct sockaddr *)&m_ServerAddr, sizeof(m_ServerAddr));  
           if (rtn < 0)  
           {  
               m_pNEOBaseLib->m_pDebug->DebugToFile("send udp fail!\r\n"); 
			   return false;
           }  
        }
		return true;
    }
    bool NeoClient::recv(const SERVICE_TYPE svctype,char *buf, int &len)
    {
        if((int)svctype == 0)
        {
            if(read(m_Socket,buf,len) < 0)
                return false;
        }
        else if((int)svctype == 1)
        {
            recvfrom(m_Socket,buf,len,0,(struct sockaddr *)&m_ServerAddr,(socklen_t*)&len);
        }
		return true;
    }
#else
    bool NeoClient::send(const SERVICE_TYPE svctype, char *buf, int len)
    {
        if((int)svctype == 0)
        {
            
	        WSABUF wsabuf;
            DWORD flag=0;

            ZeroMemory(&m_IoSend,sizeof(IO_OPERATION_DATA));
	        m_IoSend.IoType = IOWrite;
            m_IoRecv.len = len;

	        wsabuf.buf=buf;
	        wsabuf.len=512;
	        
	        int ret=WSASend(m_Socket,&wsabuf,1,NULL,flag,&(m_IoSend.overlapped),NULL);
	        if(ret==SOCKET_ERROR)
	        {
		        int err=WSAGetLastError();
		        if(err!=WSA_IO_PENDING)
		        {
                    m_pNEOBaseLib->m_pDebug->DebugToFile("NeoClient::send send msg fail!\r\n"); 
			        return false;
		        }
	        }
	        return true;
        }
        else if((int)svctype == 1)
        {
           int rtn = sendto(m_Socket, buf, len, 0, (struct sockaddr *)&m_ServerAddr, sizeof(m_ServerAddr));  
           if (rtn < 0)  
           {  
               m_pNEOBaseLib->m_pDebug->DebugToFile("send udp fail!\r\n");
           }  
        }
	    

    }

    bool NeoClient::recv(const SERVICE_TYPE svctype, char *buf, int& len)
    {

        if((int)svctype == 0)
        {
            WSABUF wsabuf;
	        ZeroMemory(&m_IoRecv,sizeof(IO_OPERATION_DATA));
	        m_IoRecv.IoType = IORead;
            m_IoRecv.len = len;
	        wsabuf.buf= new char[1024];
	        wsabuf.len=1024;
	        DWORD flag=0;
	        int ret=WSARecv(m_Socket,&wsabuf,1,NULL,&flag,&m_IoRecv.overlapped,NULL);
	        if(ret==SOCKET_ERROR)
	        {
		        int err=WSAGetLastError();
		        if(err!=WSA_IO_PENDING)
		        {
			        return false;
		        }
	        }
	        return true;
        }
        else if((int)svctype == 1)
        {
            recvfrom(m_Socket,buf,len,0,(SOCKADDR*)&m_ServerAddr,&len);
        }
    }
#endif

    bool NeoClient::recvTask(void *pThis,int &nStatus)
    {
        bool needContinue = false;
        NeoClient *tThis = (NeoClient*)pThis;
#ifdef WIN32
        if(WSAConnect(tThis->m_Socket, (struct sockaddr*)&tThis->m_ServerAddr, sizeof(tThis->m_ServerAddr),
            NULL,NULL,NULL,NULL) < 0)
        {
            tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("connected to server fail!\r\n");
            ::WIN_LINUX_CloseSocket(tThis->m_Socket);
            NEOMinSleep();
            return true;
        }
#else

#endif
        while(tThis->clientSwitch)
        {
            break;
        }

        return needContinue;
    }

    bool NeoClient::myTask(void *pThis,int &nStatus)
    {
        bool needContinue = false;
        NeoClient *tThis = (NeoClient*)pThis;


        tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("client task started!\r\n");
        CClient *pClient = new CClient(tThis->m_Socket,tThis->m_ServerAddr);
#ifdef WIN32
		pClient->setDataBuffer("i am a client",14,IOWrite);
        if(!pClient->Send())
        {
             tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("send fail!\r\n");
             delete pClient;
             return true;
        }
#else

#endif
        delete(pClient);
        return needContinue;
    }
};