#ifndef NEOBASICTHREAD

#define NEOBASICTHREAD

#include "neoindex.h" 
#include "neolooper.h"
#include "neoprocesssync.h"

namespace NEOLIB {
#ifndef WIN32
//typedef void*(__stdcall *NEO_THREAD_CALLBACK)(void *pCallParam);
typedef void*(*NEO_THREAD_CALLBACK)(void *pCallParam);
#else
typedef unsigned (__stdcall *NEO_THREAD_CALLBACK)(void *pCallParam);
#endif

class Thread{
public:

    Thread():
      mThread(NULL),mId(0),mCallback(NULL)
    {}

    Thread(NEO_THREAD_CALLBACK callback):
      mThread(NULL),mId(0),mCallback(callback)
    {}

    static THREADFUNC(run,param)
    {
        printf("Thread::run \r\n");
        return NULL;
    }

    void start(){
        if (NULL == mCallback)
            THREADCREATE(Thread::run,this,mThread,mId)
        THREADCREATE(mCallback,this,mThread,mId)
    }
private:
    THREAD mThread;
    THREADID mId;
    NEO_THREAD_CALLBACK mCallback;
};

class ThreadWithLoop{
public: 
    
    ThreadWithLoop():
      mThread(NULL),mId(0),mMyLooper(NULL)
    {
        mSemaphore = new NeoSemaphore(0);
    }

    ~ThreadWithLoop()
    {
        delete(mSemaphore);
    }

    static THREADFUNC(run,param)
    {
        printf("ThreadWithLoop::run \r\n");
        Looper::prepare(true);
        ThreadWithLoop* temp = (ThreadWithLoop*)param;

        temp->setMyLooper(Looper::myLooper());
        temp->mSemaphore->post();
        Looper::loop();
        return NULL;
    }

    void start(){
        
        THREADCREATE(this->run,this,mThread,mId)
    }
    
    

    void setMyLooper(Looper *loop){
        mMyLooper = loop; 
    }
    
    void getMyLooper(Looper** looper){
        mSemaphore->wait();
        *looper = mMyLooper;
        mSemaphore->post(); 
    }

    //废弃接口，因为只能调用一次
    Looper* getMyLooper(){
        mSemaphore->wait();
        return mMyLooper;
    }

private:
    THREAD mThread;
    THREADID mId;
    Looper * mMyLooper;
    NeoSemaphore *mSemaphore;
};

}

#endif