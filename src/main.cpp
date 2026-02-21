#include <iostream>
#include "node.h"

int main() {
    // Example from the PDF
    Node n0(5); // node_id == 5
    Node n1(63); // node_id == 63
    
    std::cout << "--- n0 joins (first node) ---" << std::endl;
    n0.join(NULL); // the first node to join the Chord network.
    
    std::cout << "\n--- n0 inserts (3, 3) ---" << std::endl;

    n0.insert(3, 3); // insert key = 3
    
    std::cout << "\n--- n1 joins (contacts n0) ---" << std::endl;
    n1.join(&n0); // the second node join the Chord network.
    
    std::cout << "\n--- n1 inserts (5) ---" << std::endl;
    // PDF says n1.insert(5), but insert requires 2 args. 
    n1.insert(5, 50); 
    
    std::cout << "\n--- n0 finds (5) ---" << std::endl;
    uint8_t val = n0.find(5); // key query
    std::cout << "Found value: " << (int)val << std::endl;

    return 0;
}
