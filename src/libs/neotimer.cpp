
#include "neoindex.h"
#include "neotimer.h"

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
NeoTimer::NeoTimer():
{

}
#endif

NeoTimer::~NeoTimer()
{
#ifdef WIN32
    if (!DeleteTimerQueue(m_hTimerQueue))
        printf("DeleteTimerQueue failed (%d)\n", GetLastError());
#else

#endif   
}

bool NeoTimer::CreateTimer(string timername, void* param)
{
#ifdef WIN32
    HANDLE tTimer = INVALID_HANDLE_VALUE;
    if (!CreateTimerQueueTimer(&tTimer, m_hTimerQueue, (WAITORTIMERCALLBACK)TimerRoutine, param,0, 1000, WT_EXECUTEDEFAULT))
        return false;
    m_hTimers.insert(pair<string,HANDLE>(timername, tTimer));
#else

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

#endif   
    return true;
}

void NeoTimer::TimerRoutine(void* lpParam, bool TimerOrWaitFired)
{

}

};//NEOLIB