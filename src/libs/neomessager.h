#ifndef NEOMESSAGER

#define NEOMESSAGER
#include "neoindex.h"  

namespace NEOLIB {

class Message;
class Handler;

typedef void(*HANDLER_CALLBACK)(Message *message);

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
    bool operator ()(const Message * a,const Message * b)
    {
        return a->mWhen<b->mWhen;
    }
};

struct MessageGreater
{
    bool operator ()(const Message * a,const Message * b)
    {
        return a->mWhen>b->mWhen;;
    }
};


#define xmsg_id(msg_ptr) \
        (*(unsigned short *)((unsigned long)msg_ptr + XMSG_ID))

#define xmsg_type(msg_ptr) \
        (*(unsigned char *)((unsigned long)msg_ptr + XMSG_TYPE))

#define xmsg_bufptr(msg_ptr) \
        (*(void **)((unsigned long)msg_ptr + XMSG_BUF_PTR))

#define xmsg_data1(msg_ptr) \
        (*(unsigned long *)((unsigned long)msg_ptr + XMSG_USER_DATA1))

#define xmsg_data2(msg_ptr) \
        (*(unsigned long *)((unsigned long)msg_ptr + XMSG_USER_DATA2))

#define xmsg_gettimerdata(msg_ptr) \
        (*(unsigned long *)((unsigned long)msg_ptr + XMSG_TIMER_USER_DATA))

#define xmsg_gettimerid(msg_ptr) \
  (*(unsigned long *)((unsigned long)msg_ptr + XMSG_TIMER_ID))

#define xmsg_bufuserdata(msg_ptr) \
        (*(unsigned long *)((unsigned long)msg_ptr + XMSG_BUFFER_USER_DATA))

/*
    系统级的消息存储格式
*/
class T_Message
{
protected:
	unsigned long	msg_ptr[4];

public:
	// access functions
	unsigned long* m_get_msg_ptr() { return msg_ptr; }

	// constructors and destructors
	T_Message();
	~T_Message() { }

	unsigned short&	m_xmsg_id();
	unsigned char& m_xmsg_type();
	void*& m_xmsg_bufptr();
	unsigned long&	m_xmsg_data1();
	unsigned long& m_xmsg_data2();
	unsigned long& m_xmsg_gettimerdata();
	unsigned long& m_xmsg_bufuserdata();
};


}//neolib

#endif