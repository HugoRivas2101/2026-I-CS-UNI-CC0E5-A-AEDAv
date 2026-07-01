//#include <iostream.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include "BTree.h"
#include "..\types.h"
#include <fstream>

//const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const KeyType * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const KeyType * keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const KeyType * keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

const T1 BTreeSize = 3;

template <typename Trait>
void ExecuteBTreeTest(const string& typeLabel)
{

        cout<<"====== PRUEBA CON TRAIT "<<typeLabel<<" ======"<<endl;

       T1 i;
       BTree<Trait> bt(BTreeSize);

       for (i = 0; keys1[i]; i++)
       {
               //cout<<"Inserting "<<keys1[i]<<endl;
               bt.Insert(keys1[i], Ref(i*i));
               //bt.Print(cout);
       }
       cout << "size=" << bt.size() << " height=" << bt.height() << endl;
       //bt.Print(cout);

       for (i = 0; keys2[i]; i++)
       {
               cout << "Searching " << keys2[i] << " ";
               long ObjID = bt.Search(keys2[i]);
               if( ObjID != -1 )
                       cout << "Encontrado " << keys2[i] << " ID = " << ObjID << endl;
               else
                       cout <<"No encontrado" << keys2[i] << endl;
       }
       cout.flush();

       for (i = 0; keys3[i]; i++)
       {
               cout << "Removing " << keys3[i] << " ";
               if( bt.Remove(keys3[i], -1) )
                       cout << keys3[i] << " removido !" << endl;
               else
                       cout <<"No encontrado" << keys3[i] << endl;
               //bt.Print(cout);
               cout<<bt;
       }
       //bt.Print(cout);
       cout.flush();
}

void BTreeOperatorsDemo(){
        BTree<AscendingBTreeTrait<KeyType>> bt(3);
        
        // operator>>: lectura desde archivo
        ifstream  ss("btree_test.txt");
        while(ss >> bt) {}
        ss.close();

        // operator<<
        cout << "Arbol insertado via operador >>:" << endl;
        cout << bt << endl;

        // operator<<
        ofstream salida("btree_salida.txt");
        salida << bt;
        salida.close();
        cout << "Arbol guardado en btree_salida.txt" << endl;
}

void BTreeUnifiedDemo(){
    
        using BT    = BTree<AscendingBTreeTrait<KeyType>>;
        using Entry = BT::ObjectInfo;

        BT bt(3);
        const KeyType* keys="ABCDEFGHIJ";
        for(T1 i=0; keys[i]; i++) bt.Insert(keys[i], Ref(i*10));

        // ForEach: Recorre todos los elementos
        cout<<"Todos los elementos: "<<endl;
        bt.ForEach([](Entry& info, ostream* os){
                *os<<info.key<<"("<<info.ObjID<<")";
        }, &cout);
        cout<<endl;

        // ForEach modo FirstThat: retorna Entry* al encontrar
        KeyType target = 'E';
        cout<<"Buscando "<<target<<" (FirstThat): "<<endl;
        auto found = bt.ForEach([](Entry& info, KeyType* t) -> Entry*{
                return (info.key == *t) ? &info : nullptr;
        }, &target);

        if(found) cout << "Encontrado: " << found->key << " ID=" << found->ObjID << endl;
        else cout<<"No encontrado"<<endl;
}

void BTreeDemo()
{
        BTreeOperatorsDemo();
        BTreeUnifiedDemo();
        ExecuteBTreeTest<AscendingBTreeTrait<KeyType>>("ASCENDENTE");
        ExecuteBTreeTest<DescendingBTreeTrait<KeyType>>("DESCENDENTE");
}








/*const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const char * keys2="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const int BTreeSize = 3;
main (int argc, char * argv)
{
       //__int64 li;
       BTree <__int64> bt (BTreeSize);
       for (register int i = 0; i < 1000000; i++)
       {
               //cout<<"Inserting "<<keys[i]<<endl;
               bt.Insert(i, i-1);
               //bt.Print(cout);
       }

       for (i = 0; i < 1000; i++)
       {
               __int64 key = 975000+(::rand()%50000);
               //cout << "Searching " << (long)key << " ";
               long ObjID = bt.Search(key);
               if( ObjID != -1 )
                       cout << "Achei " << (long)key << " ID = " << ObjID << endl;
               else
                       cout <<"  Nao achei!" << (long)key << endl;
       }
       cout.flush();

       return 1;
}*/



/*const int BTreeSize = 3;
main (int argc, char * argv)
{
       int result, i;
       BTree <LONGLONG> bt(BTreeSize);
       result = bt.Create ("ernesto3-string-btree-start.dat",ios::in|ios::out);
       if (!result) { cout<<"Please delete testbt.dat"<<endl;return 0; }
       srand( (unsigned)time( NULL ) );
       LARGE_INTEGER key;
       for (i = 0; i < 1000000; i++)
       {
               //cout<<"Inserting "<<keys[i]<<endl;
               char strTmp[50];
               key.LowPart = rand();
               key.HighPart = rand();
               std::string str(strTmp);
               result = bt.Insert(key.QuadPart, i);
               //bt.Print(cout);
               if( i % 100000 == 0 )
               {       cout << i << endl; cout.flush();        }
       }
       //cout << "Searching D " << bt.Search();
       //bt.Search(1,1);
       cout.flush();
       return 1;
}*/
