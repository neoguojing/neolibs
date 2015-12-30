#ifndef _NEOCOMMAND_H_ 
#define _NEOCOMMAND_H_
#include <iostream>
using namespace std;
namespace NEOLIB{

/*
    使用方法：
    Command所有的命令均需继承该类，并实现Excute函数

    命令模式：
    分为三部分
    Command：要执行的命令的分类
    Receiver：命令接收者即具体执行该命令的底层实现，Command的方法均通过该类来实现；
    Invoker:命令触发者，面向用户的接口，功能模块的总结类，通过调用相应Command去完成相应的功能。
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