#ifndef NEOTIMER

#define NEOTIMER

#define NEOMAXTIMERCOUNT 1024
#include<string>
#include<map>
using namespace std;

namespace NEOLIB {

#ifndef WIN32

typedef struct _neo_timer
{
    struct itimerval mTimer;
    struct itimerval mOldTimer;
}NEOTIMER,*PNEOTIMER;

#endif

class NeoTimer{

public:
    NeoTimer();
    ~NeoTimer();

    bool CreateTimer(string timername, unsigned long usec=1000000, void* param=NULL);
    bool CreateOneShotTimer(string timername, unsigned long usec=1000000, void* param=NULL);
    bool DeleteTimer(string timername);
    void PrintInside();
    int GetTimerCount();
#ifdef WIN32
    static void TimerRoutine(void* lpParam=NULL, bool TimerOrWaitFired=false);
#else
    static void TimerRoutine(int signo);
#endif

private:

#ifdef WIN32
    map<string,HANDLE> m_hTimers;
    HANDLE m_hTimerQueue;
#else
    map<string,PNEOTIMER> m_hTimers;
#endif

};



}//NEOLIB

#endif