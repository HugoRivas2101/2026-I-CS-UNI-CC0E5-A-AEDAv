#include <iostream>
#include <string>
#include "BTree.h"
using namespace std;

template <typename Node>
bool FirstGreaterEqual(Node &info, size_t /*level*/, char pivote){
    return info.getData() >= pivote;
}

void BTreeDemo(){
    const char *keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
    const size_t BTreeOrder = 3;

    BTree<BTreeTrait<char>> bt(BTreeOrder);
    for(size_t i = 0; keys1[i]; ++i)
        bt.Insert(keys1[i], Ref(i * i));

    cout << "=== BTree Demo ===\n";
    cout << "Insertadas " << bt.size() << " claves, altura = " << bt.height()
         << ", orden = " << bt.GetOrder() << "\n\n";

    cout << "Arbol (inorden, con sangria por nivel):\n";
    bt.Print(cout);

    cout << "\nSearch('K') -> Ref = " << bt.Search('K') << "\n";
    cout << "Search('@') -> Ref = " << bt.Search('@') << " (no existe, espera -1)\n";

    size_t digitos = 0;
    bt.ForEach([](BTreeNode<BTreeTrait<char>> &info, size_t /*level*/, size_t *contador){
        if(info.getData() >= '0' && info.getData() <= '9')
            (*contador)++;
    }, &digitos);
    cout << "\nForEach: cantidad de claves que son digito = " << digitos << "\n";

    auto *p = bt.FirstThat(FirstGreaterEqual<BTreeNode<BTreeTrait<char>>>, 'a');
    if(p)
        cout << "FirstThat: primera clave >= 'a' es '" << p->getData()
             << "' (Ref " << p->getRef() << ")\n";
    else
        cout << "FirstThat: no hay clave >= 'a'\n";
}
