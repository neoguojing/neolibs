
#include "neolooper.h"
#include "neomessager.h"
#include "neohandler.h"


namespace NEOLIB{

Handler::Handler(Looper* looper, HANDLER_CALLBACK callback, bool sync):
        mLooper(looper),
        mMessageQueue(looper->getQueue()),
        mCallback(callback),
        mSync(sync)
{
      
}

Handler::Handler():
        mLooper(Looper::myLooper()),
        mMessageQueue(mLooper->getQueue()),
        mCallback(NULL),
        mSync(false)
{   

}

Handler::Handler(Looper* looper):
    mLooper(looper),
    mMessageQueue(looper->getQueue()),
    mCallback(NULL),
    mSync(false)
{   
}

Handler::Handler(HANDLER_CALLBACK callback, bool sync):
        mLooper(Looper::myLooper()),
        mMessageQueue(mLooper->getQueue()),
        mCallback(callback),
        mSync(sync)
{

}

void Handler::dispatchMessage(Message* msg)
{
    if(msg->callback!=NULL){
        msg->callback(msg);
    }else{
        if(mCallback!=NULL){
            mCallback(msg);
            return;
        }
        handleMessage(msg);
    }
}

bool Handler::sendMessageAtTime(Message* msg,long timeinsec)
{
        MessageQueue *queue = mMessageQueue;
        if (queue==NULL){
            return false;
        }
        return enQueueMessage(queue,msg,timeinsec);
}

bool Handler::sendMessageDelayed(Message* msg, long timeinsec)
{
    if(timeinsec<0)
        timeinsec = 0;
    return sendMessageAtTime(msg,timeinsec);
}

bool Handler::sendMessage(Message* msg)
{
    return sendMessageDelayed(msg,0);
}

void Handler::handleMessage(Message* message)
{
    printf("Handler::handleMessage msgid=%d\n",message->mMsgId);
    if (NULL != message)
    {
        delete(message);
        message = NULL;
    }
}

bool Handler::enQueueMessage(MessageQueue* queue, Message* msg, long timeinsec)
{
    time_t tNow;
	TimeSetNow(tNow);

    msg->mTarget = this;
    msg->mWhen = (long)tNow+timeinsec;
    if(mSync){
        msg->setSync(true);
    }

    queue->push(msg);
    return true;
}

Message *Handler::obtainMessage()
{
    return Message::obtain(this);
}

Message *Handler::obtainMessage(int msgid)
{
    return Message::obtain(this,msgid);
}

Message *Handler::obtainMessage(int msgid,void *mArgs,HANDLER_CALLBACK callback)
{
    return Message::obtain(this,msgid,mArgs,callback);
}


};