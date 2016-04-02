
#include "neosocketcommon.h"


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

}//namespace

CClient::CClient(const WIN_LINUX_SOCKET socket,const struct sockaddr_in clientaddr)
    :m_s(socket)
{
    memcpy(&m_addr,&clientaddr,sizeof(struct sockaddr_in));
    memset(&m_IoRecv,0,sizeof(IO_OPERATION_DATA));
    memset(&m_IoSend,0,sizeof(IO_OPERATION_DATA));
}

CClient::~CClient(void)
{
    eraseDataBuffer();
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

bool CClient::Recv(const SERVICE_TYPE svctype, char *buf, int len)
{
    int addrsize = sizeof(m_addr);

    if(svctype == ::TCP)
    {
        if(read(m_Socket,buf,len) < 0)
            return false;
    }
    else if(svctype == ::UDP)
    {
        recvfrom(m_Socket,buf,len,0,(struct sockaddr *)&m_addr,(socklen_t*)&addrsize);
    }
	return true;
}

bool CClient::Send(const SERVICE_TYPE svctype, char *buf, int len)
{
    if(svctype == ::TCP)
    {
        if(0 > write(m_s, buf,len))
            return false;
    }
    else if(svctype == ::UDP)
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