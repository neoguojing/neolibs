#ifndef _NEOREFLECT_H_ 
#define _NEOREFLECT_H_

#include "singleton.h"
#include <iostream>
#include<map>
#include<string>
using namespace std;

namespace NEOLIB{

/*
    使用说明：
        仅需继承NEODynBaseClass:
                其中：
                    1、需实现CreateInstance；
                    2、使用宏DECLARE_CLASS和IMPLEMENT_CLASS定义和注册改函数到类工厂；
                    3、属性定义和调用可参考测试用例
*/
class NEODynBaseClass;
typedef void* (*createClass)(void);

class NEOClassFactory  
{  
public:     
    virtual ~NEOClassFactory() ;  
   
    createClass GetClassByName(string className) ;  
   
    void RegistClass(string name, createClass method) ;  
   
    NEOClassFactory& GetClassFactory() ;  
   
protected: 
    NEOClassFactory() ;  
    map<string, createClass> m_ClassMap ;  
} ;  

NEOClassFactory::NEOClassFactory()
{

}

NEOClassFactory::~NEOClassFactory()
{

}

createClass NEOClassFactory::GetClassByName(string className)  
{  
    map<string, createClass>::const_iterator iter ;  
   
    iter = m_ClassMap.find(className) ;  
    if ( iter == m_ClassMap.end() )  
        return NULL ;  
    else  
        return iter->second;  
}  
   
void NEOClassFactory::RegistClass(string name, createClass method)  
{  
    m_ClassMap.insert(pair<string, createClass>(name, method)) ;  
}  
   
NEOClassFactory& NEOClassFactory::GetClassFactory()  
{  
    static NEOClassFactory _sharedClassFactory ;  
    return _sharedClassFactory ;  
}  

typedef SingletonWithLock<NEOClassFactory> ClassFactory;

class NEODynamicClass  
{  
public:  
    NEODynamicClass(string name, createClass method)  
    {  
        ClassFactory::getInstance()->RegistClass(name, method) ;  
    }  
} ;  

//以下声明的两个变量是私有的，只能被自己访问
#define DECLARE_CLASS(className)\
    string className##Name ;    \
    static NEODynamicClass* m_##className##NEO ;  
   
#define IMPLEMENT_CLASS(className)\
    NEODynamicClass* className::m_##className##NEO = new NEODynamicClass(#className, (createClass)className::CreateInstance()) ; 

typedef void (*setValue)(NEODynBaseClass *t, void* c) ;  
   
class NEODynBaseClass  
{  
private:  
    DECLARE_CLASS(NEODynBaseClass)  
   
public:  
    NEODynBaseClass() {}  
    virtual ~NEODynBaseClass() {}  
    static void* CreateInstance() {return new NEODynBaseClass();}  
    /*注册属性函数到m_PropertyMap*/
    virtual void RegistProperty() {}  
    virtual void Print() {}  
    /*通过key值调用相应函数*/
    map<string, setValue> m_PropertyMap ;  
} ;  
  
IMPLEMENT_CLASS(NEODynBaseClass)  

/* 生成相应属性的get/set函数*/
#define SYNTHESIZE(classType, varType, varName)                    \
public:                                                            \
    inline static void set##varName(NEODynBaseClass*cp, void*value)\
    {                                                              \
        classType* tp = (classType*)cp ;                           \
        tp->varName = (varType)value ;                             \
    }                                                              \
    inline varType get##varName(void) const                        \
    {                                                              \
        return varName ;                                           \
    }





//////////////////////////////test///////////////////////////////////////
class CKHelloClass:public NEODynBaseClass
{
public:  
    SYNTHESIZE(CKHelloClass, int*, m_pValue)  
    
    CKHelloClass() {}  
    virtual ~CKHelloClass(){}  
    static void* CreateInstance()  
    {  
        return new CKHelloClass() ;  
    }  
    virtual void RegistProperty()  
    {  
        m_PropertyMap.insert(pair<string, setValue>("setm_pValue", setm_pValue)) ;  
    }  
    virtual void Print()  
    {  
        cout << *getm_pValue() << endl ;  
    }  
public:  
    DECLARE_CLASS(CKHelloClass)  
    int *m_pValue ;  
   
};  
IMPLEMENT_CLASS(CKHelloClass)

}
#endif//_NEOREFLECT_H_