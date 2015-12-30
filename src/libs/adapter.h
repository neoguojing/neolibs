#ifndef _NEOADAPTER_H_
#define _NEOADAPTER_H_

#include <iostream>
using namespace std;
namespace NEOLIB{

/*
ʹ�÷�����
    1.һ��target�࣬�����û���Ҫʹ�õ����з��������û��Լ���д
    2.һ��adaptee�࣬����Ҫ������ࣻ���û�ָ�����߱�д
    3.adapter�࣬��дtarget���ĳЩ��������ʵ����ʹ��adaptee�ķ���ȥʵ�֣�
      ����̳���ClassAdapter ���� ObjectAdapter
      ע�⣺�̳���ObjectAdapter�����캯����Ҫ��ʾ����ObjectAdapter�Ĺ��캯��
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

//////////////////////����Ϊtest//////////////////////////////
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