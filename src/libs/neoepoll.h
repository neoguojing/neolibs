#ifndef _NEOEPOLL_

#define _NEOEPOLL_

#include "neoindex.h"

namespace NEOLIB {

#ifndef WIN32

class NeoEpoll{

public:
	NeoEpoll();
    ~NeoEpoll();

    int makeSocketNonBlocking (int &fd);
    int addEvent(const int fd, epoll_event &listened_evnet);
    int modEvent(const int fd, epoll_event &listened_evnet);
    int delEvent(const int fd, epoll_event &listened_evnet);

    void setEvent(int fd, unsigned int eventbit);

   static  bool loop(void *pThis,int &nStatus);

private:
    bool m_Switch;
    struct epoll_event m_Event;
    int m_epollFd;
    struct epoll_event m_Events[EPOLL_MAX_EVENTS];
};

#endif

}//NEOLIB

#endif
