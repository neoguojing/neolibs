#ifndef _NEOCOMMAND_H_ 
#define _NEOCOMMAND_H_
#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    ʹ�÷�����
    Command���е��������̳и��࣬��ʵ��Excute����

    ����ģʽ��
    ��Ϊ������
    Command��Ҫִ�е�����ķ���
    Receiver����������߼�����ִ�и�����ĵײ�ʵ�֣�Command�ķ�����ͨ��������ʵ�֣�
    Invoker:������ߣ������û��Ľӿڣ�����ģ����ܽ��࣬ͨ��������ӦCommandȥ�����Ӧ�Ĺ��ܡ�
*/

class Command
{
public: 
    virtual ~Command(){}
    virtual void Excute() = 0;
protected: 
    Command(){}
private:
};

}//NEOLIB
#endif //_NEOCOMMAND_H_