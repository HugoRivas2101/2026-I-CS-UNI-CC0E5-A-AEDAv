#include <iostream>
#include <cstddef>
#include <string>
#include <sstream>
#include <stdexcept>
#include <mutex>
#include <shared_mutex> 
#include <utility>
#include <tuple>
#include <optional>
#include "util.h"
#include "../types.h"
#include "traits.h"
#include "linkedlist.h"
#include "general_iterator.h"

//DONE: FORWARD ITERATOR
template <typename Container>
class DoubleLinkedListForwardIterator : public general_iterator<Container, LinkedListForwardIterator<Container>>{
public:
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
    
    MySelf operator++() {
        if (this->m_pNode) {
            this->m_pNode = this->m_pNode->getNext();
        }
        return *this;
    }
};

//DONE: BACKWARD ITERATOR
template <typename Container>
class DoubleLinkedListBackwardIterator : public general_iterator<Container, DoubleLinkedListBackwardIterator<Container>>{
public:
    using MySelf = DoubleLinkedListBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;

    MySelf operator++() {
        if (this->m_pNode) {
            this->m_pNode = this->m_pNode->getPrev();
        }
        return *this;
    }
};

template <typename T>
class DLLNode : public LLNodeBase<T, DLLNode<T>>{
    protected:
        using Node = DLLNode<T>;
        Node *m_pPrev;
    public:
        DLLNode() : LLNodeBase<T, DLLNode<T>>(), m_pPrev(nullptr) {}
        DLLNode(T data, Ref ref, Node *next = nullptr, Node *prev = nullptr) : LLNodeBase<T, DLLNode<T>>(data, ref, next), m_pPrev(prev) {}

        Node*  getPrev() const     { return m_pPrev; }
        void   setPrev(Node *prev) { m_pPrev = prev; }
        Node*& getPrevRef()        { return m_pPrev; }
};


// DONE: Adapatar Traits a BaseTrait
template <typename T>
struct AscendingDLLTrait : BaseTrait<T, less<T>>{
    using Node = DLLNode<T>;
};

// DONE: Adapatar Traits a BaseTrait
template <typename T>
struct DescendingDLLTrait : BaseTrait<T, greater<T>>{
    using Node = DLLNode<T>;
};

template <typename Trait>
class DoubleLinkedList : public LinkedList<Trait>{
public:

    using MySelf = DoubleLinkedList<Trait>;
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;

    using forward_iterator = DoubleLinkedListForwardIterator<MySelf>;
    using backward_iterator = DoubleLinkedListBackwardIterator<MySelf>;

    friend forward_iterator;
    friend backward_iterator;

    DoubleLinkedList() {}

    // DONE: Copy constructor
    DoubleLinkedList(const DoubleLinkedList &other) {
        shared_lock<shared_mutex> lock(other.m_mtx);
        for(Node* curr = other.m_pRoot; curr != nullptr; curr = curr->getNext()) {
            push_back(curr->getData(), curr->getRef());
        }
    }

    // DONE: Move constructor
    DoubleLinkedList(DoubleLinkedList &&other) {
        unique_lock<shared_mutex> lockOther(other.m_mtx);
        this->m_pRoot = std::exchange(other.m_pRoot, nullptr);
        this->m_tail  = std::exchange(other.m_tail, nullptr);
        this->m_size  = std::exchange(other.m_size, 0);
    }

    // DONE: Copy assignment operator
    DoubleLinkedList& operator=(const DoubleLinkedList &other) {
        
        if(this != &other){
            while (this->m_size > 0) this->pop_front();
            shared_lock<shared_mutex> lock(other.m_mtx);
            for (Node* curr = other.m_pRoot; curr != nullptr; curr = curr->getNext()) {
                push_back(curr->getData(), curr->getRef());
            }
        }
        return *this;
    }

    // DONE: Move assignment operator
    DoubleLinkedList& operator=(DoubleLinkedList &&other) {
        if (this != &other) {
            while (this->m_size > 0) this->pop_front();
            unique_lock<shared_mutex> lockOther(other.m_mtx);
            this->m_pRoot = std::exchange(other.m_pRoot, nullptr);
            this->m_tail  = std::exchange(other.m_tail, nullptr);
            this->m_size  = std::exchange(other.m_size, 0);
        }
        return *this;
    }

    // DONE: Destructor Seguro
    virtual ~DoubleLinkedList() {
        std::unique_lock<std::shared_mutex> lock(this->m_mtx);
        
        Node* current = this->m_pRoot;
        while (current) {
            Node* next = current->getNext();
            delete current;
            current = next;             
        }
        
        this->m_pRoot = nullptr;
        this->m_tail = nullptr;
        this->m_size = 0;
    }

private:
    void internal_insert(Node* &curr, Node* prev, const value_type &value, Ref ref) {
        if (!curr || this->m_comp(value, curr->getDataRef())) {
            Node* new_node = new Node(value, ref, curr, prev);
            if (curr != nullptr)
                curr->setPrev(new_node);
            else
                this->m_tail = new_node;
            curr = new_node;
            this->m_size++;
            return;
        }
        internal_insert(curr->getNextRef(), curr, value, ref);
    }
public:

