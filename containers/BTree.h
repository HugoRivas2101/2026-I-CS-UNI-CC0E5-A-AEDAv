// BTree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <functional>   // less
#include <utility>      // std::forward
#include "BTreePage.h"
#include "../types.h"   // Ref, T1

#define DEFAULT_BTREE_ORDER 3

template <typename T>
struct BTreeTrait
{
       using value_type = T;
       using Comp       = less<T>;
       using Node       = BTreeNode<BTreeTrait<T>>;
};

template <typename Trait>
class BTree
// this is the full version of the BTree
{
       using value_type = typename Trait::value_type;
       using Node       = typename Trait::Node;
       typedef CBTreePage<Trait> BTNode;              // useful shorthand

public:
       BTree(size_t order = DEFAULT_BTREE_ORDER, bool unique = true);
       virtual ~BTree();

       bool            Insert (const value_type key, const Ref ObjID);
       bool            Remove (const value_type key, const Ref ObjID);
       Ref             Search (const value_type key);
       size_t          size()     { return m_NumKeys; }
       size_t          height()   { return m_Height;  }
       size_t          GetOrder() { return m_Order;   }

       void            Print (ostream &os);

       template <typename Func, typename... Args>
       void            ForEach  (Func func, Args &&... args);
       template <typename Func, typename... Args>
       Node           *FirstThat(Func func, Args &&... args);

protected:
       BTNode          m_Root;
       size_t          m_Height;  // height of tree
       size_t          m_Order;   // order of tree
       size_t          m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
};

template <typename Trait>
BTree<Trait>::BTree(size_t order, bool unique)
                               : m_Root(T1(2 * order + 1), unique),
                                 m_Height(1),
                                 m_Order(order),
                                 m_NumKeys(0),
                                 m_Unique(unique)
{
       m_Root.SetMaxKeysForChilds(T1(order));
}

template <typename Trait>
BTree<Trait>::~BTree()
{
}

template <typename Trait>
bool BTree<Trait>::Insert(const value_type key, const Ref ObjID)
{
       bt_ErrorCode error = m_Root.Insert(key, ObjID);
       if( error == bt_duplicate )
               return false;
       m_NumKeys++;
       if( error == bt_overflow )
       {
               m_Root.SplitRoot();
               m_Height++;
       }
       return true;
}

template <typename Trait>
bool BTree<Trait>::Remove (const value_type key, const Ref ObjID)
{
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename Trait>
Ref BTree<Trait>::Search (const value_type key)
{
       Ref ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}

template <typename Trait>
template <typename Func, typename... Args>
void BTree<Trait>::ForEach(Func func, Args &&... args)
{
       m_Root.ForEach(func, 0, std::forward<Args>(args)...);
}

template <typename Trait>
template <typename Func, typename... Args>
typename BTree<Trait>::Node *
BTree<Trait>::FirstThat(Func func, Args &&... args)
{
       return m_Root.FirstThat(func, 0, std::forward<Args>(args)...);
}

template <typename Trait>
void BTree<Trait>::Print(ostream &os){
       m_Root.Print(os);
}

#endif
