#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <shared_mutex>
#include <mutex>

#include "../types.h"
#include "avltree.h"     // trae binarytree.h, tree_iterator.h, traits.h

using namespace std;

// Iterador inorden que devuelve (key, value) = (m_data, m_ref) para structured bindings.
template <typename Container>
class HashTableKVIterator {
public:
    using Node       = typename Container::Node;
    using value_type = typename Container::value_type;

private:
    Node *m_pNode;

public:
    HashTableKVIterator(Node *pNode = nullptr) : m_pNode(pNode) {}

    bool operator==(const HashTableKVIterator &o) const { return m_pNode == o.m_pNode; }
    bool operator!=(const HashTableKVIterator &o) const { return m_pNode != o.m_pNode; }

    // key por valor, value por referencia (mutable)
    std::tuple<value_type, Ref&> operator*() const {
        return std::tuple<value_type, Ref&>(m_pNode->m_data, m_pNode->m_ref);
    }

    HashTableKVIterator& operator++() {
        if (m_pNode)
            m_pNode = TreeLogic<Node>::inorder_successor(m_pNode);
        return *this;
    }
};

// Mapa ordenado sobre AVL. key = m_data, value = m_ref.
template <typename Trait>
class HashTable : public AVLTree<Trait> {
public:
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;
    using iterator   = HashTableKVIterator<HashTable<Trait>>;

    HashTable() : AVLTree<Trait>() {}

    // Copia: reinserta los pares del origen (copia independiente, balanceada).
    HashTable(const HashTable &other) : AVLTree<Trait>() {
        shared_lock<shared_mutex> lock(other.m_mtx);
        copy_insert(other.m_pRoot);
    }

    // Move: delega en el move del AVL/BinaryTree.
    HashTable(HashTable &&other) noexcept : AVLTree<Trait>(std::move(other)) {}

    // m[key]: devuelve ref al value; si la clave no existe, la inserta.
    Ref& operator[](const value_type &key) {
        {
            shared_lock<shared_mutex> lock(this->m_mtx);
            Node *found = find_node(this->m_pRoot, key);
            if (found) return found->m_ref;
        }
        this->insert(key, Ref());
        shared_lock<shared_mutex> lock(this->m_mtx);
        Node *found = find_node(this->m_pRoot, key);
        return found->m_ref;
    }

    // Iteracion key-value en orden de claves
    iterator begin() { return iterator(TreeLogic<Node>::minimum(this->m_pRoot)); }
    iterator end()   { return iterator(nullptr); }

    // Imprime {key: value, ...} en orden
    friend ostream& operator<<(ostream &os, const HashTable &m) {
        shared_lock<shared_mutex> lock(m.m_mtx);
        os << "{";
        bool first = true;
        print_inorder(os, m.m_pRoot, first);
        os << "}";
        return os;
    }

    // Parsea {key: value, ...} y rellena el mapa
    friend istream& operator>>(istream &is, HashTable &m) {
        char c;
        is >> ws;
        if (is.peek() == '{') is >> c;

        while (is >> ws) {
            char nx = is.peek();
            if (nx == '}') { is >> c; break; }
            if (nx == ',') { is >> c; continue; }

            value_type key;
            if (!(is >> key)) { is.clear(); break; }

            is >> ws;
            if (is.peek() == ':') is >> c;
            Ref val = 0;
            is >> val;

            m[key] = val;
        }
        return is;
    }

private:
    // Busca un nodo por clave (criterio de rama del BST)
    Node* find_node(Node *curr, const value_type &key) const {
        while (curr) {
            if (curr->m_data == key) return curr;
            bool branch = this->m_comp(curr->m_data, key);
            curr = curr->m_pChild[branch];
        }
        return nullptr;
    }

    // Reinserta en preorden los pares del origen (copia)
    void copy_insert(const Node *n) {
        if (!n) return;
        this->insert(n->m_data, n->m_ref);
        copy_insert(n->m_pChild[0]);
        copy_insert(n->m_pChild[1]);
    }

    // Recorrido inorden: imprime key: value
    static void print_inorder(ostream &os, const Node *n, bool &first) {
        if (!n) return;
        print_inorder(os, n->m_pChild[0], first);
        if (!first) os << ", ";
        os << n->m_data << ": " << n->m_ref;
        first = false;
        print_inorder(os, n->m_pChild[1], first);
    }
};

#endif // __HASHTABLE_H__