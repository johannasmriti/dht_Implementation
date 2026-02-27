#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include <iostream>
#include <map>
#include <vector>
#include <set>

#define BITLENGTH 8
#define NUM_SPACES 3

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
    
    // DHT helper functions (Mimicking RPC)
    uint8_t localLookup(uint8_t key);
    Node* find_successor(uint8_t id, std::vector<uint8_t>* path = nullptr);
    Node* closest_preceding_node(uint8_t id);
    
    // Manage keys
    void migrate_keys_from(Node* successor);

    // Update finger table of this node
    void init_finger_table(Node* n);

    // --- Space Shuffle (S2) Methods ---
    void setCoordinates(const std::vector<uint8_t>& coords);
    const std::vector<uint8_t>& getCoordinates() const { return coordinates_; }
    void addS2Neighbor(Node* neighbor);
    uint8_t s2Lookup(const std::vector<uint8_t>& targetCoords);
    static uint8_t calculateCD(uint8_t x, uint8_t y);
    uint8_t calculateMCD(const std::vector<uint8_t>& targetCoords);
    
private:
	uint8_t id_;
	FingerTable fingerTable_;
	std::map<uint8_t, uint8_t> localKeys_;
	Node* predecessor_;
    
    // Space Shuffle data
    std::vector<uint8_t> coordinates_;
    std::set<Node*> s2Neighbors_;

    // Friends to access private members
    friend class FingerTable; 


};

#endif
