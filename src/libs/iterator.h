#ifndef _NEOITERATOR_H_ 
#define _NEOITERATOR_H_

/*

    迭代器模式：
    1.容器类：
    包含用户需要操作的数据，并提供next，prev和begin，end等函数，以辅助迭代子类实现步进操作；用户需要继承NEOAggregate类，以提供
    迭代子的类型定义；
    参照：class GlobalList
    2.迭代子类：实现++，--，*，->等操作，必须针对容器类要操作的data进行设计；在创建迭代子类的时候，需要使用容器类为迭代子附初值；
    用户需要继承NEOIteratorBase，使得与STL兼容；参照:class GlobalIterator
    提供够着函数，使得能够使用容器类为迭代子附初值，是比较好的实现：
    GlobalIterator(GlobalList<I>* pList)
    {
        iterator_m = pList;
    }
    迭代器型别
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    struct iterator {
        typedef _Category  iterator_category;
        typedef _Tp        value_type;
        typedef _Distance  difference_type;
        typedef _Pointer   pointer;
        typedef _Reference reference;
    };

*/
namespace NEOLIB{

template <class _Tp, class _Category = bidirectional_iterator_tag, class _Distance = ptrdiff_t,class _Pointer = _Tp*, class _Reference = _Tp&>
class NEOIteratorBase
{
public:
    typedef _Category  iterator_category;
    typedef _Tp        value_type;
    typedef _Distance  difference_type;
    typedef _Pointer   pointer;
    typedef _Reference reference;

    typedef NEOIteratorBase<_Tp, _Category> _Self;
    /*
    virtual _Self& operator++() = 0;
    virtual _Self& operator--() = 0;
    virtual _Self operator++(int) = 0;
    virtual _Self operator--(int) = 0;
    virtual reference operator*() = 0;
    virtual pointer operator->() = 0;*/
};

/*ITERATOR 必须是最终的迭代器本身*/
template <class ITERATOR>
class NEOAggregate
{
public:
    typedef ITERATOR  iterator;
    
};

/////////////////////////test/////////////////////
template <typename _Tp>
class NEOListIterator;

class NEOListNodeBase
{
public:
    NEOListNodeBase * next_m;
    NEOListNodeBase * previous_m;
};

template <class _Tp>
class NEOListNode:public NEOListNodeBase
{
public:
    NEOListNode(unsigned short id,_Tp& item):id_m(id),item_m(item)
    {

    }
    unsigned short id_m;
    _Tp& item_m;
};

template <class _Tp>
class NEOListIteratorBase:public NEOIteratorBase<_Tp>
{
public:
    NEOListIteratorBase(NEOListNodeBase* pNode):m_Node(pNode){}
    NEOListIteratorBase(){}
    void next(){ m_Node =  m_Node->next_m; }
    void previous(){ m_Node = m_Node->previous_m; }
    NEOListNodeBase *m_Node;
};


template <class _Tp>
class NEOListBase:public NEOAggregate<NEOListIterator<_Tp>>
{
public:
    NEOListBase(unsigned short id,_Tp& item)
    {
        m_Node = new  NEOListNode<_Tp>(id,item);
    }
    ~NEOListBase()
    {
        delete m_Node;
    }
    
    static NEOListNode<_Tp>* first_m;
protected:
    NEOListNode<_Tp>* m_Node;
};
template <class _Tp>
NEOListNode<_Tp>* NEOListBase<_Tp>::first_m = NULL;     
  
template <typename _Tp>
class NEOList:public NEOListBase<_Tp>
{
public:
    NEOList(unsigned short id,_Tp& item):NEOListBase<_Tp>(id,item)
    {
        registerListener();
    }
    ~NEOList()
    {
        unregisterListener();
    }
    //此处调用了NEOListIterator(NEOListNode<_Tp>* pNode)构造函数，并非强转
    static iterator begin(){ return first_m; }
    
    /*获取内置数据和索引*/
    _Tp& getItem(){return m_Node->item_m;};
    unsigned short getId()const {return m_Node->id_m;};
    
    void Print()
    {
        printf("this=%p,next_m=%p,previous_m=%p,\r\nitem_m=%d,id_m=%d,first_m=%p\r\n", first_m,first_m->next_m,first_m->previous_m,first_m->item_m,first_m->id_m,first_m);
        NEOListNode<_Tp>* iterator = (NEOListNode<_Tp>*)first_m->next_m;
        //需找合适的位置
        while((iterator!=first_m)){
            printf("this=%p,next_m=%p,previous_m=%p,\r\nitem_m=%d,id_m=%d,first_m=%p\r\n",
                        iterator,iterator->next_m,iterator->previous_m,iterator->item_m,
                        iterator->id_m,first_m);
            iterator = (NEOListNode<_Tp>*)iterator->next_m;
        }
    }
protected:

