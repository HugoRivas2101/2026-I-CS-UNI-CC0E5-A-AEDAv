// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "BTreePage.h"
#include <mutex>
#include <shared_mutex> 
#include "BTree_iterator.h"
#include <type_traits>
#include "util.h"
#include "../types.h"


#define DEFAULT_BTREE_ORDER 3

template <typename Trait>
class BTree
// this is the full version of the BTree
{
       typedef CBTreePage<Trait> BTNode;// useful shorthand

public:
       using value_type = typename Trait::value_type;
       using ObjIDType  = typename Trait::ObjIDType;
       using ObjectInfo = typename BTNode::ObjectInfo;
       using forward_iterator = btree_forward_iterator<BTree<Trait>>;
       using backward_iterator = btree_backward_iterator<BTree<Trait>>;
       using Node = ObjectInfo;

protected:
       mutable shared_mutex m_mtx;

public:
       BTree(T1 order = DEFAULT_BTREE_ORDER, bool unique = true);
       ~BTree();
       bool            Insert (const value_type key, const ObjIDType ObjID);
       bool            Remove (const value_type key, const ObjIDType ObjID);
       ObjIDType       Search (const value_type key);
       long            size()  { return m_NumKeys; }
       long            height() { return m_Height;      }

       forward_iterator  begin()  { buildIterBuffer(); return forward_iterator (this, m_iterBuffer.data()); }
       forward_iterator  end()    { return forward_iterator (this, m_iterBuffer.data() + m_iterBuffer.size()); }
       backward_iterator rbegin() { buildIterBuffer(); return backward_iterator(this, m_iterBuffer.data() + m_iterBuffer.size() - 1); }
       backward_iterator rend()   { return backward_iterator(this, m_iterBuffer.data() - 1); }

       long            GetOrder() { return m_Order;     }

       friend ostream& operator<<(ostream& os, BTree<Trait>& bt) {
              shared_lock<shared_mutex> lock(bt.m_mtx);
              bt.ForEach([](ObjectInfo& info, ostream* pOs){
                     *pOs << info.key << "->" << info.ObjID << "\n";
              }, &os);
              
              return os;
       }

       friend istream& operator>>(istream& is, BTree<Trait>& bt) {
              unique_lock<shared_mutex> lock(bt.m_mtx);
              value_type key;
              ObjIDType  ref;
              if(is >> key >> ref) bt.Insert(key, ref);
              return is;
       
       }

       template <typename Func, typename... Args>
       decltype(auto) ForEach(Func func, Args&&... args);

       void buildIterBuffer() const;

protected:
       BTNode          m_Root;
       mutable vector<ObjectInfo> m_iterBuffer;
       T1             m_Height;  // height of tree
       T1             m_Order;   // order of tree
       long            m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
};

const T1 MaxHeight = 5;
template <typename Trait>
BTree<Trait>::BTree(T1 order, bool unique)
                               : m_Root(2 * order  + 1, unique),
                                 m_Order(order),
                                 m_NumKeys(0),
                                 m_Unique(unique)
                                                                  
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

template <typename Trait>
BTree<Trait>::~BTree()
{
}

template <typename Trait>
bool BTree<Trait>::Insert(const value_type key, const ObjIDType ObjID)
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
bool BTree<Trait>::Remove (const value_type key, const ObjIDType ObjID)
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
typename BTree<Trait>::ObjIDType BTree<Trait>::Search (const value_type key)
{
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename Trait>
template <typename Func, typename... Args>
decltype(auto) BTree<Trait>::ForEach(Func func, Args&&... args)
{
    for(auto it= begin(); it!=end(); ++it)
    {
       if constexpr(is_void_v<invoke_result_t<Func, ObjectInfo&, Args...>>)
                 invoke(func, *it, forward<Args>(args)...);
       else
           if(auto ret = invoke(func, *it, forward<Args>(args)...)) return ret;              
    }
    if constexpr(!is_void_v<invoke_result_t<Func, ObjectInfo&, Args...>>){
       return invoke_result_t<Func, ObjectInfo&, Args...>{};
    }
}

// ELIMINADO FIRSTHAT. UNIFICADO EN FOREACH

// PRINT HELPER ELIMINADO

template <typename Trait>
void BTree<Trait>::buildIterBuffer() const
{
    m_iterBuffer.clear();
    const_cast<BTree<Trait>*>(this)->m_Root.ForEach(
        [](ObjectInfo& info, T1 level, vector<ObjectInfo>* buf){ buf->push_back(info);},
        0, // nivel inicial = raiz
        &m_iterBuffer
    );
}

#endif