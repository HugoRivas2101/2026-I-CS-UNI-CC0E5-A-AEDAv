#ifndef BTREE_ITERATOR_H
#define BTREE_ITERATOR_H

#include "general_iterator.h"

template <typename Container>
class btree_forward_iterator : public general_iterator<Container, btree_forward_iterator<Container>> {
public:
    using MySelf     = btree_forward_iterator<Container>;
    using Parent     = general_iterator<Container, MySelf>;
    using ObjectInfo = typename Container::ObjectInfo;
    using Parent::Parent;
    MySelf      operator++() { this->m_pNode++; return *this; }
    ObjectInfo& operator*()  { return *this->m_pNode; }
};

template <typename Container>
class btree_backward_iterator : public general_iterator<Container, btree_backward_iterator<Container>> {
public:
    using MySelf     = btree_backward_iterator<Container>;
    using Parent     = general_iterator<Container, MySelf>;
    using ObjectInfo = typename Container::ObjectInfo;
    using Parent::Parent;
    MySelf      operator++() { this->m_pNode--; return *this; }
    ObjectInfo& operator*()  { return *this->m_pNode; }
};

#endif // BTREE_ITERATOR_H