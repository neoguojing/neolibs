#ifndef _NEOBRIDGE_H_
#define _NEOBRIDGE_H_

#include <iostream>
using namespace std;
namespace NEOLIB{

/*
	
	AbstractBridge是抽象模型的基类，聚合了AbstractBridgeImp
	AbstractBridgeImp是实现模型的基类

	RefinedAbstractions是重新定义的抽象类，此处只是一个举例，具体由用户实现，只需继承AbstractBridge，
	并显示调用基类的构造函数
	ConcreteAbstractionImpA是重新定义的实现类，此处只是一个举例，具体由用户实现
	bridge模式使用：
	int main(int argc,char* argv[]) 
	{ 
		AbstractBridgeImp* imp = new ConcreteAbstractionImpA();
		AbstractBridge* abs = new RefinedAbstraction(imp);
		abs->Print(); 
		return 0; 
	}
􀂋 
*/
class AbstractBridgeImp;

class AbstractBridge
{ 
public: 
	virtual ~AbstractBridge(){}
	virtual void Print() = 0;
protected: 
	AbstractBridge(AbstractBridgeImp* pImp):m_Imp(pImp)
	{}
	AbstractBridgeImp* m_Imp;
private:

};

class RefinedAbstraction:public AbstractBridge 
{
public: 
	RefinedAbstraction(AbstractBridgeImp* pImp):AbstractBridge(pImp)
	{}
	~RefinedAbstraction(){}
    void Print()
	{
		cout<<"RefinedAbstraction"<<endl;
	}
protected:

};

class AbstractBridgeImp 
{ 
public:
	virtual ~AbstractBridgeImp(){}
	virtual void Print() = 0;
protected: 
	AbstractBridgeImp(){}
	
private:
};

class ConcreteAbstractionImpA:public AbstractBridgeImp
{
public: 
	ConcreteAbstractionImpA(){}
	~ConcreteAbstractionImpA(){}

	virtual void Print()
	{
		cout<<"ConcreteAbstractionImpA"<<endl;
	}
	
protected: 

private:
};

}
#endif //_NEOBRIDGE_H_