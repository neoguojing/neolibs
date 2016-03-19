#ifndef NEOPROCESSSYNC

#define NEOPROCESSSYNC
#include "neoindex.h"

namespace NEOLIB {

class NeoSemaphore{

public:
    NeoSemaphore(int num=1)
    {
        SEM_INIT(mSem,num)
    }

    ~NeoSemaphore()
    {
        SEM_FREE(mSem);
    }

    void get(int *value)
    {   
        SEM_GET(mSem,value);
    }

    void wait()
    {
        SEM_WAIT(mSem);
    }

    void post()
    {
        SEM_POST(mSem);
    }

    void close()
    {
        SEM_FREE(mSem);
    }

private:
    SEM mSem;
};



#ifndef WIN32
/*linux 条件变量*/
class LinuxCondVar{
public:

    LinuxCondVar(pthread_condattr_t *cattr = NULL)
    {
         pthread_cond_init(&mhCounter,cattr);
         mLock = MUTEXINIT(&mLock);
    }

    ~LinuxCondVar()
    {
          close();
    }

    void init()
    {
        pthread_cond_init(&mhCounter,NULL);
    }

    void signal()
    {
        pthread_cond_signal(&mhCounter);
    }

    void broadcast()
    {
        pthread_cond_broadcast(&mhCounter);
    }


    void close()
    {
        pthread_cond_destroy(&mhCounter);
    }

    void wait()
    {
        pthread_cond_wait(&mhCounter,&mLock);
    }

    void waitbytime(const struct timespec * abstime=NULL)
    {
        pthread_cond_timedwait(&mhCounter,&mLock,NULL);
    }

private:
    pthread_cond_t mhCounter;
    MUTEX mLock;
};

#else
/*
HANDLE hCounterIn
HANDLE hCounter= CreateMutex(NULL,FALSE,"sam sp 44");
hCounter=OpenMutex(MUTEX_ALL_ACCESS,FALSE,"sam sp 44");
HANDLE hThread[3];
WaitForSingleObject(hCounterIn,INFINITE);
ReleaseMutex(hCounterIn);
WaitForMultipleObjects(3,hThread,TRUE,INFINITE);
CloseHandle(hCounter);
-------------------------可以跨进程使用--------------------------
*/
#define MAX_THREAD 5
#define NEO_MUTEX "neo"
class GlobalMutex{
public:

    GlobalMutex(char *name = NEO_MUTEX)
    {
         mhCounter = CreateMutex(NULL,FALSE,name);
         resetThread();
    }

    ~GlobalMutex()
    {
          resetThread();
          close();
    }

    void init(char *name = NEO_MUTEX)
    {
        OpenMutex(MUTEX_ALL_ACCESS,FALSE,name);
    }

    void signal(char *name = NEO_MUTEX)
    {
        ReleaseMutex(mhCounter);
    }

    void close()
    {
        CloseHandle(mhCounter);
    }

    void wait()
    {
        if(0 !=mhThread[0])
        {
            WaitForMultipleObjects(3,mhThread,TRUE,INFINITE);
            return;
        }

        WaitForSingleObject(mhCounter,INFINITE);
    }

    void resetThread()
    {
        memset(mhThread,0,MAX_THREAD);
    }

    void addTHread(HANDLE& handle)
    {
        if(0 != mhThread[MAX_THREAD-1])
            return;
        int i = 0;
        for(;i<MAX_THREAD;i++)
        {
            if(0 == mhThread[i])
            {
                mhThread[i] = handle;
                break;
            }
        }
    }
private:
    HANDLE mhCounter;
    HANDLE mhThread[MAX_THREAD];
};

class WinEvent{
public:

    WinEvent(bool initial,char *name = NEO_MUTEX)
    {
         mhCounter = CreateEvent(NULL,FALSE,initial,name);
         resetThread();
    }

    ~WinEvent()
    {
          resetThread();
          close();
    }

    void init(char *name = NEO_MUTEX)
    {
        OpenEvent(MUTEX_ALL_ACCESS,FALSE,name);
    }

    //设为有信号状态
    void signal()
    {
        SetEvent(mhCounter);
    }

    /*设为无信号状态*/
     void reset()
    {
        ResetEvent(mhCounter);
    }

    void close()
    {
        CloseHandle(mhCounter);
    }

    void wait()
    {
        if(0 !=mhThread[0])
        {
            WaitForMultipleObjects(3,mhThread,TRUE,INFINITE);
            return;
        }

        WaitForSingleObject(mhCounter,INFINITE);
    }

    void resetThread()
    {
        memset(mhThread,0,MAX_THREAD);
    }

    void addTHread(HANDLE& handle)
    {
        if(0 != mhThread[MAX_THREAD-1])
            return;
        int i = 0;
        for(;i<MAX_THREAD;i++)
        {
            if(0 == mhThread[i])
            {
                mhThread[i] = handle;
                break;
            }
        }
    }
private:
    HANDLE mhCounter;
    HANDLE mhThread[MAX_THREAD];
};

#endif
}

#endif