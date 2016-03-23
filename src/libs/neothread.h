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
//�̳߳ز��Ե���ʾ����
class CTestThreadPool
{ 
public:
	CTestThreadPool()
    {
        m_bThreadContinue.Set(true); //�̳߳�����־��Ϊ��
        m_nThreadCount.Set(0); //�̼߳�������Ϊ0����Щ�Ǹ���ֵ����
        m_pDebug=new CTonyLowDebug("","",true); //����debug ����
        m_pThreadPool=new CTonyThreadPool(m_pDebug);//�����̳߳ض���
        if(m_pThreadPool)
        { //���һ��OK����ʼע�������̻߳ص���
            m_nThreadCount.Add(); //ע�⣬�߳�����ǰ����������+1
            //ע���̺߳�����ע�⣬�����̺߳�����Ҫ���ʱ���Ĺ�������������this ָ�봫��
            m_pThreadPool->ThreadPoolRegTask(TestThread,this);
        }
    }
	~CTestThreadPool()
    {
        //��ע�⣬���������ñ�����߳̿��Ʊ������˳�������߳�
        //�ӱ������߼�������������΢�е���࣬��Ϊ�̳߳����Ĵݻٶ�����Ҳ���˳��߳�
        //����ע�⣬�����͹����У��̳߳�������ȫ��Ψһ�����ⲿ���룬���
        //ԭ���ϣ�ÿ�������̵߳�Ӧ���࣬��Ӧ�����Լ����߳̿����ֶ�
        //�ڱ�����رջ�ݻ�ʱ�����밲ȫ�˳��Լ������������̣߳����ܷ�ֹbug��
        m_bThreadContinue.Set(false); //��׼�˳��߼�
        while(m_nThreadCount.Get()) {Sleep(MIN_SLEEP);}
        if(m_pThreadPool) //�ݻ��̳߳�
        {
            delete m_pThreadPool;
            m_pThreadPool=null;
        } 
        if(m_pDebug) //�ݻ�debug ��        ��
        {
            delete m_pDebug;
            m_pDebug=null;
        }
    }
	private:
	//�̳߳صĻص�������Ҳ�Ǿ�Ӧ�ó�����̺߳���
    //�����̣߳�ע������ȫ�����̳߳ػص��������ͣ����Ǿ�̬����
	static void TestThread (void* pCallParam,MBOOL& bThreadContinue)
    {
        //ǿ��ָ������ת������ñ�����ָ��
        CTestThreadPool* pThis=(CTestThreadPool*)pCallParam;
        while(XMG(bThreadContinue)) //���Ǳ�׼д�������̳߳��˳�֧��
        {   //ע����һ�䣬һ��ѭ�������ж���˳���
            //�˴�������һ�䣬��֧�ֱ�������˳�����
            if(!pThis->m_bThreadContinue.Get()) break;
            //...�����߼�
            Sleep(MIN_SLEEP); //ϰ����˯��
        } pThis->m_nThreadCount.Dec(); //ע�⣬�˳�ʱ������-1
    }
private:
	CTonyLowDebug* m_pDebug; //Debug ����ָ��
	CTonyThreadPool* m_pThreadPool; //�̳߳ض���ָ��
	//���ǱȽ������һ�����ӣ���Ȼ�̳߳��ṩ�����յ��̰߳�ȫ�˳��ֶ�
	//���ǣ�����Ҫ֪�����ܶ�ʱ�����ǵ�һ��Ӧ���࣬���̳߳ص��������ڲ���һ��
	//ͨ������£�Ӧ�����������ڽ϶̣���ˣ�Ӧ����Ӧ�����Լ����̰߳�ȫ�˳��ֶ�
	//��ˣ���������̳߳ص���ƣ�Ҳ�����������̰߳�ȫ�Ĺ������
	CMbool m_bThreadContinue;
	CMint m_nThreadCount;
};


*/
//�̳߳ػص�����
//ÿ���ص�������һ������Ȩ
//���н������̲߳��˳����������״̬
typedef void(*_TPOOL_CALLBACK)(void *pCallParam,MBOOL &bThreadContinue);

