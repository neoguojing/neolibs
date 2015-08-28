#ifndef NEOLOCK

#define NEOLOCK
#include "neoindex.h"

////�������ض���
//#ifdef WIN32_NEO
//#define MUTEX CRITICAL_SECTION
//#define MUTEXINIT(m) InitializeCriticalSection(m)
//#define MUTEXLOCK(m) EnterCriticalSection(m)
//#define MUTEXUNLOCK(m) LeaveCriticalSection(m)
//#define MUTEXDESTROY(m) DeleteCriticalSection(m)
//
//#else
//#define MUTEX pthread_mutex_t
//#define MUTEXINIT(m) pthread_mutex_init(m,NULL)
//#define MUTEXLOCK(m) pthread_mutex_lock(m)
//#define MUTEXUNLOCK(m) pthread_mutex_unlock(m)
//#define MUTEXDESTROY(m) pthread_mutex_destroy(m)
//
//#endif
//c���Եļ����ṹ��
typedef struct _MINT_
{
	int m_nValue;
	MUTEX m_MyLock;
}MINT,MBOOL;
//�����κ�bool�����ݵļ���
//��ʼ����һ���̰߳�ȫ������ͬʱ���Է�������ֵ
 int MvarInit(MINT &(mValue),int nValue=0);
 void MvarDestroy(MINT &(mValue));
//����һ���̱߳���ֵ
 int MvarSet(MINT &(mValue),int nValue);
//�õ��̰߳�ȫ������ֵ
 int MvarGet(MINT &(mValue));
//�̰߳�ȫ�������ӷ��ͽⷨ
 int MvarAdd(MINT &(mValue),int nValue=1);
 int MvarDec(MINT &(mValue),int nValue=1);

//��д�����
typedef struct _NEO_MULTI_READ_SINGLE_WRITE_LOCK_
{
	int m_nReadCount;                              //��������
	bool m_bWriteFlag;                             //д���
	MUTEX m_Lock;                                  //�ڲ�������֤���̰߳�ȫ
}SNeoMultiReadSingleWriteLock;
const unsigned long  SNeoMultiReadSingleWriteLockSize=sizeof(SNeoMultiReadSingleWriteLock);

 void MRSWLock_Create(SNeoMultiReadSingleWriteLock *pLock);
 void MRSWLock_Destroy(SNeoMultiReadSingleWriteLock *pLock);
//��״̬��֪
 bool MRSWLock_GetWrite(SNeoMultiReadSingleWriteLock *pLock);
 int MRSWLock_GetRead(SNeoMultiReadSingleWriteLock *pLock);
//����д����
 void MRSWLock_EnterWrite(SNeoMultiReadSingleWriteLock *pLock);
//�˳�д
 void MRSWLock_DisableWrite(SNeoMultiReadSingleWriteLock *pLock);
//�����
 int MRSWLock_AddRead(SNeoMultiReadSingleWriteLock *pLock);
//�˳���
 int MRSWLock_DecRead(SNeoMultiReadSingleWriteLock *pLock);
//��תд
 void MRSWLock_ReadToWrite(SNeoMultiReadSingleWriteLock *pLock);
/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//����������
class  CMutexLock
{
public:
	CMutexLock(void);
	~CMutexLock(void);

public:
	void Lock();
	void UnLock();
private:
	MUTEX m_Lock;
};
//���Ϻ�����c++��ʵ��
class  CMint
{
public:
	CMint(int nValue=0);
	~CMint(void);
public:
	int Get(void);
	int Set(int nValue);
	int Add(int nValue=1);
	int Dec(int nValue=1);
public:
	MINT m_nValue;
};
//��bool�ͱ����ļ���
class  CMbool
{
public:
	CMbool(int nValue=0);
	~CMbool(void);
public:
	int Get(void);
	int Set(int nValue);
public:
	MBOOL m_nValue;
};
//���Ϻ�����Ӧ��c++��װ
class  CMultiReadSingleWriteLock
{
public:
	CMultiReadSingleWriteLock();
	~CMultiReadSingleWriteLock();
public:
	void EnableWrite(void);
	void DisableWrite(void);
	void ReadToWrite(void);
	void DecRead(void);
	void AddRead(void);
	bool GetWrite(void);
	int GetRead();
private:
	SNeoMultiReadSingleWriteLock m_Lock;
};
//�����ϵ����������κ�bool�ͱ�������
class  CMRSWint
{
public:
	CMRSWint();
	~CMRSWint();
public:
	int Get(void);
	int Set(int nValue);
	int Add(int nValue=1);
	int Dec(int nValue=1);
private:
	int m_nValue;
	CMultiReadSingleWriteLock m_Lock;
};
class  CMRSWbool
{
public:
	CMRSWbool();
	~CMRSWbool();
public:
	int Get(void);
	int Set(bool bFlag);
private:
	CMRSWint m_nValue;
};
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//����������
class  CNonReentrant
{
public:
	CNonReentrant();
public:
	bool Set(bool bRunFlag);
private:
	CMutexLock m_Lock;             //flag
	bool m_bAlreadyRunFlag;        //�ڲ��ı���ֵ
};
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//�߳̿�����
class  CThreadManager
{
private:
	CMRSWbool m_bThreadContinue;//�̳߳����ı�־
	CMRSWint m_nThreadCount;    //�̼߳�����
	CMRSWint m_nThreadID;       //��Ƕ��debug�Ѻù���
public:
	CThreadManager();
	~CThreadManager();
	void Open(void);
	void CloseAll(void);
	int AddThread(void);
	void DecAThread(void);
	//��ѯ�߳�ά�ֱ�����ֵ
	bool ThreadContinue(void);
	//��ü�������ֵ
	int GetThreadCount(void);
	//����һ���߳�ID��debug��
	int GetID(void);
};


#endif