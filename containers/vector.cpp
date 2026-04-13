#include <cstddef>
#include <iostream>
#include <string>
#include <fstream>
#include "vector.h"

using namespace std;

void DemoVector(){
    Vector<T1> v1(10);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(-1);
    v1.push_back(4);
    cout << v1.toString() << endl;
    cout << v1 << endl;
    // cout << "hola" << 5 << endl;
    // cout.operator<<("hola")
    // ==============
    //           cout << 5 << endl;
    //           =========
    //                cout << endl;

    Vector<string> v2(10);
    v2.push_back("Hola");
    v2.push_back("Mundo");
    v2.push_back("!");
    cout << v2 << endl;
    cout << v2.toString() << endl;

    ofstream of("temp.txt");
    of << v1 << endl;
    of << v2 << endl;
    of.close();
    
    // READ FROM FILE
    ifstream ifs("tempRead.txt");
    Vector<string> v4(10);
    ifs >> v4;
    cout << "\nread vector: " << v4 << endl;
    cout << "first element: " << v4.get(0) << endl;
    cout << "second element: " << v4.get(1) << endl;
    cout << "size: " << v4.size() << endl;
}