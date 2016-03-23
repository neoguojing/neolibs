
#include "neomemmanager.h"
#include "neosocketclient.h"

namespace NEOLIB{

NeoClient::NeoClient(const string addr, const unsigned short port,const SERVICE_TYPE svctype)
{
    setInetAddr(addr,port);
    init(svctype);
}

NeoClient::~NeoClient()
{
    m_pDebug->DebugToFile("~NeoServer\r\n");
    if(m_pDebug)
    {
        delete m_pDebug;
        m_pDebug=NULL;
    }
    close();
}

void NeoClient::init(const SERVICE_TYPE svctype)
{
    switch((int)svctype)
    {
    case 0:
        m_Socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if(connect(m_Socket, (struct sockaddr*)&m_ServerAddr, sizeof(m_ServerAddr)) < 0)
        {
            m_pDebug->DebugToFile("connected to server fail!\r\n");
            ::WIN_LINUX_CloseSocket(m_Socket);
        }
        break;
    case 1:
        m_Socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        break;
    case 2:
        m_Socket = socket(AF_INET,SOCK_RAW,IPPROTO_IP);
        break;
    case 3:
        m_Socket = socket(AF_UNIX,SOCK_DGRAM,IPPROTO_TCP);
        break;
    }
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

void NeoClient::setInetAddr(string addr, unsigned short port)
{
    m_ServerAddr.sin_family=AF_INET;
    m_ServerAddr.sin_addr.s_addr= inet_addr(addr.c_str());
    m_ServerAddr.sin_port=htons(port);
}

void NeoClient::close()
{
    ::WIN_LINUX_CloseSocket(m_Socket);
}

#ifndef WIN32
    bool NeoClient::send(const SERVICE_TYPE svctype, char *buf, int len)
    {
        if((int)svctype == 0)
        {
        
        }
        else if((int)svctype == 1)
        {
           int rtn = sendto(m_Socket, buf, len, 0, (struct sockaddr *)&m_ServerAddr, sizeof(m_ServerAddr));  
           if (rtn < 0)  
           {  
               m_pDebug->DebugToFile("send udp fail!\r\n"); 
			   return false;
           }  
        }
		return true;
    }
    bool NeoClient::recv(const SERVICE_TYPE svctype,char *buf, int &len)
    {
        if((int)svctype == 0)
        {
            
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
            ZeroMemory(&m_IoSend,sizeof(IO_OPERATION_DATA));
	        WSABUF wsabuf[2];
	        m_IoSend.IoType=0;

	        //发送包头。
	        wsabuf[0].buf=buf;
	        wsabuf[0].len=512;
	        //送包体。
	        wsabuf[1].buf=new char[512];
	        wsabuf[1].len=512;
	        DWORD flag=0;
	        int ret=WSASend(m_Socket,wsabuf,2,NULL,flag,&m_IoSend.overlapped,NULL);
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
           int rtn = sendto(m_Socket, buf, len, 0, (struct sockaddr *)&m_ServerAddr, sizeof(m_ServerAddr));  
           if (rtn < 0)  
           {  
               m_pDebug->DebugToFile("send udp fail!\r\n");
           }  
        }
	    

    }

    bool NeoClient::recv(const SERVICE_TYPE svctype, char *buf, int& len)
    {

        if((int)svctype == 0)
        {
            WSABUF wsabuf;
	        ZeroMemory(&m_IoRecv,sizeof(IO_OPERATION_DATA));
	        m_IoRecv.IoType=1;
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
};