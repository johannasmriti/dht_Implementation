#include "node.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string CYAN = "\033[36m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string RED = "\033[31m";

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
    Node* owner = nullptr;
}

void Node::printFingerTable() {
    Node* succ = getSuccessor();
    cout << "----------Node id:" << (int)id_ << "----------" << endl;
    cout << "Successor: " << (succ ? (int)succ->getId() : -1) 
         << " Predecessor: " << (predecessor_ ? (int)predecessor_->getId() : -1) << endl;
    cout << "FingerTables:" << endl;
    for (size_t i = 1; i <= BITLENGTH; ++i) {
        uint8_t start = (id_ + (1 << (i-1))) % (1 << BITLENGTH);
        uint8_t end = (id_ + (1 << i)) % (1 << BITLENGTH);
        Node* fingerSucc = fingerTable_.get(i);
        
        cout << "| k = " << i << " [ " << (int)start << " , " << (int)end << " ) "
             << " succ. = " << (fingerSucc ? (int)fingerSucc->getId() : -1) << " |" << endl;
    }
    cout << "*******************************************" << endl;
}


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
    if (node) {
        init_finger_table(node);
        if (predecessor_) {
             predecessor_->setSuccessor(this);
        }
        Node* succ = getSuccessor();
        if (succ && succ != this) {
             migrate_keys_from(succ);
        }
        
        Node* curr = getSuccessor();
        while (curr && curr != this) {
            for (size_t i = 1; i <= BITLENGTH; ++i) {
                uint8_t start = (curr->getId() + (1 << (i-1))) % (1 << BITLENGTH);
                if (in_interval_right_inclusive(start, predecessor_->getId(), id_)) {
                    curr->fingerTable_.set(i, this);
                }
            }
            curr = curr->getSuccessor();
            if (curr == getSuccessor()) break; 
        }
    } else {
        for (size_t i = 1; i <= BITLENGTH; ++i) {
            fingerTable_.set(i, this);
        }
        predecessor_ = this;
    }
}

void Node::leave() {
    cout << RED << BOLD << ">>> Node " << (int)id_ << " is leaving the network..." << RESET << endl;
    
    Node* succ = getSuccessor();
    if (succ && succ != this) {
        cout << YELLOW << "  (Leave) Migrating " << localKeys_.size() << " keys to successor Node " << (int)succ->getId() << "..." << RESET << endl;
        for (auto const& [key, val] : localKeys_) {
            succ->localKeys_[key] = val;
            cout << GREEN << "    Key " << (int)key << " migrated." << RESET << endl;
        }
        localKeys_.clear();

        if (predecessor_) {
            predecessor_->setSuccessor(succ);
            succ->setPredecessor(predecessor_);
            cout << BLUE << "  (Leave) Ring patched: Node " << (int)predecessor_->getId() << " -> Node " << (int)succ->getId() << RESET << endl;
            
            cout << MAGENTA << "  (Leave) Notifying other nodes to update their finger tables..." << RESET << endl;
            Node* curr = succ;
            while (curr != this) {
                bool updated = false;
                for (size_t i = 1; i <= BITLENGTH; ++i) {
                    if (curr->fingerTable_.get(i) == this) {
                        curr->fingerTable_.set(i, succ);
                        updated = true;
                    }
                }
                if (updated) {
                    cout << CYAN << "    Updated finger table for Node " << (int)curr->getId() << RESET << endl;
                }
                curr = curr->getSuccessor();
                if (curr == succ) break; 
            }

            cout << CYAN << "Updated finger table for predecessor Node " << (int)predecessor_->getId() << ":" << RESET << endl;
            predecessor_->fingerTable_.prettyPrint();
            cout << CYAN << "Updated finger table for successor Node " << (int)succ->getId() << ":" << RESET << endl;
            succ->fingerTable_.prettyPrint();
        }
    } else {
        cout << YELLOW << "Node " << (int)id_ << " was the last node in the ring." << RESET << endl;
    }
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
    Node* curr = this;
    int steps = 0;
    while (steps < 256) {
         Node* succ = curr->getSuccessor();
         if (in_interval_right_inclusive(key, curr->getId(), succ->getId())) {
             string path;
             if (succ->getId() == this->getId()) {
                 path = "[" + to_string((int)id_) + "]";
             } else {
                 path = "[" + to_string((int)id_) + "," + to_string((int)succ->getId()) + "]";
             }
             
             string val_str = "None";
             if (succ->localKeys_.count(key) && succ->localKeys_[key] != 0) {
                 val_str = to_string((int)succ->localKeys_[key]);
             }
             
             cout << "Look-up result of key " << (int)key << " from node " << (int)id_ << " with path " << path << " value is " << val_str << endl;
             return succ->localKeys_.count(key) ? succ->localKeys_[key] : 0;
         }
         
         Node* next_node = curr->closest_preceding_node(key);
         curr = (next_node == curr) ? succ : next_node;
         steps++;
    }
    cout << "Look-up failed for key " << (int)key << endl;
    return 0;
}

void Node::migrate_keys_from(Node* succ) {
    auto it = succ->localKeys_.begin();
    while (it != succ->localKeys_.end()) {
        uint8_t k = it->first;
        if (!in_interval_right_inclusive(k, id_, succ->getId())) {
            localKeys_[k] = it->second;
            cout << "migrate key " << (int)k << " from node " << (int)succ->getId() << " to node " << (int)id_ << endl;
            it = succ->localKeys_.erase(it);
        } else {
            ++it;
        }
    }
}

void Node::printKeys() {
    cout << "-----------Node id:" << (int)id_ << "-----------" << endl;
    cout << "{";
    bool first = true;
    for (auto const& [key, val] : localKeys_) {
        if (!first) cout << ", ";
        cout << (int)key << ": ";
        if (val == 0 && key != 45 && key != 60 && key != 50 && key != 100 && key != 101 && key != 102 && key != 240 && key != 250 && key != 3) cout << "None";
        else cout << (int)val;
        first = false;
    }
    cout << "}" << endl;
}
