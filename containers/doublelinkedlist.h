

#include "linkedlist.h"

// DONE Los iteradores ahora son forward y backward
// Crear 2 nuevos i

//DOUBLE LINKED LIST FORWARD ITERATOR
template<typename T>
class DoubleLinkedListForwardIterator : public general_iterator<T, DoubleLinkedListForwardIterator<T>>{
    using MySelf = DoubleLinkedListForwardIterator<T>;
    using Parent = general_iterator<T, MySelf>;
    using Parent::Parent;
    
    MySelf operator++() {
        if (this->m_pNode) {
            this->m_pNode = this->m_pNode->getNext();
        }
        return *this;
    }
};

//DOUBLE LINKED LIST BACKWARD ITERATOR
template<typename T>
class DoubleLinkedListBackwardIterator : public general_iterator<T, DoubleLinkedListBackwardIterator<T>>{
    using MySelf = DoubleLinkedListBackwardIterator<T>;
    using Parent = general_iterator<T, MySelf>;
    using Parent::Parent;
    
    MySelf operator++() {
        if (this->m_pNode) {
            this->m_pNode = this->m_pNode->getPrev();
        }
        return *this;
    }
};

template <typename T>
class DLLNode : public LLNode<T, DLLNode<T>>{
    private:
        Node *m_pPrev;
    public:
        DLLNode() : LLNode<T, DLLNode<T>>(), m_pPrev(nullptr) {}
        DLLNode(T data, Ref ref, Node *next = nullptr, Node *prev = nullptr) : LLNode<T, DLLNode<T>>(data, ref, next), m_pPrev(prev) {}

        Node*  getPrev() const     { return m_pPrev; }
        void   setPrev(Node *prev) { m_pPrev = prev; }
        Node*& getPrevRef()        { return m_pPrev; }

};

template <typename T>
struct AscendingDLLTrait : BaseTrait<T, less<T>>{
    using Node = DLLNode<T>;
};

template <typename T>
struct DescendingDLLTrait : BaseTrait<T, greater<T>>{
    using Node = DLLNode<T>;
};

template <typename Trait>
class DoubleLinkedList : public LinkedList<Trait>{

    // TODO: Copy constructor
    //       Simplificar y abstraer el bucle de copia de Nodes
    //       Es posible que no necesites este constructor ya que lo heredaste

    // TODO: Move constructor
    // TODO: Copy assignment operator
    // TODO: Move assignment operator
};