    void registerListener()
    {
        if (first_m==NULL){
            first_m = this->m_Node;
            first_m->next_m = first_m;
            first_m->previous_m = first_m;
        }
        else
        {
            NEOListNode<_Tp>* iterator = (NEOListNode<_Tp>*)first_m->next_m;
            //需找合适的位置
            while((iterator!=first_m) && (m_Node->id_m > iterator->id_m)){
                iterator = (NEOListNode<_Tp>*)iterator->next_m;
            }

            if(iterator==first_m)
            {
                this->m_Node->next_m = first_m;
                this->m_Node->previous_m = first_m->previous_m;
                first_m->previous_m->next_m = this->m_Node;
                first_m->previous_m = this->m_Node;
            }
            else
            {
                //将元素插入到合适的位置
                this->m_Node->previous_m = iterator->previous_m;
                this->m_Node->next_m = iterator;
                iterator->previous_m->next_m = this->m_Node;
                iterator->previous_m = this->m_Node;
            }
                     
        }
    }

     void unregisterListener()
    {
        NEOListNode<_Tp>* iterator = (NEOListNode<_Tp>*)first_m->next_m;
        while((iterator!=first_m) && (iterator!=this->m_Node)){
            iterator = (NEOListNode<_Tp>*)iterator->next_m;
        }
        if (iterator!=first_m) 
        {
            iterator->previous_m->next_m = iterator->next_m;
            iterator->next_m->previous_m = iterator->previous_m
        }
        else if (first_m!=NULL) 
        {
            first_m = (NEOListNode<_Tp>*)first_m->next_m;
        }
    }

};

template <typename _Tp>
class NEOListIterator:public NEOListIteratorBase<_Tp>
{
public:
    NEOListIterator(){}
    NEOListIterator(NEOListNode<_Tp>* pNode):NEOListIteratorBase<_Tp>(pNode){}
    NEOListIterator(const NEOListIterator *lt):NEOListIteratorBase<_Tp>(lt->m_Node){}

    /*运算符重载在使用是必须是对象而非指针*/
    //前缀形式++i
    virtual _Self& operator++()
    {
        this->next();
        return *this;
    }
    virtual _Self& operator--()
    {
        this->previous();
        return *this;
    }
    //后缀形式i++
    virtual _Self operator++(int)
    {
        NEOListIterator old = *this;
        this->next();
        return old;
    }
    virtual _Self operator--(int)
    {
        NEOListIterator old = *this;
        this->previous();
        return old;
    }
    virtual reference operator*() const
    {
        return ((NEOListNode<_Tp>*)m_Node)->item_m;
    }
    virtual pointer operator->() const
    {
        return &(operator*());
    }
};










template <typename I,typename C=I>
class GlobalIterator;

template <typename I,typename C=I>
class GlobalList:public NEOAggregate<GlobalIterator<I>>
{
public:
    GlobalList(unsigned short id,I& item):id_m(id),item_m(item),next_m(0){registerListener();};
    ~GlobalList(){unregisterListener();};
    static iterator begin(){return first_m;};
    static GlobalList* getFirst(){return first_m;};
    GlobalList* getNext(){return next_m;};
    I& getItem(){return item_m;};
    unsigned short getId()const {return id_m;};
protected:
    unsigned short id_m;
    I& item_m;
    static GlobalList* first_m;
    GlobalList* next_m;
    void registerListener()
    {
        if (first_m==0){
            first_m = this;
        }
        else{
            GlobalList* iterator = first_m;
            GlobalList* previous = 0;
            while((iterator!=0) && (id_m > iterator->id_m)){
                previous = iterator;
                iterator = iterator->next_m;
            }
            if (previous==0){
                this->next_m = first_m;
                first_m = this;
            }
            else{
                this->next_m = previous->next_m;
                previous->next_m = this;
            }
        }
    }
    void unregisterListener()
    {
        GlobalList* iterator = first_m;
        GlobalList* previous = 0;
        while((iterator!=0) && (iterator!=this)){
            previous = iterator;
            iterator = iterator->next_m;
        }
        if (previous!=0 && iterator!=0) previous->next_m = iterator->next_m;
        else if (first_m!=0) first_m = first_m->next_m;
    }
};

template <typename I,typename C>
GlobalList<I,C>* GlobalList<I,C>::first_m=0;

template <typename I,typename C=I>
class GlobalIterator
{
public:

    GlobalIterator(GlobalList<I>* pList)
    {
        iterator_m = pList;
    }
    GlobalIterator()
    {
        iterator_m = GlobalList<I,C>::getFirst();
    }
    GlobalIterator(const GlobalIterator& copy)
    {
        iterator_m = copy.iterator_m;
    }
    I* operator*()
    {
        return &iterator_m->getItem();
    };
    const I* operator*() const
    {
        return &iterator_m->getItem();
    };
    I* operator->()
    {
        return &iterator_m->getItem();
    };
    const I* operator->()const
    {
        return &iterator_m->getItem();
    };
    GlobalIterator operator++(int)
    {
        GlobalIterator old(*this);
        iterator_m = iterator_m->getNext();
        return old;
    };
    GlobalIterator& operator++()
    {
        iterator_m = iterator_m->getNext();
        return *this;
    };
    bool isValid()const {return iterator_m!=0;};
    operator bool() const{return isValid();};
    unsigned short getId()const{return iterator_m->getId();}
protected:
    GlobalList<I,C>* iterator_m;
};

}
#endif//_NEOITERATOR_H_