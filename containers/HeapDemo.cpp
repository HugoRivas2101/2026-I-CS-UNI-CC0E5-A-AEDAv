#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>

#include "../types.h"
#include "heap.h"

using namespace std;

void DemoMinHeap(){
    cout << "\n=== Min Heap Demo ===" << endl;

    Heap<MinHeapTrait<T1>> heap;
    heap.insert(28, 15);
    heap.insert(17, 25);
    heap.insert(35, 10);
    heap.insert(10, 30);
    heap.insert(20, 5);
    heap.insert(30, 20);
    heap.insert(40, 15);

    cout << "Arreglo interno (operator<<): " << heap << endl;
    cout << "Tamano: " << heap.size() << endl;

    auto [pdata, pref] = heap.peek();
    cout << "peek() (cima, sin remover): (" << pdata << "," << pref << ")" << endl;

    cout << "Extrayendo en orden (esperado ascendente): ";
    while(!heap.isEmpty()){
        auto [data, ref] = heap.extract();
        cout << "(" << data << "," << ref << ") ";
    }
    cout << endl;

    // Persistencia con operator>> / operator<<
    cout << "Cargando desde texto '[(5,50),(1,10),(3,30),(2,20)]':" << endl;
    Heap<MinHeapTrait<T1>> heap2;
    stringstream ss("[(5,50),(1,10),(3,30),(2,20)]");
    ss >> heap2;
    cout << "Heap reconstruido: " << heap2 << endl;
    auto [d2, r2] = heap2.peek();
    cout << "Cima del MinHeap reconstruido: (" << d2 << "," << r2 << ")" << endl;
}

void DemoMaxHeap(){
    cout << "\n=== Max Heap Demo ===" << endl;

    Heap<MaxHeapTrait<T1>> heap;
    heap.insert(28, 15);
    heap.insert(17, 25);
    heap.insert(35, 10);
    heap.insert(10, 30);
    heap.insert(20, 5);
    heap.insert(30, 20);
    heap.insert(40, 15);

    cout << "Arreglo interno (operator<<): " << heap << endl;

    auto [pdata, pref] = heap.peek();
    cout << "peek() (cima, sin remover): (" << pdata << "," << pref << ")" << endl;

    cout << "Extrayendo en orden (esperado descendente): ";
    while(!heap.isEmpty()){
        auto [data, ref] = heap.extract();
        cout << "(" << data << "," << ref << ") ";
    }
    cout << endl;
}