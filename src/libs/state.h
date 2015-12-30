#ifndef _NEOSTATE_H_
#define _NEOSTATE_H_

#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    stateģʽ:
    1.�û���Ҫ�̳�StateContext������չ�Լ��Ľӿڣ��Զ���ӿڣ���Ҫʹ��m_State����ȥʵ��
    2.�û���Ҫ�Զ���state�ӿ��࣬�����״̬�Ĺ�ͬ���������Ժ���Ϊ��
    
*/
template <typename STATE>
class StateContext
{
public:
    ~StateContext(){}
    virtual void SetState(STATE *pState)
    {
        m_State = pState;
    }
    
    virtual STATE* GetState(STATE *pState)
    {
        return m_State;
    }
    
    virtual void Print()
    {
        cout<<"StateContext::Print"<<endl;
    }
protected:
    StateContext():m_State(NULL)
    {}
    STATE *m_State;

};

/////////////////////////test///////////////////////
class StateTest
{
public:
    ~StateTest(){}
    virtual void Print()
    {
        cout<<"StateTest::Print"<<endl;
    }
protected:
    StateTest(){}

};

class StateTestA:public StateTest
{
public:
    ~StateTestA(){}
    virtual void Print()
    {
        cout<<"StateTestA::Print"<<endl;
    }
    StateTestA(){}

};

class ContextTest:public StateContext<StateTest>
{
public:
    ~ContextTest(){}
    virtual void Print()
    {
        cout<<"ContextTest::Print"<<endl;
        m_State->Print();
    }
    ContextTest(){}

};

}//NEOLIB
#endif //_NEOSTATE_H_