#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include <iostream>
#include <map>
#include <vector>

#define BITLENGTH 8

//forward declaration
class Node;

class FingerTable{
public:
	FingerTable(uint8_t nodeId);
	void set(size_t index, Node* successor);
	Node* get(size_t index);
	void prettyPrint();
private:
	uint8_t nodeId_;
	std::vector<Node*> fingerTable_;
};

class Node {
public:
	Node(uint8_t id);
	
	uint8_t getId() const { return id_; }

	/**
	 * @param node: the first node to contact with to initialize join process. 
	 * If this is the first node to join the Chord network, the parameter is NULL.
	 */
	void join(Node* node);

	// Node leave
	void leave();

	// DHT lookup
	uint8_t find(uint8_t key);

	// DHT key insertion
	void insert(uint8_t key, uint8_t value);

	// DHT key deletion
	void remove(uint8_t key);

	// Helper to access predecessor/successor directly if needed
	Node* getSuccessor();
	Node* getPredecessor();
    void setPredecessor(Node* node);
    void setSuccessor(Node* node);
    
	// For debugging/printing
    void printKeys();
    void printFingerTable();
    
    // Helper for finding successor logic
    Node* find_successor(uint8_t id);
    Node* closest_preceding_node(uint8_t id);
    
    // Manage keys
    void migrate_keys_from(Node* successor);

    // Update finger table of this node
    void init_finger_table(Node* n);
    
private:
	uint8_t id_;
	FingerTable fingerTable_;
	std::map<uint8_t, uint8_t> localKeys_;
	Node* predecessor_;
    
    // Friends to access private members
    friend class FingerTable; 
    // Actually FingerTable doesn't need to access Node's private.
    // Node needs to access Node's private (always allowed).
};

#endif
