#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <cstddef> // size_t
#include <string>
#include <sstream>
#include <stdexcept>
#include <shared_mutex>
#include <mutex>
#include <utility>
#include <tuple>
#include "util.h"
#include "vector.h"
#include "../types.h"
#include "traits.h"
using namespace std;

// Traits del Heap: exponen value_type y Comp. MinHeap saca el menor; MaxHeap el mayor.
template <typename T>
struct MinHeapTrait{
    using value_type = T;
    using Comp       = less<T>;
};

template <typename T>
struct MaxHeapTrait{
    using value_type = T;
    using Comp       = greater<T>;
};

// Heap sobre un Vector: arbol implicito por indices.
template<typename Trait>
class Heap{
public:
    using value_type = typename Trait::value_type;
    using Comp       = typename Trait::Comp;
    using MySelf     = Heap<Trait>;
    using Node       = typename Vector<Trait>::Node;

private:
    Vector<Trait>        m_vec;
    Comp                 m_comp;
    mutable shared_mutex m_mtx;

    // Indices padre/hijo
    static size_t parent(size_t i)     { return (i - 1) / 2; }
    static size_t leftChild(size_t i)  { return 2 * i + 1; }
    static size_t rightChild(size_t i) { return 2 * i + 2; }

    // Intercambia dos nodos del vector
    void swapNodes(size_t a, size_t b){
        Node tmp = m_vec[a];
        m_vec[a] = m_vec[b];
        m_vec[b] = tmp;
    }

public:
    Heap() : m_vec(), m_comp() {}
    ~Heap() {}

    // Sube el nodo mientras tenga mayor prioridad que su padre
    void heapifyUp(size_t index){
        while(index > 0){
            size_t p = parent(index);
            if(m_comp(m_vec[index].getDataRef(), m_vec[p].getDataRef())){
                swapNodes(index, p);
                index = p;
            } else {
                break;
            }
        }
    }

    // Baja el nodo eligiendo el hijo de mayor prioridad
    void heapifyDown(size_t index){
        size_t n = m_vec.size();
        while(true){
            size_t best = index;
            size_t l = leftChild(index);
            size_t r = rightChild(index);

            if(l < n && m_comp(m_vec[l].getDataRef(), m_vec[best].getDataRef()))
                best = l;
            if(r < n && m_comp(m_vec[r].getDataRef(), m_vec[best].getDataRef()))
                best = r;

            if(best == index)
                break;
            swapNodes(index, best);
            index = best;
        }
    }

    // Agrega al final y reordena hacia arriba
    void insert(value_type value, Ref ref){
        unique_lock<shared_mutex> lock(m_mtx);
        m_vec.push_back(value, ref);
        heapifyUp(m_vec.size() - 1);
    }

    // Extrae la raiz (mayor prioridad) y la devuelve como (dato, ref)
    tuple<value_type, Ref> extract(){
        unique_lock<shared_mutex> lock(m_mtx);
        size_t n = m_vec.size();
        if(n == 0)
            throw out_of_range("Heap vacio: extract");

        Node& root = m_vec[0];
        tuple<value_type, Ref> result = make_tuple(root.getData(), root.getRef());

        if(n == 1){
            m_vec.pop_back();
            return result;
        }
        // Mueve la raiz al final, la remueve y reordena hacia abajo
        swapNodes(0, n - 1);
        m_vec.pop_back();
        heapifyDown(0);
        return result;
    }

    // Lee la cima sin removerla
    tuple<value_type, Ref> peek(){
        shared_lock<shared_mutex> lock(m_mtx);
        if(m_vec.size() == 0)
            throw out_of_range("Heap vacio: peek");
        Node& root = m_vec[0];
        return make_tuple(root.getData(), root.getRef());
    }

    bool isEmpty(){
        shared_lock<shared_mutex> lock(m_mtx);
        return m_vec.size() == 0;
    }

    size_t size(){
        shared_lock<shared_mutex> lock(m_mtx);
        return m_vec.size();
    }

    string toString(){
        shared_lock<shared_mutex> lock(m_mtx);
        ostringstream oss;
        oss << "[";
        for(size_t i = 0; i < m_vec.size(); ++i){
            if(i > 0) oss << ", ";
            oss << "(" << m_vec[i].getData() << "," << m_vec[i].getRef() << ")";
        }
        oss << "]";
        return oss.str();
    }

    // Imprime el arreglo del heap
    friend ostream& operator<<(ostream& os, Heap<Trait>& heap){
        return os << heap.toString();
    }

    // Lee pares (d,r) en formato [(d,r),...] e inserta
    friend istream& operator>>(istream& is, Heap<Trait>& heap){
        char c;
        is >> ws;
        if(is.peek() == '[') is >> c;

        while(is >> ws){
            char nx = is.peek();
            if(nx == ']'){ is >> c; break; }
            if(nx == ',' ){ is >> c; continue; }
            if(nx == '(' ){ is >> c; }

            value_type data;
            Ref ref = 0;
            if(!(is >> data)) { is.clear(); break; }

            is >> ws;
            if(is.peek() == ','){ is >> c; is >> ref; }
            is >> ws;
            if(is.peek() == ')'){ is >> c; }

            heap.insert(data, ref);
        }
        return is;
    }
};

#endif // __HEAP_H__
