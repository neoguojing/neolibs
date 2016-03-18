#ifndef NEOMESSAGER

#define NEOMESSAGER
#include "neoindex.h"  

namespace NEOLIB {

class Message;
class Handler;

typedef void(*HANDLER_CALLBACK)(Message &message);

class Message{

public:
    Message():mMsgId(0),
        mArgs(NULL),callback(NULL),mTarget(NULL),
        mSync(false){}

    static Message * obtain(Handler *handler);
    static Message * obtain(Handler *handler,int msgid);
    static Message * obtain(Handler *handler,int msgid,void *mArgs,HANDLER_CALLBACK callback);

    bool setSync(bool sync)
    {
        mSync= sync;
        return mSync;
    }
//private:
    long mWhen;
    int mMsgId;
    void *mArgs;
    HANDLER_CALLBACK callback;
    Handler *mTarget;
private:
    bool mSync;
};


struct MessageLess
{
    bool operator ()(const Message & a,const Message & b)
    {
        return a.mWhen<b.mWhen;
    }
};

struct MessageGreater
{
    bool operator ()(const Message & a,const Message & b)
    {
        return a.mWhen>b.mWhen;;
    }
};

}

#endif