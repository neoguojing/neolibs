#ifndef NEOSOCKETCLINET

#define NEOSOCKETCLINET
#include "neoindex.h" 
#include <string>
using namespace std;

namespace NEOLIB {

class CNEOBaseLibrary;

#ifndef WIN32

#else

#endif

class NeoClient{

public:
    NeoClient(const string addr, const unsigned short port,const SERVICE_TYPE svctype=(SERVICE_TYPE)0);
    ~NeoClient();

    bool init(const SERVICE_TYPE svctype);
    void setInetAddr(string addr, unsigned short port);
    void close();
    bool doConnection();
    bool doSend();
    bool doRecv();

    static bool connTask(void *pThis,int &nStatus);
    static bool recvTask(void *pThis,int &nStatus);
    static bool myTask(void *pThis,int &nStatus);

public:
    bool clientSwitch;
    CNEOBaseLibrary *m_pNEOBaseLib;
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA m_wsaData;
    IO_OPERATION_DATA m_IoRecv;  
    IO_OPERATION_DATA m_IoSend; 
#else

#endif
    WIN_LINUX_SOCKET m_Socket;
    struct sockaddr_in m_ServerAddr;
};



}//NEOLIB

#endif