#ifndef NEOLOOPER

#define NEOLOOPER
#include "neoindex.h"  
#include <queue>
#include <vector>
#include "neothreadlocal.h"

using namespace std;

namespace NEOLIB {

class Message;
struct MessageGreater;

namespace
{
    typedef priority_queue<Message, vector<Message>, MessageGreater> MessageQueue;
}

class Looper{

public:
    static Looper* myLooper();
    static void prepare(bool quitAllowed);
    static void loop();
    void quit();

    MessageQueue* getQueue();
    MessageQueue* setQueue(MessageQueue *queue);

    static ThreadLocal<Looper> *gLooperThreadLocal;
    void wake();
    void wait();
private:
    Looper(bool quitAllowed);
    MessageQueue *mMessageQueue;
    bool mRun;
    PIPE mWakeReadPipeFd;
    PIPE mWakeWritePipeFd;
#ifndef WIN32
    int mEpollFd;
#endif
};


}

#endif