#ifndef _NEOMEDIATOR_H_ 
#define _NEOMEDIATOR_H_
#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    ʹ�ã�
    Mediator���ǵ�ͣ�ߵĻ��࣬�û��̳и��࣬����Colleage����Ľ����ڸ�����ʵ�֣�
        �������Colleage������
    Colleage��Ҳ��ͬ���࣬��Ϣ���������⣬�û��̳и���ʵ����Ӧ�ӿ�
        ����ʹ��m_Mdt�������غ�����ʵ��
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