//�̳߳ع������ݽṹ
typedef struct _THREAD_TOKEN_
{
	int m_nExitCode;             //���߳��������̳߳صı��
	MINT m_nState;                //�߳�״̬��MIINT
	THREAD m_hThread;             //�߳̾��
	THREADID m_nThreadID;         //�߳�ID
	_TPOOL_CALLBACK m_pCallback; //�ص�����
	void* m_pCallParam;          //�ص���������
	CNEOThreadPool *m_pThreadPoolObjext;//ָ���̳߳ض����ָ��
}SThreadToken;
const unsigned long SThreadTokenSize=sizeof(SThreadToken);//�ṹ�峤��
//////////////////////////////////////////////////////////////
//�ඨ��
class  CNEOThreadPool
{
public:
	CNEOThreadPool(CNEOLowDebug *pDebug);
	~CNEOThreadPool();
private:
	SThreadToken m_TToken[THIS_POOLTHREAD_MAX];//�̳߳����������̬����
	MBOOL m_bThreadContinue;                   //����thread�����ı�־
	MINT m_nThreadPoolThreadCount;             //thread������
	MINT m_nThreadPoolIdleThreadCount;         //���е��߳�����
	MUTEX m_RegisterLock;                      //�߳�ע���ٽ���
	CNEOLowDebug *m_pDebug;                    //debug�Ķ���ָ��
private:
	//������ʵ�Ĳ���ϵͳ�̺߳���
	static THREADFUNC(ThreadPoolThread,pParam);     //�̳߳ط����߳�
	//static unsigned WINAPI ThreadPoolThread(LPVOID pParam);
	static THREADFUNC(ThreadPoolCtrlThread,pParam); //�̳߳ع����߳�
	//static unsigned WINAPI ThreadPoolCtrlThread(LPVOID pParam);
private:
	//����û��ʹ�õ�Token
	int SearchForNotUseToken(void);
	//���һ�������߳�
	int GetAIdleThread(void);
	//��ɾ���ע�ᶯ��
	int ThreadPoolRegisterANewThread(_TPOOL_CALLBACK pCallback,void *pParam);
	int ThreadPoolRegisterANewThreadWhile(_TPOOL_CALLBACK pCallback,void *pParam);
public:
	//ע��һ�����߳�
	int ThreadPoolRegTask(_TPOOL_CALLBACK pCallback,void *pParam,bool bWaitForSuccess=true/*�Ƿ�ȴ�ע��ɹ�*/);
	bool TPALLThreadIsIdle(void);           //��������߳��Ƿ����
	bool ThreadPoolIsContinue(void);        //����߳�����״̬
	int getThreadPoolThreadCount(void);    
	int getThreadPoolIdleThreadCount(void);
	SThreadToken* getTToken(void);
};

