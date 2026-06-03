#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>

#include "../types.h"
#include "hashtable.h"

using namespace std;

void HashTableDemo(){
    cout << "\n=== HashTable (AVL) Demo ===" << endl;

    HashTable<AscendingTrait<AVLNode<T1>>> m;

    // operator[]: m[key] = value
    m[28] = 15;
    m[17] = 25;
    m[35] = 10;
    m[10] = 30;
    m[20] = 5;
    m[30] = 20;
    m[40] = 15;

    cout << "Mapa (operator<<): " << m << endl;

    m[20] = 999;   // actualiza
    m[5]  = 3;     // inserta
    cout << "Tras m[20]=999 y m[5]=3: " << m << endl;

    // Recorrido con structured bindings
    cout << "Recorrido [key, value] (claves ordenadas):" << endl;
    for (const auto& [key, value] : m) {
        cout << "  " << key << " -> " << value << endl;
    }

    // Persistencia con operator>> / operator<<
    cout << "Cargando desde texto '{1: 100, 2: 200, 3: 300}':" << endl;
    HashTable<AscendingTrait<AVLNode<T1>>> m2;
    stringstream ss("{1: 100, 2: 200, 3: 300}");
    ss >> m2;
    cout << "Mapa reconstruido: " << m2 << endl;

    // Constructor copia (independiente del original)
    HashTable<AscendingTrait<AVLNode<T1>>> mCopia(m);
    mCopia[5] = 777;   // no afecta al original
    cout << "Copia (m[5] cambiado a 777): " << mCopia << endl;
    cout << "Original (m[5] sigue en 3):  " << m << endl;

    // Move constructor
    HashTable<AscendingTrait<AVLNode<T1>>> mMovido(std::move(m2));
    cout << "Movido desde m2: " << mMovido << endl;
}