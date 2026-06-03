#include "containers/vector.h"
#include "containers/linkedlist.h"
#include "containers/binarytree.h"
#include "containers/avltree.h"
#include "containers/heap.h"
#include "containers/hashtable.h"

// g++ -std=c++2b main.cpp containers/vector.cpp -o main
void ListsDemo();
void BinaryTreeDemo();
void AvlTreeDemo();
void DemoMinHeap();
void DemoMaxHeap();
void HashTableDemo();

int main(){
    // DemoVector();
    //DemoConcurrentVector();
    //ListsDemo();
    BinaryTreeDemo();
    AvlTreeDemo();
    DemoMinHeap();
    DemoMaxHeap();
    HashTableDemo();
    return 0;
}