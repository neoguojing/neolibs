#ifndef _NEOPROXY_H_
#define _NEOPROXY_H_

#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    代理模式使用：
    Subject：抽象模板类；用户定义的代码
    ConcreteSubject：Subject的实现；用户定义的代码
    AbstactProxy：工具类，需要用户继承该接口，并实现相应接口，最好显式调用该类的构造函数
    class ProxyTest:public AbstactProxy<Subject>
    使用方法：
    //新建主题类
    Subject *sub = new ConcreteSubject();
    //使用代理
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