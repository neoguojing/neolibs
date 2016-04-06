#ifndef NEOTIMER

#define NEOTIMER

#define NEOMAXTIMERCOUNT 1024
#include<string>
#include<map>
using namespace std;

namespace NEOLIB {


class NeoTimer{

public:
    NeoTimer();
    ~NeoTimer();

    bool CreateTimer(string timername, void* param=NULL);
    bool DeleteTimer(string timername);

    static void TimerRoutine(void* lpParam, bool TimerOrWaitFired);

private:

#ifdef WIN32
    map<string,HANDLE> m_hTimers;
    HANDLE m_hTimerQueue;
#else

#endif

};



}//NEOLIB

#endif