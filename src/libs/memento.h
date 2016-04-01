#ifndef _NEOMEMENTO_H_
#define _NEOMEMENTO_H_

#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    AbtractMemento：接口类，为了屏蔽Memento的实现
    Memento：备份类，继承自AbtractMemento，仅需实现状态的get/set操作以及Print
    Originator：用户类，需要实现创建/恢复备份，状态的get/set;
    Caretaker：备份管理类，仅需实现Memento的get/set的操作;用户只需继承该类

    注：
    本接口对于简单的应用，在使用Memento类的时候，用户可以简单继承Originator快速实现相关
    功能；否则，用户需要重新设计自己的Memento和Originator
    实例：
    //创建功能对象和备份对象
    Originator<string> *o = new Originator<string>();
    Caretaker *caretaker = new Caretaker();
    //改变状态
    o->SetState("on");
    //备份功能对象的状态
    caretaker->SetMemento(o->CreateBackups());
    o->Print();
    caretaker->GetMemento()->Print();
    //功能类状态改变
    o->SetState("off");
    o->Print();
    //恢复功能类状态
    o->RestoreBackups(caretaker->GetMemento());
    o->Print();
    caretaker->GetMemento()->Print();
*/
class AbtractMemento
{
public:
    virtual void Print() = 0;
};

template <typename STATE>
class Memento:public AbtractMemento
{
public:
    //构造的时候备份数据
    Memento(STATE state):m_State(state){};

    virtual STATE GetState()
    {
        return m_State;
    }

    virtual void SetState(STATE state)
    {
        m_State = state;
    }

    virtual void Print()
    {
        cout<<"Memento::Print m_State="<<m_State<<endl;
    }
protected:
    STATE m_State;
};

//发起者类备份自己的状态
template <typename STATE>
class Originator
{
public:
    //建立备份
    AbtractMemento* CreateBackups()
    {
        return new Memento<STATE>(m_State);
    }
    //恢复备份内容
    void RestoreBackups(AbtractMemento* pMemento)
    {
        m_State = dynamic_cast<Memento<STATE>*>(pMemento)->GetState();
    }

    STATE GetState()
    {
        return m_State;
    }

    void SetState(STATE state)
    {
        m_State = state;
    }

    //显示状态
    void Print() 
    {
        cout<<"Originator::Print m_State="<<m_State<<endl;
    }
protected:
    STATE m_State;
};

//管理者类管理备份
class Caretaker
{
public:
    //得到备份
    AbtractMemento* GetMemento()
    {
        return m_Memento;
    }
    //设置备份
    void SetMemento(AbtractMemento* pMemento)
    {
        m_Memento = pMemento;
    }
protected:
    AbtractMemento *m_Memento;
};

}//NEOLIB
#endif //_NEOMEMENTO_H_