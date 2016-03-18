#include "neomessager.h"

namespace NEOLIB {

Message * Message::obtain(Handler *handler)
{   
    Message * outMsg = new Message();
    outMsg->mTarget = handler;
    return outMsg;
}

Message * Message::obtain(Handler *handler, int msgid)
{
    Message * outMsg = new Message();
    outMsg->mTarget = handler;
    outMsg->mMsgId = msgid;
    return outMsg;
}

Message * Message::obtain(Handler *handler,int msgid,void *mArgs,HANDLER_CALLBACK callback)
{
    Message * outMsg = new Message();
    outMsg->mTarget = handler;
    outMsg->mMsgId = msgid;
    outMsg->mArgs = mArgs;
    outMsg->callback = callback;
    return outMsg;
}

}