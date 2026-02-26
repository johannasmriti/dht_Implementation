

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "node.h"

using namespace std;

void printIdentifierCircle(const vector<Node*>& nodes) {
    const int height = 18;
    const int width = 50;
    vector<string> grid(height, string(width, ' '));
    
    float centerX = width / 2.0;
    float centerY = height / 2.0;
    float radiusX = width * 0.4;
    float radiusY = height * 0.4;

    // Draw circle border
    for (float a = 0; a < 2 * 3.14159; a += 0.05) {
        int x = (int)(centerX + radiusX * cos(a));
        int y = (int)(centerY + radiusY * sin(a));
        if (x >= 0 && x < width && y >= 0 && y < height) grid[y][x] = '.';
    }

    // Place nodes
    for (size_t i = 0; i < nodes.size(); ++i) {
        uint8_t id = nodes[i]->getId();
        // Calculate angle based on 8-bit ID (0-255)
        // 0 is at the top ( -PI/2 )
        float angle = (id / 256.0) * 2 * 3.14159 - (3.14159 / 2.0);
        int x = (int)(centerX + radiusX * cos(angle));
        int y = (int)(centerY + radiusY * sin(angle));
        
        string label = "n" + to_string(i) + "(" + to_string((int)id) + ")";
        
        // Offset label slightly so it doesn't overwrite the dot directly if possible, 
        // or just center it.
        int startX = x - label.length() / 2;
        for (size_t j = 0; j < label.length(); ++j) {
            int lx = startX + j;
            if (lx >= 0 && lx < width && y >= 0 && y < height) {
                grid[y][lx] = label[j];
            }
        }
    }

    cout << "\n      Generated Identifier Circle (m = 8)" << endl;
    for (const string& row : grid) {
        cout << row << endl;
    }
    cout << endl;
}

int main() {
    cout << "1. Add nodes to the network using the join function, m = 8" << std::endl;
    
    Node n0(0);
    Node n1(30);
    Node n2(65);
    Node n3(110);
    Node n4(160);
    Node n5(230);
    
    vector<Node*> nodes = {&n0, &n1, &n2, &n3, &n4, &n5};
    printIdentifierCircle(nodes);

    n0.join(NULL);
    n1.join(&n0);
    n2.join(&n1);
    n3.join(&n2);
    n4.join(&n3);
    n5.join(&n4);
    
    // ... rest of main ...

    
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
