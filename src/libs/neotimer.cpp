
#include "neotimer.h"
#include "neoepoll.h"
#include "neobaselib.h"
#include "neothread.h"

namespace NEOLIB{


#ifdef WIN32
NeoTimer::NeoTimer():
    m_hTimerQueue(INVALID_HANDLE_VALUE)
{
    m_hTimerQueue = CreateTimerQueue();
    if (NULL == m_hTimerQueue)
    {
        printf("CreateTimerQueue failed (%d)\n", GetLastError());
        return;
    }
}
#else
NeoTimer::NeoTimer()
{
	m_pNEOBaseLib = CNEOBaseLibrary::getInstance("neotimer",".","timerlog",NULL);
	 m_myEpoll = new NeoEpoll();
	 if (m_myEpoll==NULL)
	 {
        return;
	 }
	 m_Event.events = EPOLLIN | EPOLLET;
	 StartTimer();
}
#endif

NeoTimer::~NeoTimer()
{
#ifdef WIN32
    if (!DeleteTimerQueue(m_hTimerQueue))
        printf("DeleteTimerQueue failed (%d)\n", GetLastError());
#else
    if (m_myEpoll)
    {
    	delete(m_myEpoll);
    	m_myEpoll = NULL;
    }

    /*map<string, PNEOTIMER>::iterator iter;
    for(iter=m_hTimers.begin();iter!=m_hTimers.end();++iter)
    {
        if(iter->second!=NULL)
            delete(iter->second);
    }*/
    map<string, int>::iterator iter;
	for(iter=m_hTimers.begin();iter!=m_hTimers.end();++iter)
	{
		if(iter->second!=0)
			close(iter->second);
	}
#endif
    if(!m_hTimers.empty())
    {
        m_hTimers.clear();
    }
}

bool NeoTimer::CreateTimer(string timername,unsigned long usec, void* param)
{
#ifdef WIN32
    HANDLE tTimer = INVALID_HANDLE_VALUE;
    if (!CreateTimerQueueTimer(&tTimer, m_hTimerQueue, (WAITORTIMERCALLBACK)TimerRoutine, param,0, usec/1000, WT_EXECUTEDEFAULT))
        return false;
    m_hTimers.insert(pair<string,HANDLE>(timername, tTimer));
#else
    //signal(SIGALRM, TimerRoutine);

    PNEOTIMER tTimer = new NEOTIMER;
    tTimer->mTimer.it_value.tv_sec = usec/1000000;
   // tTimer->mTimer.it_value.tv_usec = usec%1000000;
    tTimer->mTimer.it_value.tv_nsec =  (usec*1000)%1000000000;
    tTimer->mTimer.it_interval.tv_sec = usec/1000000;
    //tTimer->mTimer.it_interval.tv_usec = usec%1000000;
    tTimer->mTimer.it_interval.tv_nsec = (usec*1000)%1000000000;
    
   /* if (-1 == setitimer(ITIMER_REAL, &(tTimer->mTimer), &(tTimer->mOldTimer)))
    {
    	char * mesg = strerror(errno);
        printf("CreateTimer::setitimer fail errno=%s\r\n",mesg);
        return false;
    }
    m_hTimers.insert(pair<string,PNEOTIMER>(timername, tTimer));*/

    int timerId = timerfd_create(CLOCK_REALTIME, 0);
    m_myEpoll->makeSocketNonBlocking(timerId);
    m_myEpoll->addEvent(timerId,m_Event);
    if(-1==timerfd_settime(timerId, 0, &(tTimer->mTimer), &(tTimer->mOldTimer)))
    {
    	char * mesg = strerror(errno);
    	printf("CreateTimer::timerfd_settime fail errno=%s\r\n",mesg);
    	return false;
    }
    m_hTimers.insert(pair<string,int>(timername, timerId));

#endif   
    return true;
}

bool NeoTimer::CreateOneShotTimer(string timername, unsigned long usec, void* param)
{
#ifdef WIN32
    HANDLE tTimer = INVALID_HANDLE_VALUE;
    if (!CreateTimerQueueTimer(&tTimer, m_hTimerQueue, (WAITORTIMERCALLBACK)TimerRoutine, param,usec/1000, 0, WT_EXECUTEONLYONCE))
        return false;
    m_hTimers.insert(pair<string,HANDLE>(timername, tTimer));
#else
    //signal(SIGALRM, TimerRoutine);

    PNEOTIMER tTimer = new NEOTIMER;
    //tv_sec是秒部分，tv_usec是微秒部分
   /* tTimer->mTimer.it_value.tv_sec = usec/1000000;
    tTimer->mTimer.it_value.tv_usec = usec%1000000;
    tTimer->mTimer.it_interval.tv_sec = 0;
    tTimer->mTimer.it_interval.tv_usec = 0;*/
    
    tTimer->mTimer.it_value.tv_sec = usec/1000000;
    tTimer->mTimer.it_value.tv_nsec =  (usec*1000)%1000000000;
    tTimer->mTimer.it_interval.tv_sec = 0;
    tTimer->mTimer.it_interval.tv_nsec = 0;

    /*if (-1 == setitimer(ITIMER_REAL, &(tTimer->mTimer), NULL))
    {
        printf("CreateOneShotTimer::setitimer fail errno=%d\r\n",errno);
        return false;
    }
    m_hTimers.insert(pair<string,PNEOTIMER>(timername, tTimer));*/

    int timerId = timerfd_create(CLOCK_REALTIME, 0);
	m_myEpoll->makeSocketNonBlocking(timerId);
	m_myEpoll->addEvent(timerId,m_Event);
	if(-1==timerfd_settime(timerId, 0, &(tTimer->mTimer), NULL))
	{
		char * mesg = strerror(errno);
		printf("CreateOneShotTimer::timerfd_settime fail errno=%s\r\n",mesg);
		return false;
	}
	m_hTimers.insert(pair<string,int>(timername, timerId));

#endif   
    return true;
}

bool NeoTimer::DeleteTimer(string timername)
{
#ifdef WIN32
    map<string, HANDLE>::iterator iter; 
    iter = m_hTimers.find(timername);
    if (INVALID_HANDLE_VALUE==iter->second)
    {
        m_hTimers.erase(iter);
        return false;
    }
    DeleteTimerQueueTimer(m_hTimerQueue,iter->second,NULL);
    m_hTimers.erase(iter);
#else
    //map<string, PNEOTIMER>::iterator iter;
    //iter = m_hTimers.find(timername);
    //memset(iter->second,0,sizeof(NEOTIMER));

   /* if (-1 == setitimer(ITIMER_REAL, &(iter->second->mTimer), &(iter->second->mOldTimer)))
    {
        printf("DeleteTimer::setitimer fail errno=%d",errno);
        return false;
    }
    delete(iter->second);*/

    map<string, int>::iterator iter;
    iter = m_hTimers.find(timername);
    m_myEpoll->delEvent(iter->second,m_Event);
    close(iter->second);
    iter->second = 0;
    m_hTimers.erase(iter);
#endif   
    return true;
}

void NeoTimer::PrintInside()
{
#ifdef WIN32
    map<string, HANDLE>::iterator iter; 
#else
    //map<string, PNEOTIMER>::iterator iter;
    map<string, int>::iterator iter;
#endif
    
    for(iter=m_hTimers.begin();iter!=m_hTimers.end();++iter)
    {
#ifdef WIN32
        printf("timer name:%s,handler:%p\r\n",iter->first.c_str(),iter->second);
#else
        /*printf("timer name:%s,NEOTIMER:%p\r\n",iter->first.c_str(),iter->second);
        printf("mTimer:it_value.tv_sec=%d,it_value.tv=%d,it_interval.tv_sec=%d,it_interval.tv_usec=%d\r\n",
                iter->second->mTimer.it_value.tv_sec,
                iter->second->mTimer.it_value.tv_usec, 
                iter->second->mTimer.it_interval.tv_sec,
                iter->second->mTimer.it_interval.tv_usec);
        printf("mOldTimer:it_value.tv_sec=%d,it_value.tv=%d,it_interval.tv_sec=%d,it_interval.tv_usec=%d\r\n",
                iter->second->mOldTimer.it_value.tv_sec,
                iter->second->mOldTimer.it_value.tv_usec, 
                iter->second->mOldTimer.it_interval.tv_sec,
                iter->second->mOldTimer.it_interval.tv_usec);*/
        printf("timer name:%s,fd:%d\r\n",iter->first.c_str(),iter->second);
#endif
    }
}

int NeoTimer::GetTimerCount()
{
    return m_hTimers.size();
}

#ifdef WIN32
void NeoTimer::TimerRoutine(void* lpParam, bool TimerOrWaitFired)
{
    printf("I am the timer\n");
}
#else
void NeoTimer::TimerRoutine(int signo)
{
    if (SIGALRM == signo)
        printf("I am the timer\n");
}

void NeoTimer::StartTimer()
{
	m_pNEOBaseLib->m_pTaskPool->RegisterATask(NeoEpoll::loop,m_myEpoll);
}
#endif
};//NEOLIB
