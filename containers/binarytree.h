#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__
#include <iostream>
#include <cstddef>
#include <string>
#include <fstream>
#include <thread>
#include <shared_mutex>
#include <mutex>
#include <sstream>

#include "../types.h"
#include "traits.h"
#include "tree_iterator.h"

using namespace std;

// === BINARY TREE NODE ====

template<typename T, typename DerivedNode = void>
struct BinaryTreeNode{
    using value_type = T;
    using Node       = std::conditional_t<std::is_void_v<DerivedNode>, BinaryTreeNode<T>, DerivedNode>;
    
    T m_data;
    Ref m_ref;
    Node *m_pChild[2];
    Node *m_pParent;

    //Constructor
    BinaryTreeNode(T data, Ref ref, Node* parent=nullptr) : m_data(data), m_ref(ref), m_pChild{nullptr, nullptr}, m_pParent(parent) {}

    // Getters
    T& getDataRef() { return m_data; }
    Ref getRef() const { return m_ref; }
    Node* getLeft() const { return m_pChild[0]; }
    Node* getRight() const { return m_pChild[1]; }
    Node* getParent() const { return m_pParent; }
};

// === BINARY TREE ====

template<typename Trait>
class BinaryTree{

public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;
    using Comp       = typename Trait::Comp;
    using iterator   = BTInorderForwardIterator<BinaryTree<Trait>>;
    using preorder_iterator = BTPreorderForwardIterator<BinaryTree<Trait>>;
    using postorder_iterator = BTPostorderForwardIterator<BinaryTree<Trait>>;

protected:
    Node    *m_pRoot;
    Comp     m_comp;
    size_t   m_size;
    mutable shared_mutex m_mtx;

public:

    //Constructor
    BinaryTree() : m_pRoot(nullptr), m_size(0) {}
    
    // Constructor Copia (T3)
    BinaryTree(const BinaryTree& other) : m_pRoot(nullptr), m_size(0) {
        shared_lock<shared_mutex> lock(other.m_mtx);
        m_pRoot = internal_copy(other.m_pRoot);
        m_size  = other.m_size;
    }

    // Move constructor (T4)
    BinaryTree(BinaryTree&& other) noexcept : m_pRoot(nullptr), m_size(0) {
        unique_lock<shared_mutex> lock(other.m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_size  = exchange(other.m_size, (size_t)0);
    }
    
    // Destructor Seguro (T5)
    ~BinaryTree() { 
        unique_lock<shared_mutex> lock(m_mtx);
        internal_destroy(m_pRoot); 
    }

    // Forward Iterator Inorder (T6) - Iterador por defecto
    iterator begin() { return iterator(this, TreeLogic<Node>::minimum(m_pRoot)); }
    iterator end() { return iterator(this, nullptr); }

    // Forward Iterator Preorder (T13)
    preorder_iterator begin_pre() { return preorder_iterator(this, m_pRoot); }
    preorder_iterator end_pre() { return preorder_iterator(this, nullptr); }

    auto preorder() {
        struct View {
            preorder_iterator b, e;
            preorder_iterator begin() { return b; }
            preorder_iterator end()   { return e; }
        };
        return View{this->begin_pre(), this->end_pre()};
    }

    // Forward Iterator Postorder (T14)
    postorder_iterator begin_post() { return postorder_iterator(this, TreeLogic<Node>::postorder_minimum(m_pRoot)); }
    postorder_iterator end_post() { return postorder_iterator(this, nullptr); }

    auto postorder() {
        struct View {
            postorder_iterator b, e;
            postorder_iterator begin() { return b; }
            postorder_iterator end()   { return e; }
        };
        return View{this->begin_post(), this->end_post()};
    }

    // Bucle ForEach nativo (T8)
    template<typename Func, typename... Args>
    void ForEach(Func func, Args&&... args) {
        std::shared_lock<std::shared_mutex> lock(m_mtx);
        if (!m_pRoot) return; 
        for (auto it = begin(); it != end(); ++it) {
            func(*it, std::forward<Args>(args)...);
        }
    }

    void insert(const value_type &data, Ref ref){
        internal_insert(m_pRoot, data, ref);
    }

    // ToString (T9)
    string ToString() const {
        shared_lock<shared_mutex> lock(m_mtx);
        std::ostringstream oss;
        oss << "[";
        bool first = true;

        for (auto it = const_cast<BinaryTree*>(this)->begin(); 
            it != const_cast<BinaryTree*>(this)->end(); ++it) {
        
            if (!first) oss << ", ";        
            oss << *it;    
            first = false;
        
        }
        oss << "]";
        return oss.str();
    }

    // Operador << (T10)
    friend std::ostream& operator<<(std::ostream& os, const BinaryTree& tree) {
        os << tree.ToString();
        return os;
    }

    // Operador >> (T11)
    friend std::istream& operator>>(std::istream& is, BinaryTree<Trait>& tree) {
    char c;
    
    is >> std::ws; 
    if (is.peek() == '[') is >> c; 

    while (is) {
        value_type data;
        
        if (is >> data) {
            tree.insert(data, 0);
            
            is >> std::ws;
            char next = is.peek();
            if (next == ',' || next == ']') {
                is >> c; 
                if (c == ']') break; 
            }
        } else {
            is.clear();
            is >> c;
            if (c == ']') break;
        }
    }
    return is;
    }


    // Busca Elemento(Mejora Libre)
    bool find(const value_type& data) const {
    std::shared_lock<std::shared_mutex> lock(m_mtx);
    Node* curr = m_pRoot;
    while (curr) {
        if (curr->m_data == data) return true;
        bool branch = m_comp(curr->m_data, data);
        curr = curr->m_pChild[branch];
    }
    return false;
}

protected:
    virtual void internal_insert(Node* &pNode, const value_type &data, Ref ref, Node* parent=nullptr);
    
    void internal_destroy(Node* node){
        if (!node) return;
        internal_destroy(node->m_pChild[0]);
        internal_destroy(node->m_pChild[1]);
        delete node;
    }
    
    Node* internal_copy(Node* src) {
        if (!src) return nullptr;
        Node* n = new Node(src->m_data);
        n->m_pChild[0] = internal_copy(src->m_pChild[0]);
        n->m_pChild[1] = internal_copy(src->m_pChild[1]);
        return n;
    }

};


// Codigo hecho en clase
template<typename Trait>
void BinaryTree<Trait>::internal_insert(Node* &pNode, const value_type &data, Ref ref, Node* parent){
    if( pNode == nullptr ){
        pNode = new Node(data,ref,parent);
        return;
    }
    auto branch = m_comp(pNode->m_data, data);
    internal_insert(pNode->m_pChild[branch], data, ref, pNode);
}





#endif // __BINARYTREE_H__ 