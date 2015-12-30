#ifndef _NEOPROXY_H_
#define _NEOPROXY_H_

#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    ����ģʽʹ�ã�
    Subject������ģ���ࣻ�û�����Ĵ���
    ConcreteSubject��Subject��ʵ�֣��û�����Ĵ���
    AbstactProxy�������࣬��Ҫ�û��̳иýӿڣ���ʵ����Ӧ�ӿڣ������ʽ���ø���Ĺ��캯��
    class ProxyTest:public AbstactProxy<Subject>
    ʹ�÷�����
    //�½�������
    Subject *sub = new ConcreteSubject();
    //ʹ�ô���
    ProxyTest *proxy = new ProxyTest(sub);
    proxy->PreRequest();
    proxy->Request();
    proxy->PostRequest();
*/

template <typename SUBJECT>
class AbstactProxy:public SUBJECT
{ 
public: 
    AbstactProxy(){}
    AbstactProxy(SUBJECT* sub):m_Sub(sub)
    {}
    ~AbstactProxy(){}
    virtual void PreRequest() = 0;
    virtual void Request() = 0;
    virtual void PostRequest() = 0;
protected: 
    SUBJECT* m_Sub; 
private: 
    
};

///////////////////////////test//////////////////////////
class Subject 
{ 
public: 
    virtual ~Subject(){}
    virtual void Request() = 0;
protected: 
    Subject(){}
private:
};

class ConcreteSubject:public Subject 
{ 
public: 
    ConcreteSubject(){}
    ~ConcreteSubject(){}
    void Request()
    {
        cout<<"ConcreteSubject::Request"<<endl;
    }
protected:
private:
};

class ProxyTest:public AbstactProxy<Subject>
{ 
public: 
    ProxyTest(Subject *sub):AbstactProxy(sub)
    {}
    ~ProxyTest(){}
    void Request()
    {
        cout<<"ProxyTest::Request"<<endl;
        m_Sub->Request();
    }
    void PreRequest()
    {
        cout<<"ProxyTest::PreRequest"<<endl;
    }
    void PostRequest()
    {
        cout<<"ProxyTest::PostRequest"<<endl;
    }
protected:
private:
};

}//NEOLIB
#endif //_NEOPROXY_H_