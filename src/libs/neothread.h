#ifndef NEOTHREAD

#define NEOTHREAD
#include "neolock.h"

namespace NEOLIB {
class CNEOLog;
class  CNEOThreadPool;
class CNEOBaseLibrary;
class CNEOTaskRun;
class CNEOMemPoolWithLock;
class CThreadManager;
class CNEOLowDebug;
class CNEOMemQueueWithLock;
////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/*
//线程池测试调用示例类
class CTestThreadPool
{ 
public:
	CTestThreadPool()
    {
        m_bThreadContinue.Set(true); //线程持续标志置为真
        m_nThreadCount.Set(0); //线程计数器置为0，这些是赋初值动作
        m_pDebug=new CTonyLowDebug("","",true); //创建debug 对象
        m_pThreadPool=new CTonyThreadPool(m_pDebug);//创建线程池对象
        if(m_pThreadPool)
        { //如果一切OK，则开始注册任务线程回调。
            m_nThreadCount.Add(); //注意，线程启动前，计数器先+1
            //注册线程函数，注意，由于线程函数需要访问本类的管理变量，这里把this 指针传入
            m_pThreadPool->ThreadPoolRegTask(TestThread,this);
        }
    }
	~CTestThreadPool()
    {
        //请注意，这里是利用本类的线程控制变量，退出本类的线程
        //从本例的逻辑看，这个设计稍微有点多余，因为线程池随后的摧毁动作，也会退出线程
        //但请注意，在世纪工程中，线程池往往是全局唯一，是外部传入，因此
        //原则上，每个开启线程的应用类，都应该有自己的线程控制手段
        //在本对象关闭或摧毁时，必须安全退出自己所属的所有线程，方能防止bug。
        m_bThreadContinue.Set(false); //标准退出逻辑
        while(m_nThreadCount.Get()) {Sleep(MIN_SLEEP);}
        if(m_pThreadPool) //摧毁线程池
        {
            delete m_pThreadPool;
            m_pThreadPool=null;
        } 
        if(m_pDebug) //摧毁debug 对        象
        {
            delete m_pDebug;
            m_pDebug=null;
        }
    }
	private:
	//线程池的回调函数，也是就应用程序的线程函数
    //测试线程，注意其完全符合线程池回调函数构型，且是静态函数
	static void TestThread (void* pCallParam,MBOOL& bThreadContinue)
    {
        //强制指针类型转换，获得本对象指针
        CTestThreadPool* pThis=(CTestThreadPool*)pCallParam;
        while(XMG(bThreadContinue)) //这是标准写法，即线程池退出支持
        {   //注意这一句，一个循环可以有多个退出点
            //此处增加这一句，是支持本对象的退出请求
            if(!pThis->m_bThreadContinue.Get()) break;
            //...程序逻辑
            Sleep(MIN_SLEEP); //习惯性睡眠
        } pThis->m_nThreadCount.Dec(); //注意，退出时计数器-1
    }
private:
	CTonyLowDebug* m_pDebug; //Debug 对象指针
	CTonyThreadPool* m_pThreadPool; //线程池对象指针
	//这是比较特殊的一个例子，虽然线程池提供了最终的线程安全退出手段
	//但是，我们要知道，很多时候，我们的一个应用类，和线程池的生命周期并不一致
	//通常情况下，应用类生命周期较短，因此，应用类应该有自己的线程安全退出手段
	//因此，这里仿造线程池的设计，也声明了两个线程安全的管理变量
	CMbool m_bThreadContinue;
	CMint m_nThreadCount;
};


*/
//线程池回调函数
//每个回调函数有一次运行权
//运行结束，线程不退出，进入空闲状态
typedef void(*_TPOOL_CALLBACK)(void *pCallParam,MBOOL &bThreadContinue);

//线程池管理数据结构
typedef struct _THREAD_TOKEN_
{
	int m_nExitCode;             //本线程在整个线程池的编号
	MINT m_nState;                //线程状态机MIINT
	THREAD m_hThread;             //线程句柄
	THREADID m_nThreadID;         //线程ID
	_TPOOL_CALLBACK m_pCallback; //回调函数
	void* m_pCallParam;          //回调函数参数
	CNEOThreadPool *m_pThreadPoolObjext;//指向线程池对象的指针
}SThreadToken;
const unsigned long SThreadTokenSize=sizeof(SThreadToken);//结构体长度
//////////////////////////////////////////////////////////////
//类定义
class  CNEOThreadPool
{
public:
	CNEOThreadPool(CNEOLowDebug *pDebug);
	~CNEOThreadPool();
private:
	SThreadToken m_TToken[THIS_POOLTHREAD_MAX];//线程池任务参数静态数组
	MBOOL m_bThreadContinue;                   //所有thread继续的标志
	MINT m_nThreadPoolThreadCount;             //thread计数器
	MINT m_nThreadPoolIdleThreadCount;         //空闲的线程数量
	MUTEX m_RegisterLock;                      //线程注册临界区
	CNEOLowDebug *m_pDebug;                    //debug的对象指针
private:
	//这是真实的操作系统线程函数
	static THREADFUNC(ThreadPoolThread,pParam);     //线程池服务线程
	//static unsigned WINAPI ThreadPoolThread(LPVOID pParam);
	static THREADFUNC(ThreadPoolCtrlThread,pParam); //线程池管理线程
	//static unsigned WINAPI ThreadPoolCtrlThread(LPVOID pParam);
private:
	//检索没有使用的Token
	int SearchForNotUseToken(void);
	//获得一个空闲线程
	int GetAIdleThread(void);
	//完成具体注册动作
	int ThreadPoolRegisterANewThread(_TPOOL_CALLBACK pCallback,void *pParam);
	int ThreadPoolRegisterANewThreadWhile(_TPOOL_CALLBACK pCallback,void *pParam);
public:
	//注册一个新线程
	int ThreadPoolRegTask(_TPOOL_CALLBACK pCallback,void *pParam,bool bWaitForSuccess=true/*是否等待注册成功*/);
	bool TPALLThreadIsIdle(void);           //检查所有线程是否空闲
	bool ThreadPoolIsContinue(void);        //检查线程运行状态
	int getThreadPoolThreadCount(void);    
	int getThreadPoolIdleThreadCount(void);
	SThreadToken* getTToken(void);
};

void printThreadInfo(void *pCallParam,MBOOL &bThreadContinue);
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//任务池
/*
//任务池任务函数示例，静态函数
static bool TaskCallback(void* pCallParam,int& nStatus)
{
	//...
	switch(nStatus)
	{ 
	case 0: //init //初始化代码
		//...
		nStatus++;
		break;
	case 1: //loop //循环体
		//...
		if(...) //某种条件下，中断循环
		nStatus++;
		break;
	default: //exit //结束代码
		//...
		return false; //返回假，任务结束
	} 
	return true; //正常情况下，返回真，任务继续
}
*/
//改回调nStatus在RegisterATask中被赋值为0
typedef bool(*_TASKPOOL_CALLBACK)(void *pCallParam,int &nStatus);
////////////////////////////////////////////////////////////////////////////
//#define TASK_POOL_TOKEN_MAX (16*1024)      //最多并发任务数
//#define DEFAULT_THREAD_MAX (30)             //默认最大线程数
/////////////////////////////////////////////////////////////////////////
//任务池核心数据结构
typedef struct _TASK_POOL_TOKEN_
{
	_TASKPOOL_CALLBACK m_pCallback;                      //回调函数指针
	void               *m_pUserParam;                     //替用户传递的回调函数参数
	int                m_nUserStatus;                     //代替用户传递的状态值，初始为0
}STaskPoolToken;
const unsigned long STaskPoolTokenSize=sizeof(STaskPoolToken);
////////////////////////////////////////////////////////////////////////
class  CNEOTaskPool
{
public:
	CNEOTaskPool(CNEOLowDebug *pDebug,CNEOMemPoolWithLock *pMemPool,
		CNEOLog *m_pLog,int nMaxThread=DEFAULT_THREAD_MAX);

