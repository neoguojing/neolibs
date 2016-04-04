
#include "neosocketcommon.h"


namespace NEOLIB{

CClient::CClient(const WIN_LINUX_SOCKET socket,const struct sockaddr_in clientaddr)
    :m_s(socket)
{
    memcpy(&m_addr,&clientaddr,sizeof(struct sockaddr_in));
#ifdef WIN32
    memset(&m_IoRecv,0,sizeof(IO_OPERATION_DATA));
    memset(&m_IoSend,0,sizeof(IO_OPERATION_DATA));
#endif
}

CClient::~CClient(void)
{
    WIN_LINUX_CloseSocket(m_s);
#ifdef WIN32
    eraseDataBuffer(IORead);
    eraseDataBuffer(IOWrite);
#endif
}

#ifdef WIN32
void CClient::setDataBuffer(char *buf, const unsigned long buffsize,IOTYPE iotype)
{
	if (iotype == IORead)
	{
		ZeroMemory(&m_IoRecv,sizeof(IO_OPERATION_DATA));
		m_IoRecv.IoType=IORead;
		m_IoRecv.len = NEO_SERVER_RECEIVE_BUFFER_SIZE;
		m_IoRecv.dataBuf.buf = new char[NEO_SERVER_RECEIVE_BUFFER_SIZE];
		m_IoRecv.dataBuf.len = NEO_SERVER_RECEIVE_BUFFER_SIZE;
	}
	else if (iotype == IOWrite)
	{
        int sendBufferSize = buffsize;
		ZeroMemory(&m_IoSend,sizeof(IO_OPERATION_DATA));
		m_IoSend.IoType=IOWrite;
        if (sendBufferSize > NEO_CLIENT_RECEIVE_BUFFER_SIZE)
        {
            m_IoSend.len = buffsize;
            m_IoSend.dataBuf.len = NEO_CLIENT_RECEIVE_BUFFER_SIZE;
        }
        else
        {
            m_IoSend.len = buffsize;
            m_IoSend.dataBuf.len = buffsize;
        }
		m_IoSend.dataBuf.buf = buf;
		
	}
}

void CClient::eraseDataBuffer(IOTYPE iotype)
{
	if(IORead == iotype)
    {
		if (m_IoRecv.dataBuf.buf != NULL)
		{
			delete(m_IoRecv.dataBuf.buf);
		}
		ZeroMemory(&m_IoRecv,sizeof(IO_OPERATION_DATA));
	}
	else
	{
		if (m_IoSend.dataBuf.buf != NULL)
		{
			delete(m_IoSend.dataBuf.buf);
		}
		ZeroMemory(&m_IoSend,sizeof(IO_OPERATION_DATA));
	}
}

#else
/*
void CClient::setDataBuffer(char *buf, const unsigned long buffsize,IOTYPE iotype)
{
        if (iotype == IORead)
	{
		memset(&m_IoRecv,0,sizeof(IO_OPERATION_DATA));
		m_IoRecv.IoType=IORead;
		m_IoRecv.len = NEO_SERVER_RECEIVE_BUFFER_SIZE;
		m_IoRecv.dataBuf = new char[NEO_SERVER_RECEIVE_BUFFER_SIZE];
		m_IoRecv.buffersize = NEO_SERVER_RECEIVE_BUFFER_SIZE;
	}
	else if (iotype == IOWrite)
	{
	        int sendBufferSize = buffsize;
		memset(&m_IoSend,0,sizeof(IO_OPERATION_DATA));
		m_IoSend.IoType=IOWrite;
	        if (sendBufferSize > NEO_CLIENT_RECEIVE_BUFFER_SIZE)
	        {
	            m_IoSend.len = buffsize;
	            m_IoSend.buffersize = NEO_CLIENT_RECEIVE_BUFFER_SIZE;
	        }
	        else
	        {
	            m_IoSend.len = buffsize;
	            m_IoSend.buffersize = buffsize;
	        }
		m_IoSend.dataBuf = buf;
	
	}
}

void CClient::eraseDataBuffer(IOTYPE iotype)
{
    if(IORead == iotype)
    {
	if (m_IoRecv.dataBuf != NULL)
	{
		delete(m_IoRecv.dataBuf);
	}
	memset(&m_IoRecv,0,sizeof(IO_OPERATION_DATA));
    }
    else
    {
	if (m_IoSend.dataBuf != NULL)
	{
		delete(m_IoSend.dataBuf);
	}
        memset(&m_IoSend,0,sizeof(IO_OPERATION_DATA));
    }
}
*/
#endif

#ifdef WIN32

bool CClient::Recv(const SERVICE_TYPE svctype)
{
    int addrsize = sizeof(m_addr);
    WSABUF wsabuf;
	wsabuf.buf = (char*)m_IoRecv.dataBuf.buf;
	wsabuf.len= m_IoRecv.dataBuf.len;
	DWORD flag=0;

    if(svctype == ::TCP)
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
    else if(svctype == ::UDP)
    {
        recvfrom(m_s,wsabuf.buf,wsabuf.len,flag,(SOCKADDR*)&m_addr,&addrsize);
    }

	return true;

}

bool CClient::Send(const SERVICE_TYPE svctype)
{
    bool needContinue = false;
    DWORD flag=0;
	WSABUF wsabuf;

	wsabuf.buf = m_IoSend.dataBuf.buf;
	wsabuf.len = m_IoSend.dataBuf.len;

    if(svctype == ::TCP)
    {
	    int ret=WSASend(m_s,&wsabuf,1,&wsabuf.len,flag,&m_IoSend.overlapped,NULL);
	    if(ret==SOCKET_ERROR)
	    {
		    int err=WSAGetLastError();
		    if(err!=WSA_IO_PENDING)
		    {
			    needContinue = false;
		    }
	    }
    }
    else if(svctype == ::UDP)
    {
        int rtn = sendto(m_s, wsabuf.buf, wsabuf.len, flag, (struct sockaddr *)&m_addr, sizeof(m_addr));  
        if (rtn < 0)  
        {  
            needContinue = false;
        }  
    }

    if (m_IoSend.len >=  NEO_CLIENT_RECEIVE_BUFFER_SIZE)
    {
        setDataBuffer(m_IoSend.dataBuf.buf+NEO_CLIENT_RECEIVE_BUFFER_SIZE,
            m_IoSend.len-NEO_CLIENT_RECEIVE_BUFFER_SIZE,IOWrite);
        needContinue = true;
    }

	return needContinue;
}
#else

bool CClient::Recv(const SERVICE_TYPE svctype)
{
    bool needContinue = false;
    /*int addrsize = sizeof(m_addr);
    int result = 0;

    setDataBuffer();
    IO_OPERATION_DATA IoRecv;
    IoRecv.IoType=IORead;
    IoRecv.len = NEO_SERVER_RECEIVE_BUFFER_SIZE;
    IoRecv.dataBuf = new char[NEO_SERVER_RECEIVE_BUFFER_SIZE];
    IoRecv.buffersize = NEO_SERVER_RECEIVE_BUFFER_SIZE;

    if(svctype == ::TCP)
    {
        while((result = read(m_s,m_IoRecv.dataBuf+m_IoRecv.len, 
            m_IoRecv.buffersize))>0)
        {
            m_IoRecv.len += result;
        }

	printf("recv msg = %s,len=%d!\r\n",m_IoRecv.dataBuf,m_IoRecv.len);

        if (result == -1 && errno != EAGAIN)
        {
            printf("CClient::Recv fail!\r\n");
            needContinue = true;
        }

    }
    else if(svctype == ::UDP)
    {
        recvfrom(m_s,m_IoRecv.dataBuf,m_IoRecv.len,0,(struct sockaddr *)&m_addr,(socklen_t*)&addrsize);
    }

    delete(IoRecv.dataBuf);*/
    return needContinue;
}

bool CClient::Send(const SERVICE_TYPE svctype)
{
    bool needContinue = false;
    /*int result = 0;
    int totalsent = 0;

    if(svctype == ::TCP)
    {
        int n = m_IoSend.len;

        while (n > 0)
        {
            result = write(m_s,m_IoSend.dataBuf+totalsent,n);
            if (result < n)
            {
                if (result == -1 && errno != EAGAIN)
                {
                    printf("CClient::Send fail!\r\n");
                    needContinue = true;
		    break;
                }
            }  
            totalsent += result;
	    n -= result;
        }
    }
    else if(svctype == ::UDP)
    {
        result = sendto(m_s, m_IoSend.dataBuf, m_IoSend.len, 0, (struct sockaddr *)&m_addr, sizeof(m_addr));  
        if (result < 0)  
        {  
			needContinue = true;
        }  
    }*/
    return needContinue;
}

#endif


}//NEOLIB
