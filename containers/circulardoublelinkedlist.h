#pragma once
#include "doublelinkedlist.h"

template <typename Trait>
class CircularDoubleLinkedList : public DoubleLinkedList<Trait> {
public:
    using MySelf     = CircularDoubleLinkedList<Trait>;
    using Base       = DoubleLinkedList<Trait>;
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;

    CircularDoubleLinkedList() : Base() {}

    // Destructor
    virtual ~CircularDoubleLinkedList() {
        std::unique_lock<std::shared_mutex> lock(this->m_mtx);
        if (this->m_pRoot && this->m_tail) {
            this->m_tail->setNext(nullptr);
            this->m_pRoot->setPrev(nullptr);
        }
    }

    // DONE: PUSH BACK
    void push_back(value_type value, Ref ref) override {
        Base::push_back(value, ref);
        close_ring();
    }

    // DONE: PUSH FRONT
    void push_front(value_type value, Ref ref) override {
        Base::push_front(value, ref);
        close_ring();
    }

    void insert(const value_type &value, Ref ref) override {
        std::unique_lock<std::shared_mutex> lock(this->m_mtx);
        
        // Rompemos temporalmente el anillo
        if (this->m_tail) this->m_tail->setNext(nullptr);
        
        // Invocamos la lógica de la base (internal_insert)
        this->internal_insert(this->m_pRoot, nullptr, value, ref);
        
        // Re-cerramos el anillo
        if (this->m_pRoot && this->m_tail) {
            this->m_tail->setNext(this->m_pRoot);
            this->m_pRoot->setPrev(this->m_tail);
        }
    }

    // DONE: POP BACK
    std::tuple<value_type, Ref> pop_back() override {
        auto result = Base::pop_back();
        close_ring();
        return result;
    }

    // DONE: POP FRONT
    std::tuple<value_type, Ref> pop_front() override {
        auto result = Base::pop_front();
        close_ring();
        return result;
    }

    // DONE: OPERADOR <<
    // Se usa m_size para evitar el bucle infinito al imprimir
    friend std::ostream& operator<<(std::ostream& os, const CircularDoubleLinkedList& list) {
        std::shared_lock<std::shared_mutex> lock(list.m_mtx);
        os << "[Circular: ";
        if (list.m_pRoot) {
            Node* curr = list.m_pRoot;
            for (size_t i = 0; i < list.m_size; ++i) {
                os << "(" << curr->getData() << "," << curr->getRef() << ")";
                if (i < list.m_size - 1) os << ", ";
                curr = curr->getNext();
            }
        }
        os << "]";
        return os;
    }

private:
    // Mantiene la integridad circular
    void close_ring() {
        if (this->m_pRoot && this->m_tail) {
            this->m_tail->setNext(this->m_pRoot);
            this->m_pRoot->setPrev(this->m_tail);
        }
    }
};
