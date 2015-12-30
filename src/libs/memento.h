#ifndef _NEOMEMENTO_H_
#define _NEOMEMENTO_H_

#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    AbtractMemento���ӿ��࣬Ϊ������Memento��ʵ��
    Memento�������࣬�̳���AbtractMemento������ʵ��״̬��get/set�����Լ�Print
    Originator���û��࣬��Ҫʵ�ִ���/�ָ����ݣ�״̬��get/set;
    Caretaker�����ݹ����࣬����ʵ��Memento��get/set�Ĳ���;�û�ֻ��̳и���

    ע��
    ���ӿڶ��ڼ򵥵�Ӧ�ã���ʹ��Memento���ʱ���û����Լ򵥼̳�Originator����ʵ�����
    ���ܣ������û���Ҫ��������Լ���Memento��Originator
    ʵ����
    //�������ܶ���ͱ��ݶ���
    Originator<string> *o = new Originator<string>();
    Caretaker *caretaker = new Caretaker();
    //�ı�״̬
    o->SetState("on");
    //���ݹ��ܶ����״̬
    caretaker->SetMemento(o->CreateBackups());
    o->Print();
    caretaker->GetMemento()->Print();
    //������״̬�ı�
    o->SetState("off");
    o->Print();
    //�ָ�������״̬
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
    //�����ʱ�򱸷�����
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

//�������౸���Լ���״̬
template <typename STATE>
class Originator
{
public:
    //��������
    AbtractMemento* CreateBackups()
    {
        return new Memento<STATE>(m_State);
    }
    //�ָ���������
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

    //��ʾ״̬
    void Print() 
    {
        cout<<"Originator::Print m_State="<<m_State<<endl;
    }
protected:
    STATE m_State;
};

//�������������
class Caretaker
{
public:
    //�õ�����
    AbtractMemento* GetMemento()
    {
        return m_Memento;
    }
    //���ñ���
    void SetMemento(AbtractMemento* pMemento)
    {
        m_Memento = pMemento;
    }
protected:
    AbtractMemento *m_Memento;
};

}//NEOLIB
#endif //_NEOMEMENTO_H_