	~CNEOTaskPool();
private:
	CMRSWbool m_bThreadContinue;           
	CMRSWint m_nThreadCount;
	CNEOMemQueueWithLock *m_pTaskQueue;  //核心任务队列
	CNEOThreadPool *m_pThreadPool;       //线程池指针
	int m_nMaxThread;                    //最大线程的保存变量
	CMRSWint m_nThreadID;
	CNEOLowDebug *m_pDebug;    
	CNEOMemPoolWithLock *m_pMemPool;
	CNEOLog *m_pLog;
	
private:
	//真实的内部注册函数
	bool RegisterATaskDolt(STaskPoolToken *pToken,int nLimit=-1);
	//服务者线程
	bool TaskServiceThreadDolt(STaskPoolToken &Task);
	static void TaskServiceThread(void *pCallParam,
		MBOOL &bThreadContinue);
	//管理者线程
	static void TaskCtrlThread(void *pCallParam,
		MBOOL &bThreadContinue);
public:
	bool ICanWork(void);
	void PrintInfo();
	void XGSysLog(const char *szFormat,...);
	bool isTaskPoolContinue(void);
	int getMaxThread(void);
	int getThreadID(void);
	int getThreadCount(void);
public:
	//注册一个新任务
	bool RegisterATask(_TASKPOOL_CALLBACK pCallback,void *pUserParam=NULL);
};
/*
	only for test
*/
bool printTaskPoolInfo(void *pCallParam,int &nStatus);
/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//任务池简化类
//#define NEO_TASK_RUN_MAX_TASK 16              //最多步动作
typedef struct _NEO_TASK_RUN_INFO_
{
	int m_nTaskCount;                         //总共多少步骤
	void *m_pCallParam;                       //公用的回调函数参数
	//动作回调函数数组
	_TASKPOOL_CALLBACK m_CallbackArray[NEO_TASK_RUN_MAX_TASK];
}SNEOTaskRunInfo;
const unsigned long SNEOTaskRunInfoSize=sizeof(SNEOTaskRunInfo);
//用于CNEOTaskRun的结构
typedef struct _NEOTeskRunTaskCallback_Param_
{
	SNEOTaskRunInfo m_Info;                  //任务结构描述体
	//CNEOBaseLibrary *m_pNEOBaseLib;          //基本聚合工具类
	CNEOTaskRun *m_pThis;                    //任务运行体对象指针
	int m_nRunIndex;                         //当前执行的步据
	char szAppName[256];                     //应用名
}SNEOTestRunTaskCallbackParam;
const unsigned long SNEOTestRunTaskCallbackParamSize=sizeof(SNEOTestRunTaskCallbackParam);
//任务运行信息
/*
	CNEOTaskRunInfo是一个工具类，为
	CNEOTaskRun提供服务，负责任务的注册
*/
class  CNEOTaskRunInfo
{
public:
	CNEOTaskRunInfo(SNEOTestRunTaskCallbackParam *pParam);
	CNEOTaskRunInfo(SNEOTaskRunInfo *pInfo);
	CNEOTaskRunInfo();
	~CNEOTaskRunInfo();
public:
	SNEOTaskRunInfo m_Info;
	SNEOTaskRunInfo *m_pInfo;
private:
	static void Init(SNEOTaskRunInfo *pInfo);
public:
	//获得当前实际使用的数据区
	SNEOTaskRunInfo *GetInfoPoint(void);
	//设置共用参数指针
	void SetCallbackParam(void *pCallParam);
	//将新的任务回调函数添加到内部数据区的数组末尾
	bool AddTask(_TASKPOOL_CALLBACK pCallback,void *pCallParam);
	bool AddTask(_TASKPOOL_CALLBACK pCallback);
	//将数据结构的内容拷贝到当前本对象使用的数据结构体中
	void CopyFrom(SNEOTaskRunInfo *pInfo);
};
/////////////////////////////////////////////////////////////////////////
/*
适用于一次性执行的任务
//由于实际任务开发较为复杂，依赖条件较多，我们这里假设一个聚合工具类指针
CTonyBaseLibrary* g_pTonyBaseLib; //演示用的聚合工具类指针
class CTestTaskRun
{ 
private:
	//我们假设本对象的任务分三个片段，就是前文所述的三段式开发，代码此处略
	static bool TaskInit(void* pCallParam,int& nStatus); //初始化
	static bool TaskLoop(void* pCallParam,int& nStatus); //循环体
	static bool TaskExit(void* pCallParam,int& nStatus); //结束代码
	CTonyTaskRun* m_pTonyTaskRun; //线程池运行体指针
public:
	CTestTaskRun()
	{
		//创建线程池运行体
		m_pTonyTaskRun=new CTonyTaskRun(g_pTonyBaseLib);
		//注意这里，设置一个描述对象
		CTonyTaskRunInfo tri;
		tri.AddTask(TaskInit,this); //请注意，依次添加，顺序不能乱了
		tri.AddTask(TaskLoop,this);
		tri.AddTask(TaskExit,this);
		m_pTonyTaskRun->StartTask(&tri); //启动任务
	} 
	~CTestTaskRun()
	{
		if(m_pTonyTaskRun)
		{
			m_pTonyTaskRun->StopAll(); //结束所有任务
			delete m_pTonyTaskRun; //释放资源
			m_pTonyTaskRun=null;
		}
	}
};
*/
//任务运行类
class  CNEOTaskRun
{
public:
	CNEOTaskRun(CNEOBaseLibrary *pNEOBaseLib);
	CNEOTaskRun(CNEOMemPoolWithLock *pMemPool,CNEOLog *pLog,CNEOTaskPool *pTaskPool);
	~CNEOTaskRun();
private:
	//任务回调函数
	 static bool NEOTestRunTaskCallback(void *pCallParam,int &nStatus);
	CThreadManager m_ThreadManager;  
	//CNEOBaseLibrary *m_pNEOBaseLib;
	  //日志模块
    CNEOLog *m_pLog;
    //内存池
    CNEOMemPoolWithLock *m_pMemPool;
    //线程池
    CNEOTaskPool *m_pTaskPool;
public:
	void XGSysLog(const char *szFormat,...);
	//启动一个任务
	bool StartTask(_TASKPOOL_CALLBACK pCallback,void *pCallParam=NULL,char *szAppName=NULL);
	//利用Info描述启动多次任务
	bool StartTask(SNEOTaskRunInfo *pInfoStruct,char *szAppName=NULL);
	bool StartTask(CNEOTaskRunInfo *pInfoObject,char *szAppName=NULL);
	//停止所有任务
	void StopAll(void);
	bool IsRunning(void);
	int GetThreadCount(void);
	void PrintInfo(void);
};

}
#endif