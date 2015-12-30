#ifndef _NEOREFLECT_H_ 
#define _NEOREFLECT_H_

#include "singleton.h"
#include <iostream>
#include<map>
#include<string>
using namespace std;

namespace NEOLIB{

/*
    ʹ��˵����
        ����̳�NEODynBaseClass:
                ���У�
                    1����ʵ��CreateInstance��
                    2��ʹ�ú�DECLARE_CLASS��IMPLEMENT_CLASS�����ע��ĺ������๤����
                    3�����Զ���͵��ÿɲο���������
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

//��������������������˽�еģ�ֻ�ܱ��Լ�����
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
    /*ע�����Ժ�����m_PropertyMap*/
    virtual void RegistProperty() {}  
    virtual void Print() {}  
    /*ͨ��keyֵ������Ӧ����*/
    map<string, setValue> m_PropertyMap ;  
} ;  
  
IMPLEMENT_CLASS(NEODynBaseClass)  

/* ������Ӧ���Ե�get/set����*/
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