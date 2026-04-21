#include "linkedlist.h"
#include <fstream>

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

    // T1: Copy constructor
    cout << "\n--- Test Copia ---" << endl;
    LinkedList<DescendingLinkedListTrait<T1>> copia(list); 
    cout << "Size original: " << list.size() << " | Size copia: " << copia.size() << endl;
    cout << "Copia Item 0: " << copia[0] << " (Debe ser 100)" << endl;

    //T2: Move constructor
    cout << "\n--- Test Move ---" << endl;
    size_t old_size = list.size();
    LinkedList<DescendingLinkedListTrait<T1>> listaMovida(std::move(list)); 
    cout << "Size lista movida: " << listaMovida.size() << " (Esperado: " << old_size << ")" << endl;
    cout << "Size original tras move: " << list.size() << " (Esperado: 0)" << endl;

    // T11: Test del operador << (toString)
    cout << "\n--- Test Operador << ---" << endl;
    cout << "Contenido de listaMovida: " << listaMovida << endl;

    // T14: Test del operador >>
    cout << "\n--- Test Lectura Operador >> desde Archivo (testOperator.txt) ---" << endl;
    
    ifstream file("testOperator.txt");
    if (file.is_open()) {
        LinkedList<DescendingLinkedListTrait<T1>> listFromFile;
        file >> listFromFile;
        cout << "Contenido leido del archivo: " << listFromFile << endl;
        file.close();
    } else {
        cout << "No se encontro 'testOperator.txt' para la prueba de archivo." << endl;
    }
}

void ListsDemo(){
    LinkedListDemo();

}