void printThreadInfo(void *pCallParam,MBOOL &bThreadContinue);
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//�����
/*
//�����������ʾ������̬����
static bool TaskCallback(void* pCallParam,int& nStatus)
{
	//...
	switch(nStatus)
	{ 
	case 0: //init //��ʼ������
		//...
		nStatus++;
		break;
	case 1: //loop //ѭ����
		//...
		if(...) //ĳ�������£��ж�ѭ��
		nStatus++;
		break;
	default: //exit //��������
		//...
		return false; //���ؼ٣��������
	} 
	return true; //��������£������棬�������
}
*/
//�Ļص�nStatus��RegisterATask�б���ֵΪ0
typedef bool(*_TASKPOOL_CALLBACK)(void *pCallParam,int &nStatus);
////////////////////////////////////////////////////////////////////////////
//#define TASK_POOL_TOKEN_MAX (16*1024)      //��ಢ��������
//#define DEFAULT_THREAD_MAX (30)             //Ĭ������߳���
/////////////////////////////////////////////////////////////////////////
//����غ������ݽṹ
typedef struct _TASK_POOL_TOKEN_
{
	_TASKPOOL_CALLBACK m_pCallback;                      //�ص�����ָ��
	void               *m_pUserParam;                     //���û����ݵĻص���������
	int                m_nUserStatus;                     //�����û����ݵ�״ֵ̬����ʼΪ0
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
	CNEOMemQueueWithLock *m_pTaskQueue;  //�����������
	CNEOThreadPool *m_pThreadPool;       //�̳߳�ָ��
	int m_nMaxThread;                    //����̵߳ı������
	CMRSWint m_nThreadID;
	CNEOLowDebug *m_pDebug;    
	CNEOMemPoolWithLock *m_pMemPool;
	CNEOLog *m_pLog;
	
private:
	//��ʵ���ڲ�ע�ắ��
	bool RegisterATaskDolt(STaskPoolToken *pToken,int nLimit=-1);
	//�������߳�
	bool TaskServiceThreadDolt(STaskPoolToken &Task);
	static void TaskServiceThread(void *pCallParam,
		MBOOL &bThreadContinue);
	//�������߳�
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
	//ע��һ��������
	bool RegisterATask(_TASKPOOL_CALLBACK pCallback,void *pUserParam=NULL);
};
/*
	only for test
*/
bool printTaskPoolInfo(void *pCallParam,int &nStatus);
/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//����ؼ���
//#define NEO_TASK_RUN_MAX_TASK 16              //��ಽ����
typedef struct _NEO_TASK_RUN_INFO_
{
	int m_nTaskCount;                         //�ܹ����ٲ���
	void *m_pCallParam;                       //���õĻص���������
	//�����ص���������
	_TASKPOOL_CALLBACK m_CallbackArray[NEO_TASK_RUN_MAX_TASK];
}SNEOTaskRunInfo;
const unsigned long SNEOTaskRunInfoSize=sizeof(SNEOTaskRunInfo);
//����CNEOTaskRun�Ľṹ
typedef struct _NEOTeskRunTaskCallback_Param_
{
	SNEOTaskRunInfo m_Info;                  //����ṹ������
	//CNEOBaseLibrary *m_pNEOBaseLib;          //�����ۺϹ�����
	CNEOTaskRun *m_pThis;                    //�������������ָ��
	int m_nRunIndex;                         //��ǰִ�еĲ���
	char szAppName[256];                     //Ӧ����
}SNEOTestRunTaskCallbackParam;
const unsigned long SNEOTestRunTaskCallbackParamSize=sizeof(SNEOTestRunTaskCallbackParam);
//����������Ϣ
/*
	CNEOTaskRunInfo��һ�������࣬Ϊ
	CNEOTaskRun�ṩ���񣬸��������ע��
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
	//��õ�ǰʵ��ʹ�õ�������
	SNEOTaskRunInfo *GetInfoPoint(void);
	//���ù��ò���ָ��
	void SetCallbackParam(void *pCallParam);
	//���µ�����ص�������ӵ��ڲ�������������ĩβ
	bool AddTask(_TASKPOOL_CALLBACK pCallback,void *pCallParam);
	bool AddTask(_TASKPOOL_CALLBACK pCallback);
	//�����ݽṹ�����ݿ�������ǰ������ʹ�õ����ݽṹ����
	void CopyFrom(SNEOTaskRunInfo *pInfo);
};
/////////////////////////////////////////////////////////////////////////
/*
������һ����ִ�е�����
//����ʵ�����񿪷���Ϊ���ӣ����������϶࣬�����������һ���ۺϹ�����ָ��
CTonyBaseLibrary* g_pTonyBaseLib; //��ʾ�õľۺϹ�����ָ��
class CTestTaskRun
{ 
private:
	//���Ǽ��豾��������������Ƭ�Σ�����ǰ������������ʽ����������˴���
	static bool TaskInit(void* pCallParam,int& nStatus); //��ʼ��
	static bool TaskLoop(void* pCallParam,int& nStatus); //ѭ����
	static bool TaskExit(void* pCallParam,int& nStatus); //��������
	CTonyTaskRun* m_pTonyTaskRun; //�̳߳�������ָ��
public:
	CTestTaskRun()
	{
		//�����̳߳�������
		m_pTonyTaskRun=new CTonyTaskRun(g_pTonyBaseLib);
		//ע���������һ����������
		CTonyTaskRunInfo tri;
		tri.AddTask(TaskInit,this); //��ע�⣬������ӣ�˳��������
		tri.AddTask(TaskLoop,this);
		tri.AddTask(TaskExit,this);
		m_pTonyTaskRun->StartTask(&tri); //��������
	} 
	~CTestTaskRun()
	{
		if(m_pTonyTaskRun)
		{
			m_pTonyTaskRun->StopAll(); //������������
			delete m_pTonyTaskRun; //�ͷ���Դ
			m_pTonyTaskRun=null;
		}
	}
};
*/
//����������
class  CNEOTaskRun
{
public:
	CNEOTaskRun(CNEOBaseLibrary *pNEOBaseLib);
	CNEOTaskRun(CNEOMemPoolWithLock *pMemPool,CNEOLog *pLog,CNEOTaskPool *pTaskPool);
	~CNEOTaskRun();
private:
	//����ص�����
	 static bool NEOTestRunTaskCallback(void *pCallParam,int &nStatus);
	CThreadManager m_ThreadManager;  
	//CNEOBaseLibrary *m_pNEOBaseLib;
	  //��־ģ��
    CNEOLog *m_pLog;
    //�ڴ��
    CNEOMemPoolWithLock *m_pMemPool;
    //�̳߳�
    CNEOTaskPool *m_pTaskPool;
public:
	void XGSysLog(const char *szFormat,...);
	//����һ������
	bool StartTask(_TASKPOOL_CALLBACK pCallback,void *pCallParam=NULL,char *szAppName=NULL);
	//����Info���������������
	bool StartTask(SNEOTaskRunInfo *pInfoStruct,char *szAppName=NULL);
	bool StartTask(CNEOTaskRunInfo *pInfoObject,char *szAppName=NULL);
	//ֹͣ��������
	void StopAll(void);
	bool IsRunning(void);
	int GetThreadCount(void);
	void PrintInfo(void);
};

}
#endif