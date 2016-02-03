#ifndef _NEODECORATOR_H_
#define _NEODECORATOR_H_

#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    使用方法：
    Component(齐天大圣)，最好是一个抽象类，描述一个实物的所有特征：由用户实现
    ConcreteComponent（大圣本尊），具体的实现Component：由用户实现
    Decorator（七十二变）装饰器的基类
    ConcreteDecorator（鱼）Decorator的实现类，将大圣装饰为鱼：由用户实现，需要继承Decorator并指定相应的基类
    （class ConcreteDecorator:public Decorator<Component>）
    具体使用：
    //创建大圣本尊
    Component* com = new ConcreteComponent();
    //以下相当于把大圣本尊装饰成为鱼儿
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