#ifndef NEOSOCKETCOMMOM

#define NEOSOCKETCOMMOM
#include "neoindex.h" 

namespace NEOLIB {

class CNEOBaseLibrary;

/*工具类
  并非是真正的client
*/
class CClient

{

public:

   CClient(const WIN_LINUX_SOCKET socket,const sockaddr_in clientaddr);

   ~CClient(void);

public:
   void setDataBuffer(char *buf = NULL, const unsigned long buffsize = 0,IOTYPE iotype = IORead);
   void eraseDataBuffer();
#ifdef WIN32
   bool Recv(const SERVICE_TYPE svctype = TCP);

   bool Send(const SERVICE_TYPE svctype = TCP);
#else
   bool Recv(const SERVICE_TYPE svctype, char *buf, int len);

   bool Send(const SERVICE_TYPE svctype, char *buf, int len);
#endif

public:

   WIN_LINUX_SOCKET m_s;

   struct sockaddr_in m_addr;

   IO_OPERATION_DATA m_IoRecv;

   IO_OPERATION_DATA m_IoSend;
};


}//NEOLIB

#endif