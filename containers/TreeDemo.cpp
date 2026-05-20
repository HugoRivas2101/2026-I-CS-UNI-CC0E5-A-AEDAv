#include <iostream>
#include <fstream>
#include <thread>

#include "../types.h"
#include "binarytree.h"
#include "avltree.h"

using namespace std;

void BinaryTreeDemo(){
    
    cout << "\n=== Binary Tree Demo ===" << endl;

    BinaryTree<AscendingTrait<BinaryTreeNode<T1>>> tree;
    tree.insert(28, 15);
    tree.insert(17, 25);
    tree.insert(35, 10);
    tree.insert(10, 30);
    tree.insert(20, 5);
    tree.insert(30, 20);
    tree.insert(40, 15);
    
    cout << "Impresión con operador << : ";
    cout << tree << endl;
    
    // Persistencia Escritura
    ofstream os("TreeDemo.txt");
    os << tree;
    os.close();
    cout<<endl;

    // Persistencia Lectura
    cout << "Cargando desde TreeDemo.txt" << endl;
    BinaryTree<AscendingTrait<BinaryTreeNode<T1>>> tree2;
    ifstream is("TreeDemo.txt");
    is >> tree2;
    is.close();
    cout<< tree2 << endl;

    // Usando el bucle for
    cout << "Impresión con for-each: ";
    for (int val : tree) {
        cout << val << " ";
    }
    cout << endl;


    cout << "Preorder: ";
    for (int val : tree.preorder()) {
        cout << val << " ";
    }
    cout << endl;

    cout << "Postorder: ";
    for (int val : tree.postorder()){
        cout << val << " ";
    }
    cout << endl;

    // Búsqueda
    cout << "Busqueda: " << endl;
    cout << "¿Existe 20? " << (tree.find(20) ? "Sí" : "No") << endl;
    cout << "¿Existe 99? " << (tree.find(99) ? "Sí" : "No") << endl;

}

void AvlTreeDemo() {
    cout << "\n=== AVL Demo ===" << endl;

    AVLTree<AscendingTrait<AVLNode<int>>> avl;
    avl.insert(28, 15);
    avl.insert(17, 25);
    avl.insert(35, 10);
    avl.insert(10, 30);
    avl.insert(20, 5);
    avl.insert(30, 20);
    avl.insert(40, 15);

    cout << "AVL inorder (elementos balanceados): " << avl << endl;
    cout << "Altura: " << avl.height() << " (esperado <= 3)" << endl;
}