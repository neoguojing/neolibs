#ifndef _NEOOBSERVER_H_
#define _NEOOBSERVER_H_

#include <iostream>
#include <set> 
#include <string> 
using namespace std;

namespace NEOLIB{

/*
    Observer:
    1.用户继承该类，并实现Update；
    2.子类的构造函数中需要调用Observable或其子类的Attach进行注册
    Observable:
    用户继承该类，自由实现自身逻辑；
    重要的是调用SetChange 和 Notify通知观察者自身的状态变化



*/
class Observable;

class Observer 
{ 
public: 
    virtual ~Observer(){}
    virtual void Update(Observable* sub, void *args) = 0;
    virtual void Print()
    {
        cout<<"Observer::Print"<<endl;
    }
protected: 
    Observer(){}
private:
};

class Observable
{ 
public: 
    virtual ~Observable()
    {
        if(NULL!=m_Bbvs)
            delete m_Bbvs;
    }
    virtual void Attach(Observer *obv)
    {
        if (NULL!=m_Bbvs)
            m_Bbvs->insert(obv);
    }

    virtual void Detach(Observer *obv)
    {
        if (NULL!=m_Bbvs)
            m_Bbvs->erase(obv);
    }

    virtual void Notify()
    {
        Notify(NULL);
    }

    virtual void Notify(void *args)
    {
        if (!HasChanged())
            return;
        set<Observer*>::iterator it;
        for(it=m_Bbvs->begin();it!=m_Bbvs->end();++it)
        {
            (*it)->Update(this, args);
        }
    }

    virtual int Count()
    {
        return m_Bbvs->size();
    }

    virtual void ClearAll()
    {
        m_Bbvs->clear();
    }

    /*子类覆盖该方法，设置自身数据，并调用Notify，通知观察者*/
    virtual void SetChange()
    {
        m_ChangeFlag =true;
    }

    virtual void ClearChange()
    {
        m_ChangeFlag =false;
    }

    virtual bool HasChanged()
    {
        return m_ChangeFlag;
    }

protected: 
    Observable():m_ChangeFlag(false)
    {
        m_Bbvs = new set<Observer*>;
    }
private: 
    set<Observer*> *m_Bbvs;
    /*状态变化标记*/
    bool m_ChangeFlag;
};

////////////////////////////test//////////////////////////////
class Watched:public Observable
{
public:
    Watched():m_data()
    {
    }

    ~Watched(){}
    
    virtual void SetData(const string& data)
    {
        if (m_data!=data)
        {
            m_data = data;
            this->SetChange();
            this->Notify();
        }
    }

    virtual string GetData()
    {
        return m_data;
    }
private:
    string m_data;
};

class Watcher:public Observer
{
public:
    Watcher(Observable *wd)
    {
        wd->Attach(this);
    }

    ~Watcher(){}

    void Update(Observable *sub, void *args)
    {
        cout<<"Watcher::Update"<<endl<<dynamic_cast<Watched*>(sub)->GetData()<<endl;
    }

    void Print()
    {
        cout<<"Watcher::Print"<<endl;
    }
};

class Watcher1:public Observer
{
public:
    Watcher1(Observable *wd)
    {
        wd->Attach(this);
    }

    ~Watcher1(){}

    void Update(Observable *sub, void *args)
    {
        cout<<"Watcher1::Update"<<endl<<dynamic_cast<Watched*>(sub)->GetData()<<endl;
    }

    void Print()
    {
        cout<<"Watcher1::Print"<<endl;
    }
};
}//NEOLIB
#endif //_NEOOBSERVER_H_