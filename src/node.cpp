#include "node.h"
#include <iostream>
#include <iomanip>

using namespace std;

// --- Helper Functions ---

// Check if val is in (start, end]
// Handles wrapping around 2^BITLENGTH
bool in_interval_right_inclusive(int val, int start, int end) {
    if (start == end) return true; // Full circle (if more than 1 node, this logic might need care, but standard Chord treats equal as full)
    int limit = (1 << BITLENGTH);
    if (start < end) {
        return val > start && val <= end;
    } else { // Wrap around
        return val > start || val <= end;
    }
}

// Check if val is in (start, end)
bool in_interval_exclusive(int val, int start, int end) {
    if (start == end) return false; // Empty interval
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
    cout << "FingerTable for Node " << (int)nodeId_ << ":" << endl;
    for (size_t i = 1; i <= BITLENGTH; ++i) {
        uint8_t start = (nodeId_ + (1 << (i-1))) % (1 << BITLENGTH);
        int succId = -1;
        if (fingerTable_[i]) succId = fingerTable_[i]->getId();
        // Calculate interval end for display
        uint8_t next_start = (nodeId_ + (1 << i)) % (1 << BITLENGTH);
        if (i == BITLENGTH) next_start = (nodeId_ + 1) % (1 << BITLENGTH); // rough approx for display
        
        cout << "| k = " << i << " [" << (int)start << ", " << (int)((start + (1<<(i-1)))%(1<<BITLENGTH)) << ") " 
             << "succ. = " << succId << endl;
    }
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
    if (node) {
        init_finger_table(node);
        // Basic stabilization/ring update:
        // Our predecessor's successor must be us.
        if (predecessor_) {
             predecessor_->setSuccessor(this);
        }
        
        // Move keys from successor
        Node* succ = getSuccessor();
        if (succ && succ != this) {
             migrate_keys_from(succ);
        }
    } else {
        // Form ring with self
        for (size_t i = 1; i <= BITLENGTH; ++i) {
            fingerTable_.set(i, this);
        }
        predecessor_ = this;
    }
    
    fingerTable_.prettyPrint();
    printKeys();
}

void Node::init_finger_table(Node* n) {
    // finger[1] = n.find_successor(start[1])
    uint8_t start1 = (id_ + 1) % (1 << BITLENGTH);
    Node* succ = n->find_successor(start1);
    fingerTable_.set(1, succ);
    
    predecessor_ = succ->getPredecessor();
    succ->setPredecessor(this);
    
    for (size_t i = 1; i < BITLENGTH; ++i) {
        uint8_t start_next = (id_ + (1 << i)) % (1 << BITLENGTH);
        Node* finger_i = fingerTable_.get(i);
        
        // Optimization: if start_next is in [id, finger_i.id), use finger_i
        // But to be safe and simple, we can just query find_successor again.
        // n->find_successor(start_next) works generally.
        // But we should use our own lookup if we were fully linked? No, we are joining.
        // We use `n` (the bootstrapper) to find successors.
        
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
        if (n0 == this) {
            // If closest preceding is self, and it's not in our immediate successor's range,
            // we might be in a scenario where we are the best bet or the ring is small.
            // But we should not infinite loop.
            // If n0 == this, then we return succ? 
            // It means no other finger is closer.
            return succ;
        }
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
    cout << "Inserting key " << (int)key << " into node " << (int)target->getId() << endl;
    target->localKeys_[key] = value;
}

void Node::remove(uint8_t key) {
     Node* target = find_successor(key);
     target->localKeys_.erase(key);
}

uint8_t Node::find(uint8_t key) {
    cout << "Lookup path: ";
    Node* curr = this;
    cout << (int)curr->getId();
    
    int steps = 0;
    while (steps < 20) { // Safety break
         Node* succ = curr->getSuccessor();
         if (in_interval_right_inclusive(key, curr->getId(), succ->getId())) {
             if (succ != curr) cout << " -> " << (int)succ->getId();
             cout << endl;
             // Check if key exists? The assignment implies it does or we return 0/default.
             if (succ->localKeys_.count(key))
                 return succ->localKeys_[key];
             else
                 return 0; // Or some error
         }
         
         Node* next_node = curr->closest_preceding_node(key);
         if (next_node == curr) {
             // Fallback to linear
             curr = succ;
         } else {
             curr = next_node;
         }
         cout << " -> " << (int)curr->getId();
         steps++;
    }
    cout << endl;
    return 0;
}

void Node::migrate_keys_from(Node* succ) {
    auto it = succ->localKeys_.begin();
    while (it != succ->localKeys_.end()) {
        uint8_t k = it->first;
        // Logic: if k is NOT in (id, succ->id], it belongs to me (id).
        // (id, succ->id] is the range succ keeps.
        if (!in_interval_right_inclusive(k, id_, succ->getId())) {
            localKeys_[k] = it->second;
            cout << "Migrated key " << (int)k << " from Node " << (int)succ->getId() << " to Node " << (int)id_ << endl;
            it = succ->localKeys_.erase(it);
        } else {
            ++it;
        }
    }
}

void Node::printKeys() {
    cout << "Node " << (int)id_ << " keys: ";
    for (auto const& [key, val] : localKeys_) {
        cout << (int)key << " ";
    }
    cout << endl;
}
