#ifndef _NEOMEDIATOR_H_ 
#define _NEOMEDIATOR_H_
#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    使用：
    Mediator类是调停者的基类，用户继承该类，所有Colleage对象的交互在该类中实现；
        该类持有Colleage的引用
    Colleage类也叫同事类，消息交互的主题，用户继承该类实现相应接口
        该类使用m_Mdt，完成相关函数的实现
*/
class Mediator;

template <typename STATE>
class Colleage { 
public:
    virtual ~Colleage(){}
    virtual void NotifyAll() = 0;
    virtual void Print() = 0;
    virtual void SetState(const STATE& sdt) = 0;
    virtual string GetState() = 0;
protected: 
    Colleage(){}
    Colleage(Mediator* mdt):m_Mdt(mdt)
    {}
    Mediator* m_Mdt;
private:
};

class Mediator 
{ 
public: 
    virtual ~Mediator(){}
    virtual void Print() = 0;
protected: 
    Mediator(){}
private:

};

}//NEOLIB
#endif //_NEOMEDIATOR_H_