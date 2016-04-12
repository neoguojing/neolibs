
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

#ifdef WIN32
bool NeoTimer::CreateTimer(string timername,unsigned long delay,unsigned long interval,void* callback, void* param)
{
    HANDLE tTimer = INVALID_HANDLE_VALUE;
    if (!CreateTimerQueueTimer(&tTimer, m_hTimerQueue, (WAITORTIMERCALLBACK)callback, param,delay/1000, interval/1000, WT_EXECUTEDEFAULT))
	{
		printf("CreateTimer::CreateTimerQueueTimer fail\r\n");
        return false;
	}
    m_hTimers.insert(pair<string,HANDLE>(timername, tTimer));

    return true;
}
#else
bool NeoTimer::CreateTimer(string timername,unsigned long delay,unsigned long interval)
{
    PNEOTIMER tTimer = new NEOTIMER;
    tTimer->mTimer.it_value.tv_sec = delay/NEOSECONDINMICRO;
    tTimer->mTimer.it_value.tv_nsec =  (delay*1000)%NEOSECONDINNANO;
    tTimer->mTimer.it_interval.tv_sec = interval/NEOSECONDINMICRO;
    tTimer->mTimer.it_interval.tv_nsec = (interval*1000)%NEOSECONDINNANO;

    int timerId = timerfd_create(CLOCK_REALTIME, 0);
    m_myEpoll->makeSocketNonBlocking(timerId);
    m_myEpoll->addEvent(timerId,m_Event);
    if(-1==timerfd_settime(timerId, 0, &(tTimer->mTimer), &(tTimer->mOldTimer)))
    {
    	char * mesg = strerror(errno);
    	printf("CreateTimer::timerfd_settime fail errno=%s\r\n",mesg);
		delete(tTimer);
    	return false;
    }
    m_hTimers.insert(pair<string,int>(timername, timerId));

    return true;
}
#endif  

#ifdef WIN32
bool NeoTimer::CreateOneShotTimer(string timername, unsigned long delay,void* callback, void* param)
{

    HANDLE tTimer = INVALID_HANDLE_VALUE;
    if (!CreateTimerQueueTimer(&tTimer, m_hTimerQueue, (WAITORTIMERCALLBACK)callback, param,delay/1000, 0, WT_EXECUTEONLYONCE))
        return false;
    m_hTimers.insert(pair<string,HANDLE>(timername, tTimer));

    return true;
}
#else
bool NeoTimer::CreateOneShotTimer(string timername, unsigned long delay)
{
    PNEOTIMER tTimer = new NEOTIMER;
    
    tTimer->mTimer.it_value.tv_sec = delay/NEOSECONDINMICRO;
    tTimer->mTimer.it_value.tv_nsec =  (delay*1000)%NEOSECONDINNANO;
    tTimer->mTimer.it_interval.tv_sec = 0;
    tTimer->mTimer.it_interval.tv_nsec = 0;

    int timerId = timerfd_create(CLOCK_REALTIME, 0);
	m_myEpoll->makeSocketNonBlocking(timerId);
	m_myEpoll->addEvent(timerId,m_Event);
	if(-1==timerfd_settime(timerId, 0, &(tTimer->mTimer), NULL))
	{
		char * mesg = strerror(errno);
		printf("CreateOneShotTimer::timerfd_settime fail errno=%s\r\n",mesg);
		delete(tTimer);
		return false;
	}
	m_hTimers.insert(pair<string,int>(timername, timerId));

    return true;
}
#endif 

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
    map<string, int>::iterator iter;
#endif
    
    for(iter=m_hTimers.begin();iter!=m_hTimers.end();++iter)
    {
#ifdef WIN32
        printf("timer name:%s,handler:%p\r\n",iter->first.c_str(),iter->second);
#else
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
    //m_pNEOBaseLib->m_pTaskPool->RegisterATask(lpParam,TimerOrWaitFired);
}
#else
void NeoTimer::TimerRoutine(void* lpParam)
{
    printf("I am the timer\n");
}

void NeoTimer::StartTimer()
{
	m_pNEOBaseLib->m_pTaskPool->RegisterATask(NeoEpoll::loop,m_myEpoll);
}
#endif
};//NEOLIB
