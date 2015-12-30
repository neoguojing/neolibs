#ifndef _NEOBUILDER_H_
#define _NEOBUILDER_H_

namespace NEOLIB{


/*
	������ģʽʹ�ã�
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
	��Ʒ�Ļ���
*/
public:
	~Product(){}

	/*��ӡ��Ʒ���ڲ��ṹ*/
	virtual void show() = 0;

private:
	Product(){}

};

class AbstractBuilder{
	/*
		�����߻���
		����ʵ�֣�
		1.ʵ����Ӧ���麯����
		2.ʵ�ֽ��캯��
	*/
public:
	virtual ~AbstractBuilder(){}

	/*�����ղ�Ʒ����*/
	virtual void createProduct() = 0;
	/*
	����������ƾ���Ľ��캯��
	��Щ��������m_product�ķ���������Ʒ
	virtual void buildPartA(int param) = 0;
	virtual void buildPartB(int param) = 0;
	virtual void buildPartC(int param) = 0;
	*/

	/*��ȡ����֮��Ĳ�Ʒ����*/
	virtual Product* getProduct() = 0;

protected:
	AbstractBuilder():m_product(0)
	{}
	Product* m_product;

};


class AbstractDirector
{
	/*
		�����߻���
		1��������Ҫʵ��construct������ʵ�֣����ε���m_Builder�ķ���������Ʒ
		2��������Ҫ��ʾ�ĵ���AbstractDirector�Ĺ��캯����Ҳ���Ǳ��봫��AbstractBuilderָ��
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