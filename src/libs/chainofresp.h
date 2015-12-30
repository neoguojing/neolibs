#ifndef _NEOCHAINOFRESP_H_ 
#define _NEOCHAINOFRESP_H_
#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    责任链模式：
    1、用户必须继承Handle，并仅需实现HandleRequest
    使用方法：
    Handle* h1 = new ConcreteHandle();
    Handle* h2 = new ConcreteHandle();
    h1->SetSuccessor(h2);
    h1->HandleRequest();
*/

class Handle 
{ 
public: 
    virtual ~Handle(){}
    virtual void HandleRequest() = 0;
    void SetSuccessor(Handle* succ)
    {
        m_pSuccessor = succ;
    }
    Handle* GetSuccessor()
    {
        return m_pSuccessor;
    }
protected: 
    Handle():m_pSuccessor(NULL){}
    Handle(Handle* succ):m_pSuccessor(succ)
    {}
private: 
    Handle* m_pSuccessor; 
};

////////////////////////////////test/////////////////////
class ConcreteHandle:public Handle 
{ 
public:
    ConcreteHandle(){}
    ~ConcreteHandle(){}
    ConcreteHandle(Handle* succ):Handle(succ)
    {}
    void HandleRequest()
    {
        if (this->GetSuccessor() != NULL) 
        { 
            cout<<"ConcreteHandle 我把处理权给后继节点....."<<endl;
            this->GetSuccessor()->HandleRequest();
        } 
        else 
        { 
            cout<<"ConcreteHandle 没有后继了，我必须自己处理...."<<endl; 
        }
    }
protected:

private:
};


}//NEOLIB
#endif //_NEOCHAINOFRESP_H_