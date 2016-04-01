#ifndef _NEOSTATE_H_
#define _NEOSTATE_H_

#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    state模式:
    1.用户需要继承StateContext，并扩展自己的接口；自定义接口，需要使用m_State方法去实现
    2.用户需要自定义state接口类，抽象出状态的共同特征（属性和行为）
    
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