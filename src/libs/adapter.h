#ifndef _NEOADAPTER_H_
#define _NEOADAPTER_H_

#include <iostream>
using namespace std;
namespace NEOLIB{

/*
使用方法：
    1.一个target类，包含用户需要使用的所有方法；由用户自己编写
    2.一个adaptee类，是需要适配的类；由用户指定或者编写
    3.adapter类，改写target类的某些方法，在实现中使用adaptee的方法去实现；
      必须继承自ClassAdapter 或者 ObjectAdapter
      注意：继承自ObjectAdapter，构造函数需要显示调用ObjectAdapter的构造函数
	classadapter
    AdapterTargetTest* adt = new AdpterTest1();
    adt->request1();
    adt->request2();

    objectadapter
    AdapteeTest* ade = new AdapteeTest;
    AdapterTargetTest* adt = new AdpterTest2(ade);
    adt->request1();
    adt->request2();
*/

template <typename TARGET,typename ADAPTEE>
class ClassAdapter:public TARGET,protected ADAPTEE 
{ 
public:
    ClassAdapter(){}
    ~ClassAdapter(){}
    virtual void Print()
    {
        cout<<"ClassAdapter"<<endl;
    }
protected:

private:
};

template <typename TARGET,typename ADAPTEE>
class ObjectAdapter:public TARGET 
{ 
public: 
    ObjectAdapter(ADAPTEE* ade):m_ade(ade)
    {}
    ~ObjectAdapter(){}
    virtual void Print()
    {
        cout<<"ObjectAdapter"<<endl;
    }
protected:
    ADAPTEE* m_ade;
private: 
    
};

//////////////////////以下为test//////////////////////////////
class AdapterTargetTest 
{ 
public: 
    AdapterTargetTest()
    {}
    virtual ~AdapterTargetTest()
    {}

    virtual void request1()
    {
        cout<<"AdapterTargetTest::request1"<<endl;
    }
    virtual void request2()
    {
        cout<<"AdapterTargetTest::request2"<<endl;
    }
protected:
private:
};


class AdapteeTest
{ 
public: 
    AdapteeTest(){}
    ~AdapteeTest(){}
    void SpecificRequest()
    {
        cout<<"AdapteeTest::SpecificRequest"<<endl;
    }
protected:

private:
};

class AdpterTest1:public ClassAdapter<AdapterTargetTest,AdapteeTest>
{
public: 
    AdpterTest1()
    {}
    ~AdpterTest1()
    {}
    void request1()
    {
        this->SpecificRequest();
    }
};

class AdpterTest2:public ObjectAdapter<AdapterTargetTest,AdapteeTest>
{
public: 
    AdpterTest2(AdapteeTest *ade):ObjectAdapter<AdapterTargetTest,AdapteeTest>(ade)
    {}
    ~AdpterTest2()
    {}
    void request1()
    {
        m_ade->SpecificRequest();
    }
};

}//NEOLIB
#endif //_NEOBRIDGE_H_