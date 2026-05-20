#pragma once
#include "linkedlist.h"

template <typename Trait>
class CircularLinkedList : public LinkedList<Trait> {
public:
    using Base       = LinkedList<Trait>;
    using value_type = typename Trait::value_type;
    using Node       = typename Trait::Node;

    CircularLinkedList() : Base() {}

    // Destructor: Se rompe el ciclo
    virtual ~CircularLinkedList() {
        std::unique_lock<std::shared_mutex> lock(this->m_mtx);
        if (this->m_tail) {
            this->m_tail->setNext(nullptr);
        }
    }

    // DONE: Push front
    void push_front(value_type value, Ref ref) override {
        Base::push_front(value, ref);
        close_ring();
    }

    // DONE: Push back
    void push_back(value_type value, Ref ref) override {
        Base::push_back(value, ref);
        close_ring();
    }

    // DONE: Insert
    void insert(const value_type &value, Ref ref) override {
        std::unique_lock<std::shared_mutex> lock(this->m_mtx);
        
        if (this->m_tail) this->m_tail->setNext(nullptr);
        
        this->internal_insert(this->m_pRoot, value, ref);
        
        if (this->m_pRoot && this->m_tail) {
            this->m_tail->setNext(this->m_pRoot);
        }
    }

    // DONE: Pop front
    std::tuple<value_type, Ref> pop_front() override {
        auto result = Base::pop_front();
        close_ring();
        return result;
    }

    // DONE: Pop back
    std::tuple<value_type, Ref> pop_back() override {
        std::unique_lock<std::shared_mutex> lock(this->m_mtx);
        if (!this->m_pRoot) throw std::runtime_error("Lista vacia");

        std::tuple<value_type, Ref> result;
        
        if (this->m_pRoot == this->m_tail) {
            result = std::make_tuple(this->m_pRoot->getData(), this->m_pRoot->getRef());
            delete this->m_pRoot;
            this->m_pRoot = this->m_tail = nullptr;
        } else {
            Node* act = this->m_pRoot;
            while (act->getNext() != this->m_tail) {
                act = act->getNext();
            }
            result = std::make_tuple(this->m_tail->getData(), this->m_tail->getRef());
            delete this->m_tail;
            this->m_tail = act;
            this->m_tail->setNext(this->m_pRoot); 
        }
        this->m_size--;
        return result;
    }

    // DONE: Operador <<
    friend std::ostream& operator<<(std::ostream& os, const CircularLinkedList& list) {
        std::shared_lock<std::shared_mutex> lock(list.m_mtx);
        os << "[Circular Simple: ";
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

    // ROTATE
    void rotate(int steps) {
    if (this->m_size <= 1 || steps == 0) return;

    std::unique_lock<std::shared_mutex> lock(this->m_mtx);
    
    steps = steps % this->m_size;
    if (steps < 0) steps += this->m_size;

    for (int i = 0; i < steps; ++i) {
        this->m_pRoot = this->m_pRoot->getNext();
        this->m_tail = this->m_tail->getNext();
    }
}

private:
    void close_ring() {
        if (this->m_pRoot && this->m_tail) {
            this->m_tail->setNext(this->m_pRoot);
        }
    }
};