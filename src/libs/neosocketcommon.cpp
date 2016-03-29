
#include "neosocketcommon.h"


namespace NEOLIB{

CClient::CClient(const WIN_LINUX_SOCKET socket,const struct sockaddr_in clientaddr)
    :m_s(socket)
{
    memcpy(&m_addr,&clientaddr,sizeof(struct sockaddr_in));
    memset(&m_IoRecv,0,sizeof(IO_OPERATION_DATA));
    memset(&m_IoSend,0,sizeof(IO_OPERATION_DATA));
}

CClient::~CClient(void)
{

}

#ifdef WIN32
void CClient::setDataBuffer(char *buf, const unsigned long buffsize,IOTYPE iotype)
{
	if (iotype == IORead)
	{
		ZeroMemory(&m_IoRecv,sizeof(IO_OPERATION_DATA));
		m_IoRecv.IoType=IORead;
		m_IoRecv.len = 1024;
		m_IoRecv.dataBuf.buf = new char[1024];
		m_IoRecv.dataBuf.len = 1024;
	}
	else if (iotype == IOWrite)
	{
		ZeroMemory(&m_IoSend,sizeof(IO_OPERATION_DATA));
		m_IoSend.IoType=IOWrite;
		m_IoSend.dataBuf.buf = buf;
		m_IoSend.dataBuf.len = buffsize;
	}
}

void CClient::eraseDataBuffer()
{
	if (m_IoRecv.dataBuf.buf != NULL)
	{
		delete(m_IoRecv.dataBuf.buf);
	}

	if (m_IoSend.dataBuf.buf != NULL)
	{
		delete(m_IoSend.dataBuf.buf);
	}

	ZeroMemory(&m_IoRecv,sizeof(IO_OPERATION_DATA));
	ZeroMemory(&m_IoSend,sizeof(IO_OPERATION_DATA));
}
#else
#endif

#ifdef WIN32

bool CClient::Recv(const SERVICE_TYPE svctype)
{
    int addrsize = sizeof(m_addr);
    WSABUF wsabuf;
	//ZeroMemory(&m_IoRecv,sizeof(IO_OPERATION_DATA));
	//m_IoRecv.IoType=IORead;
	wsabuf.buf = (char*)m_IoRecv.dataBuf.buf;
	wsabuf.len= m_IoRecv.dataBuf.len;
	DWORD flag=0;

    if(svctype == SERVICE_TYPE::TCP)
    {
	    int ret=WSARecv(m_s,&wsabuf,1,&m_IoRecv.len,&flag,&m_IoRecv.overlapped,NULL);
	    if(ret==SOCKET_ERROR)
	    {
		    int err=WSAGetLastError();
		    if(err!=WSA_IO_PENDING)
		    {
			    return false;
		    }
	    }
    }
    else if(svctype == SERVICE_TYPE::UDP)
    {
        recvfrom(m_s,wsabuf.buf,wsabuf.len,flag,(SOCKADDR*)&m_addr,&addrsize);
    }

	return true;

}

bool CClient::Send(const SERVICE_TYPE svctype)
{
	//ZeroMemory(&m_IoSend,sizeof(IO_OPERATION_DATA));

	WSABUF wsabuf;
	//m_IoSend.IoType=IOWrite;
    m_IoSend.len = m_IoSend.dataBuf.len;
	wsabuf.buf = m_IoSend.dataBuf.buf;
	wsabuf.len = m_IoSend.dataBuf.len;
	DWORD flag=0;

    if(svctype == SERVICE_TYPE::TCP)
    {
	    int ret=WSASend(m_s,&wsabuf,1,NULL,flag,&m_IoSend.overlapped,NULL);
	    if(ret==SOCKET_ERROR)
	    {
		    int err=WSAGetLastError();
		    if(err!=WSA_IO_PENDING)
		    {
			    return false;
		    }
	    }
    }
    else if(svctype == SERVICE_TYPE::UDP)
    {
        int rtn = sendto(m_s, wsabuf.buf, wsabuf.len, flag, (struct sockaddr *)&m_addr, sizeof(m_addr));  
        if (rtn < 0)  
        {  
            return false;
        }  
    }
	return true;
}
#else

bool CClient::Recv(const SERVICE_TYPE svctype, char *buf, int len)
{
    int addrsize = sizeof(m_addr);

    if(svctype == SERVICE_TYPE::TCP)
    {
        if(read(m_Socket,buf,len) < 0)
            return false;
    }
    else if(svctype == SERVICE_TYPE::UDP)
    {
        recvfrom(m_Socket,buf,len,0,(struct sockaddr *)&m_addr,(socklen_t*)&addrsize);
    }
	return true;
}

bool CClient::Send(const SERVICE_TYPE svctype, char *buf, int len)
{
    if(svctype == SERVICE_TYPE::TCP)
    {
        if(0 > write(m_s, buf,len))
            return false;
    }
    else if(svctype == SERVICE_TYPE::UDP)
    {
        int rtn = sendto(m_s, buf, len, 0, (struct sockaddr *)&m_addr, sizeof(m_addr));  
        if (rtn < 0)  
        {  
			return false;
        }  
    }
    return true;
}

#endif


}//NEOLIB