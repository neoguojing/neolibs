#ifndef _NEOTIMER_

#define _NEOTIMER_

#include "neoindex.h"
#include<string>
#include<map>
using namespace std;

#define NEOMAXTIMERCOUNT 1024
namespace NEOLIB {


#define NEOSECONDINNANO 1000000000
#define NEOSECONDINMICRO 1000000
#define NEOSECONDINMILLI 1000

class CNEOBaseLibrary ;
#ifndef WIN32

/*typedef struct _neo_timer
{
    struct itimerval mTimer;
    struct itimerval mOldTimer;
}NEOTIMER,*PNEOTIMER;*/

typedef struct _neo_timer
{
    struct itimerspec   mTimer;
    struct itimerspec   mOldTimer;
}NEOTIMER,*PNEOTIMER;

class NeoEpoll;

#endif

class NeoTimer{

public:
    NeoTimer();
    ~NeoTimer();
	//delay and interval is in micro second.
    bool CreateTimer(string timername, unsigned long delay=0,unsigned long interval=NEOSECONDINMICRO,
		void* callback=TimerRoutine, void* param=NULL);
    bool CreateOneShotTimer(string timername,  unsigned long delay=NEOSECONDINMICRO,
		void* callback=TimerRoutine, void* param=NULL);
    bool DeleteTimer(string timername);
    void PrintInside();
    int GetTimerCount();
#ifdef WIN32
    static void TimerRoutine(void* lpParam=NULL, bool TimerOrWaitFired=false);
#else
    static void TimerRoutine(void* lpParam=NULL);
#endif

private:
    CNEOBaseLibrary *m_pNEOBaseLib;
#ifdef WIN32
    map<string,HANDLE> m_hTimers;
    HANDLE m_hTimerQueue;
#else
    void StartTimer();
    map<string,int> m_hTimers;
    NeoEpoll *m_myEpoll;
    struct epoll_event m_Event;
#endif

};



}//NEOLIB

#endif
