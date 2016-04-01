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


/************************************************************************************/
T_Message::T_Message()
{
	msg_ptr[0] = 0;
	msg_ptr[1] = 0;
	msg_ptr[2] = 0;
	msg_ptr[3] = 0;
}

unsigned short&	T_Message::m_xmsg_id()
{
	return xmsg_id(msg_ptr);
}

unsigned char& T_Message::m_xmsg_type()
{
	return xmsg_type(msg_ptr);
}

void*& T_Message::m_xmsg_bufptr()
{
	return xmsg_bufptr(msg_ptr);
}

unsigned long& T_Message::m_xmsg_data1()
{
	return xmsg_data1(msg_ptr);
}

unsigned long& T_Message::m_xmsg_data2()
{
	return xmsg_data2(msg_ptr);
}

unsigned long& T_Message::m_xmsg_gettimerdata()
{
	return xmsg_gettimerdata(msg_ptr);
}

unsigned long&	T_Message::m_xmsg_bufuserdata()
{
	return xmsg_bufuserdata(msg_ptr);
}

}//neolib