    void insert(const value_type &value, Ref ref) override{
        unique_lock<shared_mutex> lock(this->m_mtx);
        internal_insert(this->m_pRoot, nullptr, value, ref);

    }

    forward_iterator begin() { return forward_iterator(this, this->m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    backward_iterator rbegin() { return backward_iterator(this,this->m_tail); }
    backward_iterator rend()   { return backward_iterator(this, nullptr); }

    
    // DONE: ForEach Nativo
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args) {
        unique_lock<shared_mutex> lock(this->m_mtx);
        for(auto it = begin(); it != end(); ++it){
            func(*it, forward<Args>(args)...);
        }
    }

    // DONE: ReverseForEach Nativo
    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&... args){
        unique_lock<shared_mutex> lock(this->m_mtx);
        for(auto it = rbegin(); it != rend(); ++it){
            func(*it, forward<Args>(args)...);
        }
    }

    // DONE: pop_back
    std::tuple<value_type, Ref> pop_back() override{
        unique_lock<shared_mutex> lock(this->m_mtx);
        //Empty List
        if (!this->m_tail){
            return std::make_tuple(value_type{}, Ref{});
        }

        Node* oldTail = this->m_tail;
        value_type outData = oldTail->getData();
        Ref outRef = oldTail->getRef();

        if (this->m_size > 1) {
            this->m_tail = oldTail->getPrev();
            this->m_tail->setNext(nullptr);
        } else {
            this->m_pRoot = nullptr;
            this->m_tail = nullptr;
        }
        delete oldTail;
        this->m_size--;
        return std::make_tuple(outData, outRef);
    }

    // DONE: pop_front
    std::tuple<value_type, Ref> pop_front() override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (!this->m_pRoot) {
            return std::make_tuple(value_type{}, Ref{});
        }
        Node* oldRoot = this->m_pRoot;
        value_type outData = oldRoot->getData();
        Ref outRef = oldRoot->getRef();

        if (this->m_size > 1) {
            this->m_pRoot = oldRoot->getNext();
            this->m_pRoot->setPrev(nullptr);
        } else {
            this->m_pRoot = nullptr;
            this->m_tail = nullptr;
        }

        delete oldRoot;
        this->m_size--;
        return std::make_tuple(outData, outRef);
    }

    // DONE: push_back
    void push_back(value_type value, Ref ref) override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        Node* new_node = new Node(value, ref, nullptr, this->m_tail);
        if (this->m_tail)
            this->m_tail->setNext(new_node);
        else
            this->m_pRoot = new_node;
        this->m_tail = new_node;
        this->m_size++;
    }

    // DONE: push_front 
    void push_front(value_type value, Ref ref) override {
        unique_lock<shared_mutex> lock(this->m_mtx);
        Node* new_node = new Node(value, ref, this->m_pRoot, nullptr);
        if (this->m_pRoot)
            this->m_pRoot->setPrev(new_node);
        else
            this->m_tail = new_node;
        this->m_pRoot = new_node;
        this->m_size++;
    }

    //DONE: OPERADOR <<
    friend ostream& operator<<(ostream& os, const DoubleLinkedList& list) {
        shared_lock<shared_mutex>lock(list.m_mtx); 
        os << "[";
        Node* curr = list.m_pRoot;
        while(curr){
            os << "(" << curr->getData() << "," << curr->getRef() << ")";
            if(curr->getNext()) {
                os << ",";
            } 
            curr = curr->getNext();
        }
        os << "]";
        return os;
    }

    //DONE: OPERADOR >>
    friend istream& operator>>(istream& is, DoubleLinkedList& list) {
        char ch;
        if (!(is >> ch) || ch != '[') {
            is.clear(ios_base::failbit);
            return is;
        }
        value_type val;
        Ref ref;
        char comma, parenClose;
        while (is >> ch && ch != ']') {
            if (ch == '(') {
                if (is >> val >> comma >> ref >> parenClose) {
                    if (comma == ',' && parenClose == ')') {
                        list.push_back(val, ref);
                    }
                }
            }
        }
        return is;
    }

    //DONE: MEJORA EN EL OPERADOR []
    value_type operator[](size_t index) const {
        shared_lock<shared_mutex> lock(this->m_mtx);
        if (index >= this->m_size) throw out_of_range("Error: Indice fuera de rango");
        
        Node* current=nullptr;
        
        if (index < this->m_size / 2) {
            current = this->m_pRoot;
            for (size_t i = 0; i < index; ++i) {
                current = current->getNext();
            }
        } else {
            current = this->m_tail;
            for (size_t i = this->m_size - 1; i > index; --i) {
                current = current->getPrev();
            }
        }
        return current->getData();
    }
};