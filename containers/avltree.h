#ifndef __AVLTREE_H__
#define __AVLTREE_H__

#include "binarytree.h"

template<typename T>
struct AVLNode : public BinaryTreeNode<T, AVLNode<T>> {
    int m_height;

    // Propaga 'parent' a la base (necesario para los iteradores inorden)
    AVLNode(T data, Ref ref, AVLNode* parent = nullptr)
        : BinaryTreeNode<T, AVLNode<T>>(data, ref, parent), m_height(1) {}
};

template<typename Trait>
class AVLTree : public BinaryTree<Trait> {
public:
    using Node = typename Trait::Node;
    using value_type = typename Trait::value_type;

    int height() {
        std::shared_lock<std::shared_mutex> lock(this->m_mtx);
        return get_node_height(this->m_pRoot);
    }

protected:
    void internal_insert(Node*& pNode, const value_type& data, Ref ref, Node* parent = nullptr) override {
        // Inserción estándar (Lógica de la base)
        if (pNode == nullptr) {
            pNode = new Node(data, ref, parent);
            return;
        }

        auto branch = this->m_comp(pNode->m_data, data);
        internal_insert(pNode->m_pChild[branch], data, ref, pNode);

        //Lógica AVL: Se ejecuta al regresar de la recursión (post-orden)
        update_height(pNode);
        balance(pNode);
    }

private:
    int get_node_height(Node* n) { return n ? n->m_height : 0; }

    void update_height(Node* n) {
        if (n) {
            int hl = get_node_height(n->m_pChild[0]);
            int hr = get_node_height(n->m_pChild[1]);
            n->m_height = 1 + (hl > hr ? hl : hr);
        }
    }

    // Rota y mantiene m_pParent (n, q y el hijo que cambia de dueno)
    void rotate(Node*& n, bool b) {
        Node* P     = n->m_pParent;
        Node* q     = n->m_pChild[!b];
        Node* child = q->m_pChild[b];

        n->m_pChild[!b] = child;
        if (child) child->m_pParent = n;

        q->m_pChild[b] = n;
        n->m_pParent   = q;
        q->m_pParent   = P;

        update_height(n);
        update_height(q);
        n = q;
    }

    // b==1: rotacion derecha (izquierda pesada). b==0: rotacion izquierda (derecha pesada).
    void balance(Node*& n) {
        int bf = get_node_height(n->m_pChild[0]) - get_node_height(n->m_pChild[1]);

        // Izquierda pesada
        if (bf > 1) {
            // Left-Right: rota el hijo izquierdo primero
            if (get_node_height(n->m_pChild[0]->m_pChild[1]) > get_node_height(n->m_pChild[0]->m_pChild[0])) {
                rotate(n->m_pChild[0], 0);
            }
            rotate(n, 1);
        }
        // Derecha pesada
        else if (bf < -1) {
            // Right-Left: rota el hijo derecho primero
            if (get_node_height(n->m_pChild[1]->m_pChild[0]) > get_node_height(n->m_pChild[1]->m_pChild[1])) {
                rotate(n->m_pChild[1], 1);
            }
            rotate(n, 0);
        }
    }
};

#endif