#include "linkedlist.h"

void LinkedListDemo(){
    LinkedList<DescendingLinkedListTrait<T1>> list;
    
    list.insert(5, 1);
    list.insert(10, 2);
    list.insert(15, 3);
    list.insert(20, 4);
    //list.insert(5, 55);
    cout<<"Test Size (4): "<<list.size()<<endl;

    // T5: Push front
    list.push_front(100, 10); // [100, 20, 15, 10, 5]
    list.push_front(200, 20); // [200, 100, 20, 15, 10, 5]
    cout << "T5 (Push front) - Size esperado 6: " << list.size() << endl;

    // T6: Pop front
    list.pop_front(); // [100, 20, 15, 10, 5]
    cout << "T6 (Pop front) - Size esperado 5: " << list.size() << endl;

    // T7: Push back
    list.push_back(300, 30); // [100, 20, 15, 10, 5, 300]
    cout << "T7 (Push back) - Size esperado 6: " << list.size() << endl;

    // T8: Pop back
    list.pop_back(); // [100, 20, 15, 10, 5]
    cout << "T8 (Pop back) - Size esperado 5: " << list.size() << endl;

    // T9: Indexing
    cout << "T9 (Indexing) - Item 0: " << list[0] << " (Expected 100)" << endl;
    cout << "T9 (Indexing) - Item 4: " << list[4] << " (Expected 5)" << endl;
}

void ListsDemo(){
    LinkedListDemo();

}
