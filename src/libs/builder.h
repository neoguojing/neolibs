#ifndef _NEOBUILDER_H_
#define _NEOBUILDER_H_

namespace NEOLIB{


/*
	建造者模式使用：
int main()
{
	AbstractBuilder* builder = new Builder();
	AbstractDirector* director = new Director(builder);

	director->construct();
	
	Product* product = builder->getProduct();
	product->show();
	return 0;
}
*/
class Product{
/*
	产品的基类
*/
public:
	~Product(){}

	/*打印产品的内部结构*/
	virtual void show() = 0;

private:
	Product(){}

};

class AbstractBuilder{
	/*
		建造者基类
		子类实现：
		1.实现相应的虚函数；
		2.实现建造函数
	*/
public:
	virtual ~AbstractBuilder(){}

	/*创建空产品对象*/
	virtual void createProduct() = 0;
	/*
	在子类中设计具体的建造函数
	这些函数调用m_product的方法创建产品
	virtual void buildPartA(int param) = 0;
	virtual void buildPartB(int param) = 0;
	virtual void buildPartC(int param) = 0;
	*/

	/*获取建造之后的产品对象*/
	virtual Product* getProduct() = 0;

protected:
	AbstractBuilder():m_product(0)
	{}
	Product* m_product;

};


class AbstractDirector
{
	/*
		建造者基类
		1、子类需要实现construct函数，实现：依次调用m_Builder的方法构建产品
		2、子类需要显示的调用AbstractDirector的构造函数，也就是必须传入AbstractBuilder指针
	*/
public:
	~AbstractDirector(){}

	virtual void construct() = 0;

protected:
	AbstractDirector(AbstractBuilder* builder):
	  m_Builder(builder)
	{}
	AbstractBuilder* m_Builder;
};


}
#endif