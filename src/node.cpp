#include "node.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

// ANSI Color Codes for "Visually Pleasing" Terminal Output
const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string CYAN = "\033[36m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string RED = "\033[31m";

// --- Helper Functions ---

bool in_interval_right_inclusive(int val, int start, int end) {
    if (start == end) return true;
    if (start < end) {
        return val > start && val <= end;
    } else { 
        return val > start || val <= end;
    }
}

bool in_interval_exclusive(int val, int start, int end) {
    if (start == end) return false;
    if (start < end) {
        return val > start && val < end;
    } else {
        return val > start || val < end;
    }
}

// --- FingerTable Implementation ---

FingerTable::FingerTable(uint8_t nodeId) : nodeId_(nodeId) {
    fingerTable_.resize(BITLENGTH + 1, nullptr);
}

void FingerTable::set(size_t index, Node* successor) {
    if (index >= 1 && index <= BITLENGTH)
        fingerTable_[index] = successor;
}

Node* FingerTable::get(size_t index) {
    if (index >= 1 && index <= BITLENGTH)
        return fingerTable_[index];
    return nullptr;
}

void FingerTable::prettyPrint() {
    cout << CYAN << BOLD << "\n[ Finger Table for Node " << (int)nodeId_ << " ]" << RESET << endl;
    cout << CYAN << "----------------------------------------------------" << RESET << endl;
    cout << left << setw(10) << "Index" << setw(15) << "Interval" << "Successor Node" << endl;
    cout << CYAN << "----------------------------------------------------" << RESET << endl;
    for (size_t i = 1; i <= BITLENGTH; ++i) {
        uint8_t start = (nodeId_ + (1 << (i-1))) % (1 << BITLENGTH);
        int succId = -1;
        if (fingerTable_[i]) succId = fingerTable_[i]->getId();
        
        string interval = "[" + to_string((int)start) + ", ...)";
        cout << left << setw(10) << i << setw(15) << interval 
             << YELLOW << (succId == -1 ? "NULL" : "ID: " + to_string(succId)) << RESET << endl;
    }
    cout << CYAN << "----------------------------------------------------" << RESET << endl;
}

// --- Node Implementation ---

Node::Node(uint8_t id) : id_(id), fingerTable_(id) {
    predecessor_ = nullptr;
}

Node* Node::getSuccessor() {
    return fingerTable_.get(1);
}

Node* Node::getPredecessor() {
    return predecessor_;
}

void Node::setPredecessor(Node* node) {
    predecessor_ = node;
}

void Node::setSuccessor(Node* node) {
    fingerTable_.set(1, node);
}

void Node::join(Node* node) {
    cout << BLUE << BOLD << ">>> Node " << (int)id_ << " is joining the network..." << RESET << endl;
    if (node) {
        init_finger_table(node);
        if (predecessor_) {
             predecessor_->setSuccessor(this);
        }
        Node* succ = getSuccessor();
        if (succ && succ != this) {
             migrate_keys_from(succ);
        }
    } else {
        for (size_t i = 1; i <= BITLENGTH; ++i) {
            fingerTable_.set(i, this);
        }
        predecessor_ = this;
        cout << GREEN << "Node " << (int)id_ << " initialized as the first node in the ring." << RESET << endl;
    }
    
    fingerTable_.prettyPrint();
}

void Node::init_finger_table(Node* n) {
    uint8_t start1 = (id_ + 1) % (1 << BITLENGTH);
    Node* succ = n->find_successor(start1);
    fingerTable_.set(1, succ);
    
    predecessor_ = succ->getPredecessor();
    succ->setPredecessor(this);
    
    for (size_t i = 1; i < BITLENGTH; ++i) {
        uint8_t start_next = (id_ + (1 << i)) % (1 << BITLENGTH);
        fingerTable_.set(i+1, n->find_successor(start_next));
    }
}

Node* Node::find_successor(uint8_t id) {
    Node* succ = getSuccessor();
    if (!succ) return this; 

    if (in_interval_right_inclusive(id, id_, succ->getId())) {
        return succ;
    } else {
        Node* n0 = closest_preceding_node(id);
        if (n0 == this) return succ;
        return n0->find_successor(id);
    }
}

Node* Node::closest_preceding_node(uint8_t id) {
    for (size_t i = BITLENGTH; i >= 1; --i) {
        Node* finger = fingerTable_.get(i);
        if (!finger) continue;
        if (in_interval_exclusive(finger->getId(), id_, id)) {
            return finger;
        }
    }
    return this;
}

void Node::insert(uint8_t key, uint8_t value) {
    Node* target = find_successor(key);
    cout << MAGENTA << "[INSERT] Key " << (int)key << " -> Stored at Node " << (int)target->getId() << RESET << endl;
    target->localKeys_[key] = value;
}

void Node::remove(uint8_t key) {
     Node* target = find_successor(key);
     cout << RED << "[REMOVE] Deleting key " << (int)key << " from Node " << (int)target->getId() << RESET << endl;
     target->localKeys_.erase(key);
}

uint8_t Node::find(uint8_t key) {
    cout << YELLOW << BOLD << "[LOOKUP] Searching for Key " << (int)key << RESET << endl;
    cout << "  Path: " << BLUE << BOLD << (int)id_ << RESET;
    
    Node* curr = this;
    int steps = 0;
    while (steps < 256) {
         Node* succ = curr->getSuccessor();
         if (in_interval_right_inclusive(key, curr->getId(), succ->getId())) {
             if (succ != curr) cout << " -> " << BLUE << BOLD << (int)succ->getId() << RESET;
             cout << GREEN << " [FOUND!]" << RESET << endl;
             return succ->localKeys_.count(key) ? succ->localKeys_[key] : 0;
         }
         
         Node* next_node = curr->closest_preceding_node(key);
         curr = (next_node == curr) ? succ : next_node;
         cout << " -> " << BLUE << BOLD << (int)curr->getId() << RESET;
         steps++;
    }
    cout << RED << " [FAILED]" << RESET << endl;
    return 0;
}

void Node::migrate_keys_from(Node* succ) {
    auto it = succ->localKeys_.begin();
    while (it != succ->localKeys_.end()) {
        uint8_t k = it->first;
        if (!in_interval_right_inclusive(k, id_, succ->getId())) {
            localKeys_[k] = it->second;
            cout << GREEN << "  (Migration) Moved key " << (int)k << " from Node " << (int)succ->getId() << " to Node " << (int)id_ << RESET << endl;
            it = succ->localKeys_.erase(it);
        } else {
            ++it;
        }
    }
}

void Node::printKeys() {
    cout << "Node " << (int)id_ << " keys: { ";
    for (auto const& [key, val] : localKeys_) {
        cout << (int)key << " ";
    }
    cout << "}" << endl;
}
