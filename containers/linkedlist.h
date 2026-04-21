#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <cstddef> // size_t
#include <string>
#include <sstream>
#include <shared_mutex> // shared_mutex
#include <mutex> // unique_lock
#include "general_iterator.h"
#include "util.h"
#include "../types.h"
using namespace std;

// Forward iterator
template <typename Container>
class LinkedListForwardIterator : public general_iterator<Container, LinkedListForwardIterator<Container>>{
    public:
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
    // DONE: Completar el operator++
    MySelf operator++() {this->m_pNode=this->m_pNode->getNext(); return *this;}
};

// Linked List Node
template <typename T>
class LLNode{
    using Node = LLNode<T>;
private:
    T   m_data;
    Ref m_ref;
    Node *m_next;
public:

    LLNode() : m_data(T()), m_next(nullptr), m_ref(Ref()) {}
    LLNode(T data) : m_data(data), m_next(nullptr), m_ref(Ref()) {}
    LLNode(T data, Node *next) : m_data(data), m_next(next), m_ref(Ref()) {}
    LLNode(T data, Ref ref, Node *next) : m_data(data), m_ref(ref), m_next(next) {}
    virtual ~LLNode() {}

    T      getData() const { return m_data; }
    T&     getDataRef()    { return m_data; }
    void   setData(T data) { m_data = data; }
    Node*  getNext() const { return m_next; }
    Node*& getNextRef()    { return m_next; }
    void   setNext(Node *next) { m_next = next; }
    Ref    getRef() const { return m_ref; }
};

template <typename T>
struct AscendingLinkedListTrait{
    using value_type = T;
    using Node = LLNode<T>;
    using Comp = less<T>;
};

template <typename T>
struct DescendingLinkedListTrait{
    using value_type = T;
    using Node = LLNode<T>;
    using Comp = greater<T>;
};

template <typename Trait>
class LinkedList{
public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;
    using Comp       = typename Trait::Comp;
    using MySelf     = LinkedList<Trait>;

    using forward_iterator = LinkedListForwardIterator<MySelf>;
    // friend forward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_tail = nullptr;
    size_t m_size = 0;
    Comp   m_comp;
    mutable shared_mutex m_mtx;
public:
    LinkedList() {}
    LinkedList(const LinkedList &other); // Copy constructor
    LinkedList(LinkedList &&other){ // Move constructor
    }
    LinkedList& operator=(const LinkedList &other){ // Copy assignment operator
    }
    LinkedList& operator=(LinkedList &&other){ // Move assignment operator
    }
    
    virtual        ~LinkedList() {}
    virtual void    push_front(value_type value, Ref ref);
    virtual void    pop_front();
    virtual void    push_back(value_type value, Ref ref);
    virtual void    pop_back();
private:
            void    internal_insert(Node* &pParent, const value_type &value, Ref ref);
public:
    virtual void    insert(const value_type &value, Ref ref);
    
    virtual value_type& operator[](size_t index);
    virtual size_t  size() const;
    virtual string  toString() const;

    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    // Agregar Foreach
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){
        unique_lock<shared_mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }
};

template<typename Trait>
LinkedList<Trait>::LinkedList(const LinkedList<Trait> &other){
    shared_lock<shared_mutex> lock(other.m_mtx);

    Node *pNode = other.m_pRoot;
    Node *pPrev = nullptr;
    m_size = other.m_size;

    while(pNode){
        Node *pNewNode = new Node(pNode->getData(), pNode->getRef(), nullptr);
        if(!m_pRoot){
            m_pRoot = pNewNode;
        }
        else{
            pPrev->setNext(pNewNode);
        }
        pPrev = pNewNode;
        pNode = pNode->getNext();
    }
    m_tail = pPrev;
}

template <typename Trait>
void LinkedList<Trait>::internal_insert(Node* &pPrev, const value_type &value, Ref ref){
    if(!pPrev || m_comp(value, pPrev->getDataRef())){
        pPrev = new Node(value, ref, pPrev);
        m_size++;
        if(pPrev->getNext() == nullptr)
            m_tail = pPrev;
        return;
    }
    internal_insert(pPrev->getNextRef(), value, ref);
}

template <typename Trait>
void LinkedList<Trait>::insert(const value_type &value, Ref ref){
    internal_insert(m_pRoot, value, ref);
}

template <typename Trait>
void LinkedList<Trait>::push_front(value_type value, Ref ref){
    unique_lock<shared_mutex> lock(m_mtx);
    m_pRoot = new Node(value, ref, m_pRoot);
    m_size++;
    if(!m_tail)
        m_tail = m_pRoot;
}

template <typename Trait>
void LinkedList<Trait>::push_back(value_type value, Ref ref){
    unique_lock<shared_mutex> lock(m_mtx);
    Node *newNodo = new Node(value, ref, nullptr);

    if(!m_tail){
        m_pRoot = newNodo;
    }
    else{
        m_tail->setNext(newNodo);
    }
    m_tail = newNodo;
    m_size++;
}

template <typename Trait>
void LinkedList<Trait>::pop_front(){
    unique_lock<shared_mutex> lock(m_mtx);
    if(!m_pRoot)
        return;
    Node *temp = m_pRoot;
    m_pRoot = m_pRoot->getNext();
    delete temp;
    m_size--;
    //One element in list before pop
    if(m_pRoot == nullptr)
        m_tail = nullptr;
}

template <typename Trait>
void LinkedList<Trait>::pop_back(){
    unique_lock<shared_mutex> lock(m_mtx);
    if(!m_tail)
        return;

    //If there is only one element in the list
    if(m_pRoot == m_tail){
        delete m_pRoot;
        m_pRoot = m_tail = nullptr;
        m_size--;
        return;
    }
    else{
        Node *pCurr = m_pRoot;
        while(pCurr->getNext() != m_tail){
            pCurr = pCurr->getNext();
        }
        delete m_tail;
        m_tail = pCurr;
        m_tail->setNext(nullptr);
    }
    m_size--;
}

template<typename Trait>
typename LinkedList<Trait>::value_type& LinkedList<Trait>::operator[](size_t index){
    shared_lock<shared_mutex> lock(m_mtx);
    if(index >= m_size)
        throw out_of_range("Index out of range");
    
    Node *pCurr = m_pRoot;
    for(size_t i = 0; i < index; ++i)
        pCurr = pCurr->getNext();
    return pCurr->getDataRef();
}

template<typename Trait>
size_t LinkedList<Trait>::size() const{
    shared_lock<shared_mutex> lock(m_mtx);
    return m_size;
}

template<typename Trait>
string LinkedList<Trait>::toString() const{
    shared_lock<shared_mutex> lock(m_mtx);
    return "test";
}

void ListsDemo();

#endif // __LINKEDLIST_H__