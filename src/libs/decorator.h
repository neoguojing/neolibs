#ifndef _NEODECORATOR_H_
#define _NEODECORATOR_H_

#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    ʹ�÷�����
    Component(�����ʥ)�������һ�������࣬����һ��ʵ����������������û�ʵ��
    ConcreteComponent����ʥ���𣩣������ʵ��Component�����û�ʵ��
    Decorator����ʮ���䣩װ�����Ļ���
    ConcreteDecorator���㣩Decorator��ʵ���࣬����ʥװ��Ϊ�㣺���û�ʵ�֣���Ҫ�̳�Decorator��ָ����Ӧ�Ļ���
    ��class ConcreteDecorator:public Decorator<Component>��
    ����ʹ�ã�
    //������ʥ����
    Component* com = new ConcreteComponent();
    //�����൱�ڰѴ�ʥ����װ�γ�Ϊ���
    Component *dec1 = new ConcreteDecorator(com);
    dec1->Print();
    delete dec1;
*/

template <typename COMPONENT>
class Decorator:public COMPONENT 
{ 
public: 
    Decorator(COMPONENT* com):m_Com(com)
    {}
    virtual ~Decorator(){}
    virtual void Print() = 0;
protected: 
    COMPONENT* m_Com;
private: 

};

//////////////////////////test///////////////////////
class Component 
{ 
public: 
    virtual ~Component(){}
    virtual void Print() = 0;
protected: 
    Component(){}
private:
};

class ConcreteComponent:public Component 
{ 
public: 
    ConcreteComponent(){}
    ~ConcreteComponent(){}
    void Print()
    {
        cout<<"ConcreteComponent::Print"<<endl;
    }
protected:
private:
};


class ConcreteDecorator:public Decorator<Component>
{ 
public: 
    ConcreteDecorator(Component* com):Decorator<Component>(com)
    {}
    ~ConcreteDecorator(){}
    void Print()
    {
        m_Com->Print();
        this->AddedBehavior();
    }
    void AddedBehavior()
    {
        cout<<"ConcreteDecorator::AddedBehavior"<<endl;
    }
protected:

private:
};

}//NEOLIB
#endif //_NEODECORATOR_H_