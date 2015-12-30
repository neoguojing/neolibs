#ifndef _NEOCOMPOSITE_H_
#define _NEOCOMPOSITE_H_

#include <iostream>
#include <vector>
using namespace std;
namespace NEOLIB{

/*
  使用方法：
  1.实现一个Component类，作为抽象类，一般要包含Add，remove和gechild方法；由用户实现
  2.实现一个Leaf类，完全继承Component不实现Add，remove和gechild；由用户实现
  3.使用template <typename COMPONENT>
    class Composite:public COMPONENT ，构建树形结构
    也可以继承该类，实现更加丰富的接口
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