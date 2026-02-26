#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
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

    for (float a = 0; a < 2 * 3.14159; a += 0.05) {
        int x = (int)(centerX + radiusX * cos(a));
        int y = (int)(centerY + radiusY * sin(a));
        if (x >= 0 && x < width && y >= 0 && y < height) grid[y][x] = '.';
    }

    for (size_t i = 0; i < nodes.size(); ++i) {
        uint8_t id = nodes[i]->getId();
        float angle = (id / 256.0) * 2 * 3.14159 - (3.14159 / 2.0);
        int x = (int)(centerX + radiusX * cos(angle));
        int y = (int)(centerY + radiusY * sin(angle));
        
        string label = "n" + to_string(i) + "(" + to_string((int)id) + ")";
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

void simulateSpaceShuffle() {
    cout << "\n===========================================" << endl;
    cout << "6. Optional: Simulate Space Shuffle (S2)" << endl;
    cout << "===========================================" << endl;

    const int num_nodes = 10;
    const int num_spaces = 3;
    vector<Node*> s2_nodes;
    
    for (int i = 0; i < num_nodes; ++i) {
        s2_nodes.push_back(new Node(i * 25));
    }

    mt19937 rng(42); 
    uniform_int_distribution<int> dist(0, 255);

    cout << "Node Coordinates in " << num_spaces << " spaces:" << endl;
    for (auto node : s2_nodes) {
        vector<uint8_t> coords;
        for (int s = 0; s < num_spaces; ++s) {
            coords.push_back((uint8_t)dist(rng));
        }
        node->setCoordinates(coords);
        cout << "  Node " << (int)node->getId() << ": [";
        for (int s = 0; s < num_spaces; ++s) cout << (int)coords[s] << (s == num_spaces - 1 ? "" : ", ");
        cout << "]" << endl;
    }

    for (int s = 0; s < num_spaces; ++s) {
        vector<pair<uint8_t, Node*>> ring;
        for (auto node : s2_nodes) {
            ring.push_back({node->getCoordinates()[s], node});
        }
        sort(ring.begin(), ring.end());

        for (int i = 0; i < num_nodes; ++i) {
            Node* curr = ring[i].second;
            Node* prev = ring[(i - 1 + num_nodes) % num_nodes].second;
            Node* next = ring[(i + 1) % num_nodes].second;
            curr->addS2Neighbor(prev);
            curr->addS2Neighbor(next);
        }
    }

    cout << "\nPerforming Space Shuffle Lookups (Greediest Routing):" << endl;
    
    vector<uint8_t> target = s2_nodes[9]->getCoordinates();
    s2_nodes[0]->s2Lookup(target);

    vector<uint8_t> randomTarget = {100, 200, 50};
    s2_nodes[2]->s2Lookup(randomTarget);

    for (auto node : s2_nodes) delete node;
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

    simulateSpaceShuffle();
    
    return 0;
}
