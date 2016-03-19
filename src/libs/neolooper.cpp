
#include "neomessager.h"
#include "neohandler.h"
#include "neolooper.h"


namespace NEOLIB {

#ifndef WIN32
extern int errno;
#endif

ThreadLocal<Looper> * Looper::gLooperThreadLocal = NULL;

Looper::Looper(bool quitAllowed){
#ifndef WIN32
    PIPE wakeFds[2];
    int result = PIPE_INIT(wakeFds);

    mWakeReadPipeFd = wakeFds[0];
    mWakeWritePipeFd = wakeFds[1];

    result = PIPE_NONBLOCK(mWakeReadPipeFd);
    result = PIPE_NONBLOCK(mWakeWritePipeFd);

    mEpollFd = epoll_create(EPOLL_SIZE_HINT);

    struct epoll_event eventItem;
    memset(&eventItem,0,sizeof(epoll_event));
    eventItem.events = EPOLLIN;
    eventItem.data.fd = mWakeReadPipeFd;
    result = epoll_ctl(mEpollFd,EPOLL_CTL_ADD,mWakeReadPipeFd,&eventItem);

#endif
    mMessageQueue = new MessageQueue();
    gLooperThreadLocal = ThreadLocal<Looper>::getInstance();
    mRun = true;
}

Looper* Looper::myLooper()
{
    return gLooperThreadLocal->get();
}

void Looper::loop()
{
    Looper *me = myLooper();
    if(me==NULL){
        return;
    }

   MessageQueue *queue = me->getQueue();

   for(;;){
        
       if(queue->empty()){
           NEOMinSleep();
       }else{
           Message &msg = const_cast<Message&>(queue->top());
           if(TimelsUp((time_t)msg.mWhen,0)){
               msg.mTarget->dispatchMessage(msg);
               queue->pop();
           }
           
       }
    }
}

void Looper::prepare(bool quitAllowed)
{
    //将Looper 作为本线程的任务加入到任务队列
    Looper *looper = new Looper(quitAllowed);
    if(gLooperThreadLocal->get()==NULL){
        
        gLooperThreadLocal->set(looper);
    }
}

void Looper::quit()
{
    if(getQueue()!=NULL){
        delete(getQueue());
        
    }
    if(myLooper()!=NULL)
        delete(myLooper());
    if(gLooperThreadLocal!=NULL)
        delete(gLooperThreadLocal);

    setQueue(NULL);
    gLooperThreadLocal = NULL;
    ThreadLocal<Looper>::free();
}

MessageQueue* Looper::getQueue()
{
    return mMessageQueue;
}

MessageQueue* Looper::setQueue(MessageQueue *queue)
{
    mMessageQueue = queue;
    return mMessageQueue;
}

void Looper::wake()
{
#ifndef WIN32
    size_t nWrite;
    do{
        nWrite = PIPE_WRITE(mWakeWritePipeFd,"w",1);
    }while(nWrite == -1 && errno == EINTR);

#endif
}

void Looper::wait()
{
#ifndef WIN32
    struct epoll_event eventItems[EPOLL_MAX_EVENTS];
    int eventCount = epoll_wait(mEpollFd,eventItems,EPOLL_MAX_EVENTS,NULL);
    int i = 0;
    for(;i<eventCount;i++){
        int fd = eventItems[i].data.fd;
        uint32_t epollEvents = eventItems[i].events;
        if(fd == mWakeReadPipeFd){
        
            if(epollEvents & EPOLLIN){
                //待实现的，将pipe中的数据读完
                //awoken();
                char out;
                PIPE_READ(mWakeWritePipeFd,&out,1);
            }
        }
    }

#endif
}

}