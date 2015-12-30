#ifndef _NEOSINGLETON_H_ 
#define _NEOSINGLETON_H_

#include <iostream>
#include "neolock.h"

using namespace std;

namespace NEOLIB{
namespace
{
	CMutexLock s_Lock;
}
/*
	ʹ�÷�����
	1.Ҫ���û���Ĺ��캯��Ϊprotected���ҹ��캯���Ĳ���ֻ��Ϊһ��ָ�룬�ڲ����¶�ָ����н���
	2.��Ҫ���ʵĳ�Ա���Ϊprotected��public
		SingletonWithLock<CNEOBaseLibrary>::getInstance()
	3.ʵ���������ӿڣ�һ�����в����Ľӿ����ڴ��в����ĵ������󣬲�����ָ�����ʽ����

	ʹ�þ�����
		SingletonWithLock<CNEOBaseLibrary>::getInstance();


*/
template<class USERCLASS>
class Singleton:public USERCLASS
{
public: 
	static Singleton* getInstance();
	static Singleton* getInstance(void *args);

protected:
	Singleton();
	Singleton(void *args);
private: 
	static Singleton* m_Instance;

};

template<class USERCLASS>
Singleton<USERCLASS>* Singleton<USERCLASS>::m_Instance = 0;

template<class USERCLASS>
Singleton<USERCLASS>::Singleton()
{ 
	cout<<"Singleton...."<<endl;
}

template<class USERCLASS>
Singleton<USERCLASS>::Singleton(void *args):USERCLASS(args)
{ 
	cout<<"Singleton with args...."<<endl;
}

template<class USERCLASS>
Singleton<USERCLASS>* Singleton<USERCLASS>::getInstance() 
{ 
	if (m_Instance == 0) 
	{ 
		m_Instance = new Singleton(); 
	}
	return m_Instance; 
}

template<class USERCLASS>
Singleton<USERCLASS>* Singleton<USERCLASS>::getInstance(void *args) 
{ 
	if (m_Instance == 0) 
	{ 
		m_Instance = new Singleton(args); 
	}
	return m_Instance; 
}

template<class USERCLASS>
class SingletonWithLock:public USERCLASS
{
public: 
	static SingletonWithLock* getInstance();
	static SingletonWithLock* getInstance(void *args);

protected:
	SingletonWithLock();
	SingletonWithLock(void *args);
private: 
	static SingletonWithLock* m_Instance;

};

template<class USERCLASS>
SingletonWithLock<USERCLASS>* SingletonWithLock<USERCLASS>::m_Instance = 0;

template<class USERCLASS>
SingletonWithLock<USERCLASS>::SingletonWithLock() 
{ 
	cout<<"SingletonWithLock...."<<endl;
}

template<class USERCLASS>
SingletonWithLock<USERCLASS>::SingletonWithLock(void *args):USERCLASS(args)
{ 
	cout<<"SingletonWithLock with args...."<<endl;
}

template<class USERCLASS>
SingletonWithLock<USERCLASS>* SingletonWithLock<USERCLASS>::getInstance() 
{ 
	if (m_Instance == 0)
	{
		NEOLIB::s_Lock.Lock();
		{
			if (m_Instance == 0) 
			{ 
				m_Instance = new SingletonWithLock(); 
			}		
		}
		NEOLIB::s_Lock.UnLock();
	}
	return m_Instance; 
}


template<class USERCLASS>
SingletonWithLock<USERCLASS>* SingletonWithLock<USERCLASS>::getInstance(void *args) 
{ 
	if (m_Instance == 0)
	{
		NEOLIB::s_Lock.Lock();
		{
			if (m_Instance == 0) 
			{ 
				m_Instance = new SingletonWithLock(args); 
			}		
		}
		NEOLIB::s_Lock.UnLock();
	}
	return m_Instance; 
}

}

#endif