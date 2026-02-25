

#include <iostream>
#include <vector>
#include "node.h"

using namespace std;

int main() {
    cout << "1. Add nodes to the network using the join function, m = 8" << std::endl;
    Node n0(0);
    Node n1(30);
    Node n2(65);
    Node n3(110);
    Node n4(160);
    Node n5(230);

    n0.join(NULL);
    n1.join(&n0);
    n2.join(&n1);
    n3.join(&n2);
    n4.join(&n3);
    n5.join(&n4);

    cout << "\n2. Print finger table of all nodes" << std::endl;
    n0.printFingerTable();
    n1.printFingerTable();
    n2.printFingerTable();
    n3.printFingerTable();
    n4.printFingerTable();
    n5.printFingerTable();

    cout << "\n3. Insert keys and add new node joins" << std::endl;
    n0.insert(3, 3);
    n1.insert(200, 0); 
    n2.insert(123, 0);
    n3.insert(45, 3);
    n4.insert(99, 0);
    n2.insert(60, 10);
    n0.insert(50, 8);
    n3.insert(100, 5);
    n3.insert(101, 4);
    n3.insert(102, 6);
    n5.insert(240, 8);
    n5.insert(250, 10);

    cout << "\n3.1 print keys that stored in each node" << std::endl;
    n0.printKeys();
    n1.printKeys();
    n2.printKeys();
    n3.printKeys();
    n4.printKeys();
    n5.printKeys();

    cout << "\nn6 (id: 100) joins" << std::endl;
    Node n6(100);
    n6.join(&n3);

    cout << "\n4. Lookup keys" << std::endl;
    std::vector<uint8_t> keys_to_lookup = {3, 200, 123, 45, 99, 60, 50, 100, 101, 102, 240, 250};
    
    cout << "--- Lookup from n0 ---" << std::endl;
    for (uint8_t k : keys_to_lookup) n0.find(k);
    
    cout << "--- Lookup from n2 ---" << std::endl;
    for (uint8_t k : keys_to_lookup) n2.find(k);

    cout << "--- Lookup from n6 ---" << std::endl;
    for (uint8_t k : keys_to_lookup) n6.find(k);

    cout << "\n5. Leave" << std::endl;
    cout << "Let one node n2 leave, print the updated finger tables of n0 and n1, and keys distribution" << endl;
    n2.leave();
    
    n0.printFingerTable();
    n1.printFingerTable();
    
    n0.printKeys();
    n1.printKeys();
    n6.printKeys();
    n3.printKeys();
    n4.printKeys();
    n5.printKeys();
    
    return 0;
}
