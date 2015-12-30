#ifndef _NEOCOMPOSITE_H_
#define _NEOCOMPOSITE_H_

#include <iostream>
#include <vector>
using namespace std;
namespace NEOLIB{

/*
  ʹ�÷�����
  1.ʵ��һ��Component�࣬��Ϊ�����࣬һ��Ҫ����Add��remove��gechild���������û�ʵ��
  2.ʵ��һ��Leaf�࣬��ȫ�̳�Component��ʵ��Add��remove��gechild�����û�ʵ��
  3.ʹ��template <typename COMPONENT>
    class Composite:public COMPONENT ���������νṹ
    Ҳ���Լ̳и��࣬ʵ�ָ��ӷḻ�Ľӿ�
*/


template <typename COMPONENT>
class Composite:public COMPONENT 
{ 
public: 
    Composite(){}
    ~Composite(){}
public: 
    void Print()
    {
        cout<<"Composite::Print"<<endl;
    }

    void Add(COMPONENT* com)
    {
        m_comVec.push_back(com);
    }

    void Remove(COMPONENT* com)
    {
        m_comVec.erase(com);
    }

    COMPONENT* GetChild(int index)
    {
        return m_comVec[index];
    }
protected:
    vector<COMPONENT*> m_comVec;
private: 
    
};

/////////////////////////////test////////////////////////
class ComponentTest 
{ 
public: 
    ComponentTest(){}
    virtual ~ComponentTest(){}
public: 
    virtual void Print() = 0;
    virtual void Add(const ComponentTest& ){}
    virtual void Remove(const ComponentTest& ){}
    virtual ComponentTest* GetChild(int )
    {
        return 0;
    }

protected:
private:
};


class Leaf:public ComponentTest 
{ 
public: 
    Leaf(){}
    ~Leaf(){}
    void Print()
    {
        cout<<"Leaf::Print"<<endl;
    }

protected:
private:
};
}//NEOLIB
#endif //_NEOCOMPOSITE_H_