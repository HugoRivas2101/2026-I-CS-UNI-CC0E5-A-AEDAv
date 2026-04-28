#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include "../types.h"
#include "linkedlist.h"
#include "doublelinkedlist.h"
#include "circularlinkedlist.h"
#include "circulardoublelinkedlist.h"

using namespace std;

void printHeader(string title) {
    cout << "\n--- TEST: " << title << " ---" << endl;
}

void DoubleLinkedListDemo() {
    cout << "=== PRUEBAS DE DOUBLE LINKED LIST ===" << endl;

    // Inserción Ordenada
    printHeader("INSERCION ORDENADA");
    DoubleLinkedList<AscendingDLLTrait<int>> listAsc;
    listAsc.insert(30, 1); listAsc.insert(10, 2); listAsc.insert(20, 3);
    cout << "Ascendente (esperado 10, 20, 30): " << listAsc << endl;

    DoubleLinkedList<DescendingDLLTrait<int>> listDesc;
    listDesc.insert(10, 1); listDesc.insert(30, 2); listDesc.insert(20, 3);
    cout << "Descendente (esperado 30, 20, 10): " << listDesc << endl;

    // Push/Pop Front & Back
    printHeader("PUSH/POP EXTREMOS");
    DoubleLinkedList<AscendingDLLTrait<int>> listPB;
    listPB.push_back(100, 10); 
    listPB.push_front(50, 5);
    cout << "Lista antes de Pop: " << listPB << endl;
    
    auto [valF, refF] = listPB.pop_front();
    cout << "Valor extraido Front: " << valF << " (Ref: " << refF << ")" << endl;
    auto [valB, refB] = listPB.pop_back();
    cout << "Valor extraido Back: " << valB << " (Ref: " << refB << ")" << endl;
    cout << "Lista despues de Pops (debe estar vacia): " << listPB << endl;

    // Acceso por Índice
    printHeader("ACCESO POR INDICE (Operador [])");
    DoubleLinkedList<AscendingDLLTrait<int>> listIdx;
    for(int i=0; i<6; i++) listIdx.push_back(i*10, i); 
    cout << "Lista completa: " << listIdx << endl;
    cout << "Indice 1 (Cerca inicio): " << listIdx[1] << endl;
    cout << "Indice 4 (Cerca final): " << listIdx[4] << endl;

    // Copy & Move
    printHeader("COPY & MOVE");
    DoubleLinkedList<AscendingDLLTrait<int>> copyList = listIdx;
    cout << "Lista Copiada: " << copyList << " | Size: " << copyList.size() << endl;

    DoubleLinkedList<AscendingDLLTrait<int>> moveList = std::move(copyList);
    cout << "Lista Movida: " << moveList << " | Size: " << moveList.size() << endl;
    cout << "Lista Original (tras move, size 0): " << copyList.size() << endl;

    // Concurrencia
    printHeader("CONCURRENCIA");
    DoubleLinkedList<AscendingDLLTrait<int>> concurrentList;
    vector<thread> workers;
    for(int i = 0; i < 4; i++) {
        workers.emplace_back([&concurrentList]() {
            for(int j = 0; j < 250; j++) concurrentList.push_back(j, 0);
        });
    }
    for(auto& t : workers) t.join();
    cout << "Elementos insertados por 4 hilos: " << concurrentList.size() << endl;
    cout << "Estado Concurrencia: " << (concurrentList.size() == 1000 ? "CORRECTO" : "ERROR") << endl;
}

// DemoList para persistencia de archivos
template <typename Container>
void DemoList(Container& list, string fileName){
    list.insert(28, 15);
    list.insert(17, 25);
    list.insert(8, 35);
    
    ofstream os(fileName);
    os << list << endl;

    ifstream is(fileName);
    is >> list;
    cout << "Lectura/Escritura en " << fileName << " completada." << endl;
}

void ListsDemo(){
    DoubleLinkedListDemo();
    cout << "\n=== PRUEBAS FINALIZADAS ===" << endl;
}