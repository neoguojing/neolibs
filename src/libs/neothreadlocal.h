#ifndef NEOTHREADLOCAL

#define NEOTHREADLOCAL
#include "neoindex.h"  

namespace NEOLIB {

class Looper;

template <class OBJECT>
class ThreadLocal{

private:
    ThreadLocal(){
        if (gThreadLocalKey == NULL)
            CREATETHREADLOCALVAR(gThreadLocalKey)
    }
public:
    static ThreadLocal* gThreadLocal;

    static ThreadLocal* getInstance()
    {
        if(gThreadLocal!=NULL){
            return gThreadLocal;
        }
        return new ThreadLocal();
    }

    OBJECT *get(){
        return (OBJECT*)GETTHREADLOCALVAR(gThreadLocalKey);
    }

    int set(OBJECT* object){
        return (int)SETTHREADLOCALVAR(gThreadLocalKey,object);
    }

    static void free(){
        DELETETHREADLOCALVAR(gThreadLocalKey);
    }

private:
    static THREAD_LOCAL_KEY gThreadLocalKey;




};
template <class OBJECT>
THREAD_LOCAL_KEY ThreadLocal<OBJECT>::gThreadLocalKey = NULL;
template <class OBJECT>
ThreadLocal<OBJECT>* ThreadLocal<OBJECT>::gThreadLocal = NULL;

}

#endif