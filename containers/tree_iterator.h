#ifndef __TREE_ITERATOR_H__
#define __TREE_ITERATOR_H__

#include "general_iterator.h"

template <typename Trait>
class BinaryTree;

// Lógica de navegación
template <typename Node>
struct TreeLogic {

    //MINIMO: Más a la izquierda en un ASC Binary Tree
    static Node* minimum(Node* n) {
        if (!n) return nullptr;
        while (n->m_pChild[0]) n = n->m_pChild[0];
        return n;
    }

    //MAXIMO: Más a la derecha en un ASC Binary Tree
    static Node* maximum(Node* n) {
        if (!n) return nullptr;
        while (n->m_pChild[1]) n = n->m_pChild[1];
        return n;
    } 

    static Node* inorder_successor(Node* n) {
        if (!n) return nullptr;
        if (n->m_pChild[1]) return minimum(n->m_pChild[1]);
        
        // Subir hasta encontrar un padre que sea hijo izquierdo
        auto* p = n->m_pParent;
        while (p && n == p->m_pChild[1]) {
            n = p;
            p = p->m_pParent;
        }
        return p;
    }

    static Node* inorder_predecessor(Node* n) {
        if (!n) return nullptr;
        if (n->m_pChild[0]) return maximum(n->m_pChild[0]);
        
        // Subir hasta encontrar un padre que sea hijo derecho
        auto* p = n->m_pParent;
        while (p && n == p->m_pChild[0]) {
            n = p;
            p = p->m_pParent;
        }
        return p;
    }

    static Node* preorder_successor(Node* n) {
        if (!n) return nullptr;
        //Si hay hijo izquierdo, es el siguiente
        if (n->m_pChild[0]) return n->m_pChild[0];
        //Si no hay izquierdo, el siguiente es el derecho
        if (n->m_pChild[1]) return n->m_pChild[1];
        
        //Si no hay hijos, subir hasta encontrar un ancestro con hijo derecho
        //que no hayamos visitado (venimos del izquierdo)
        Node* p = n->m_pParent;
        while (p && (p->m_pChild[1] == nullptr || n == p->m_pChild[1])) {
            n = p;
            p = p->m_pParent;
        }
        return (p == nullptr) ? nullptr : p->m_pChild[1];
    }
    
    static Node* preorder_predecessor(Node* n) {
        if (!n) return nullptr;
        Node* p = n->m_pParent;
        if (!p) return nullptr; // La raíz no tiene predecesor en preorder

        // Si es hijo derecho y hay un hermano izquierdo
        if (n == p->m_pChild[1] && p->m_pChild[0]) {
            // El predecesor es el nodo más profundo (derecha-izquierda) del hermano izq
            Node* cur = p->m_pChild[0];
            while (true) {
                if (cur->m_pChild[1]) cur = cur->m_pChild[1];
                else if (cur->m_pChild[0]) cur = cur->m_pChild[0];
                else break;
            }
            return cur;
        }
        // Si es hijo izquierdo, el predecesor el padre
        return p;
    }

    static Node* postorder_minimum(Node* n) {
        if (!n) return nullptr;
        // El inicio de postorder es la hoja más profunda
        while (n->m_pChild[0] || n->m_pChild[1]) {
            if (n->m_pChild[0]) n = n->m_pChild[0];
            else n = n->m_pChild[1];
        }
        return n;
    }

    static Node* postorder_successor(Node* n) {
        if (!n) return nullptr;
        Node* p = n->m_pParent;
        if (!p) return nullptr; // La raíz es el último nodo

        // Si es hijo derecho, sucesor es el padre
        if (n == p->m_pChild[1]) return p;
        
        // Si hay hermano derecho, el sucesor es el postorder_minimum de ese hermano
        if (p->m_pChild[1]) return postorder_minimum(p->m_pChild[1]);
        // Si no hay hermano derecho, el sucesor es el padre
        return p;

    }

    static Node* postorder_predecessor(Node* n) {
        if (!n) return nullptr;
        // Si hay hijo derecho, es el anterior
        if (n->m_pChild[1]) return n->m_pChild[1];
        // Si no hay derecho, pero hay izquierdo, es el anterior
        if (n->m_pChild[0]) return n->m_pChild[0];

        // Si es hoja, subir hasta encontrar un padre que tenga hijo izquierdo
        Node* p = n->m_pParent;
        while (p && (p->m_pChild[0] == nullptr || n == p->m_pChild[0])) {
            n = p;
            p = p->m_pParent;
        }
        return (p == nullptr) ? nullptr : p->m_pChild[0];
    }

};


/*
------INORDER------
*/

// Iterador Forward
template <typename Container>
class BTInorderForwardIterator : public general_iterator<Container, BTInorderForwardIterator<Container>> {
public:
    using MySelf = BTInorderForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    
    // Se hereda el constructor de general_iterator
    using Parent::Parent;

    MySelf& operator++() {
        if (this->m_pNode) {
            // El tipo Node se extrae del Container (BinaryTree)
            using Node = typename Container::Node;
            this->m_pNode = TreeLogic<Node>::inorder_successor(this->m_pNode);
        }
        return *this;
    }
};


// Iterador Backward
template <typename Container>
class BTInorderBackwardIterator : public general_iterator<Container, BTInorderBackwardIterator<Container>> {
public:
    using MySelf = BTInorderBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;

    MySelf& operator++() {
        if (this->m_pNode) {
            using Node = typename Container::Node;
            this->m_pNode = TreeLogic<Node>::predecessor(this->m_pNode);
        }
        return *this;
    }
};


/*
------PREORDER------
*/

template <typename Container>
class BTPreorderForwardIterator : public general_iterator<Container, BTPreorderForwardIterator<Container>> {
public:
    using MySelf = BTPreorderForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;

    MySelf& operator++() {
        if (this->m_pNode) {
            using Node = typename Container::Node;
            this->m_pNode = TreeLogic<Node>::preorder_successor(this->m_pNode);
        }
        return *this;
    }
};

// --- Preorder Backward ---
template <typename Container>
class BTPreorderBackwardIterator : public general_iterator<Container, BTPreorderBackwardIterator<Container>> {
public:
    using MySelf = BTPreorderBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;

    MySelf& operator++() {
        if (this->m_pNode) {
            using Node = typename Container::Node;
            this->m_pNode = TreeLogic<Node>::preorder_predecessor(this->m_pNode);
        }
        return *this;
    }
};

/*
------POSTORDER------
*/

template <typename Container>
class BTPostorderForwardIterator : public general_iterator<Container, BTPostorderForwardIterator<Container>> {
public:
    using MySelf = BTPostorderForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;

    MySelf& operator++() {
        if (this->m_pNode) {
            using Node = typename Container::Node;
            this->m_pNode = TreeLogic<Node>::postorder_successor(this->m_pNode);
        }
        return *this;
    }
};

template <typename Container>
class BTPostorderBackwardIterator : public general_iterator<Container, BTPostorderBackwardIterator<Container>> {
public:
    using MySelf = BTPostorderBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;

    MySelf& operator++() {
        if (this->m_pNode) {
            using Node = typename Container::Node;
            this->m_pNode = TreeLogic<Node>::postorder_predecessor(this->m_pNode);
        }
        return *this;
    }
};



#endif