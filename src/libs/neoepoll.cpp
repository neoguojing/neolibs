
#include "neoepoll.h"

namespace NEOLIB{

#ifndef WIN32

NeoEpoll::NeoEpoll():m_Switch(true)
{
	m_epollFd = epoll_create(EPOLL_SIZE_HINT);
	if (m_epollFd == -1)
	 {
		printf("NeoEpoll  create fail!\r\n");
		 return;
	 }
	memset(&m_Event,0,sizeof(m_Event));
	m_Event.events = EPOLLIN | EPOLLET;
	memset(m_Events,0,sizeof(m_Events));
}

NeoEpoll::~NeoEpoll()
{
	close();
}

int NeoEpoll::makeSocketNonBlocking (int& fd)
     {
         int flags, s;

          //得到文件状态标志
         flags = fcntl (fd, F_GETFL, 0);
         if (flags == -1)
         {
           perror ("fcntl get failed");
           return -1;
         }

         //设置文件状态标志
         flags |= O_NONBLOCK;
         s = fcntl (fd, F_SETFL, flags);
         if (s == -1)
         {
             perror ("fcntl set faied");
             return -1;
         }

         return 0;
     }

int NeoEpoll::addEvent( const int fd, epoll_event &listened_evnet)
   {
	   listened_evnet.data.fd = fd;
       int rtn = epoll_ctl(m_epollFd,EPOLL_CTL_ADD,fd,&listened_evnet);
       if (rtn==-1)
       {
            printf("addEvent fail!\r\n");
       }
       return rtn;
   }

   int NeoEpoll::modEvent(const int fd, epoll_event &listened_evnet)
   {
	   listened_evnet.data.fd = fd;
       int rtn = epoll_ctl(m_epollFd, EPOLL_CTL_MOD,fd,&listened_evnet);
       if (rtn == -1)
       {
           printf("modEvent fail!\r\n");
       }

       return rtn;
   }

   int NeoEpoll::delEvent(const int fd, epoll_event &listened_evnet)
      {
	      listened_evnet.data.fd = fd;
          int rtn = epoll_ctl(m_epollFd, EPOLL_CTL_DEL,fd,&listened_evnet);
          if (rtn == -1)
          {
              printf("delEvent fail!\r\n");
          }

          return rtn;
      }

   void NeoEpoll::setEvent(int fd, unsigned int eventbit)
   {
	    m_Event.data.fd = fd;
	   	m_Event.events = eventbit;
   }

   void NeoEpoll::close()
   {
	   m_Switch = false;
	   ::close(m_epollFd);
   }
   bool NeoEpoll::loop(void *pThis,int &nStatus)
       {
           bool needContinue = false;
           int numofwaitingfds = 0;
           int tempfd = 0;
           NeoEpoll *tThis = (NeoEpoll*)pThis;

           char buf[128] ={0};

           while(tThis->m_Switch)
           {
              //tThis->m_pNEOBaseLib->m_pDebug->DebugToFile("epoll_wait loop!\r\n");
              numofwaitingfds = epoll_wait(tThis->m_epollFd, tThis->m_Events, EPOLL_SIZE_HINT, -1);
              if (-1 == numofwaitingfds)
              {
                   printf("NeoEpoll::epoll_wait fail!\r\n");
                   continue;
              }

              int i = 0;
              for (;i < numofwaitingfds; ++i)
              {
                  tempfd = tThis->m_Events[i].data.fd;

                  if (tThis->m_Events[i].events & EPOLLIN)
                  {
                	  while (read(tempfd, buf, 128) > 0);
                	  printf("NeoEpoll::I am the Timer !\r\n");
                  }
                  else if (tThis->m_Events[i].events & EPOLLOUT)
                  {

                  }
              }//for

           }//while
           return needContinue;
       }
#endif

};//NEOLIB
