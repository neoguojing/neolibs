#ifndef NEOSOCKETCLINET

#define NEOSOCKETCLINET
#include "neoindex.h" 
#include <string>
using namespace std;

class CNEOLowDebug;
namespace NEOLIB {

#ifndef WIN32

#else

#pragma once
typedef struct _io_operation_data

{
    OVERLAPPED overlapped;
    WSABUF dataBuf;
    char IoType;//IO操作类型：如READ或WRITE。
    BYTE len;//实际传输的数据长度。
}IO_OPERATION_DATA;

#endif

class NeoClient{

public:
    NeoClient(const string addr, const unsigned short port,const SERVICE_TYPE svctype=SERVICE_TYPE::TCP);
    ~NeoClient();

    void init(const SERVICE_TYPE svctype);
    void setInetAddr(string addr, unsigned short port);
    void close();
    bool send(const SERVICE_TYPE svctype, char *buf, int len);
    bool recv(const SERVICE_TYPE svctype, char *buf, int& len);
private:
    CNEOLowDebug *m_pDebug;
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA m_wsaData;
    IO_OPERATION_DATA m_IoRecv;  
    IO_OPERATION_DATA m_IoSend; 
#else

#endif
    WIN_LINUX_SOCKET m_Socket;
    sockaddr_in m_ServerAddr;
};



}//NEOLIB

#endif