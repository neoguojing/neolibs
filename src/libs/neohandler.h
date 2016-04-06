#ifndef NEOHANDLER

#define NEOHANDLER
#include "neoindex.h" 
#include <queue>
#include <vector>

using namespace std;

namespace NEOLIB {

class Message;
class Looper;
struct MessageGreater;
typedef void(*HANDLER_CALLBACK)(Message *message);
namespace
{
    typedef priority_queue<Message*, vector<Message*>, MessageGreater> MessageQueue;
}
class Handler{

public:
    
    Handler();
    Handler(Looper* looper, HANDLER_CALLBACK callback, bool sync);
    Handler(Looper* looper);
    Handler(HANDLER_CALLBACK callback, bool sync);

    ~Handler(){}

    virtual void handleMessage(Message *message);
    void dispatchMessage(Message* msg);
    bool sendMessageDelayed(Message* msg, long timeinsec);
    bool sendMessage(Message* msg);
    bool sendMessageAtTime(Message* msg,long timeinsec);

    Message *obtainMessage();
    Message *obtainMessage(int msgid);
    Message *obtainMessage(int msgid,void *mArgs,HANDLER_CALLBACK callback);

private:
    //未完待续
    bool enQueueMessage(MessageQueue* queue, Message* msg, long timeinsec);
public:
    Looper *mLooper;
    MessageQueue *mMessageQueue;
    HANDLER_CALLBACK mCallback;
    bool mSync;
};

}

#endif