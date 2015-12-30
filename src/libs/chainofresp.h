#ifndef _NEOCHAINOFRESP_H_ 
#define _NEOCHAINOFRESP_H_
#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    ������ģʽ��
    1���û�����̳�Handle��������ʵ��HandleRequest
    ʹ�÷�����
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
            cout<<"ConcreteHandle �ҰѴ���Ȩ����̽ڵ�....."<<endl;
            this->GetSuccessor()->HandleRequest();
        } 
        else 
        { 
            cout<<"ConcreteHandle û�к���ˣ��ұ����Լ�����...."<<endl; 
        }
    }
protected:

private:
};


}//NEOLIB
#endif //_NEOCHAINOFRESP_H_