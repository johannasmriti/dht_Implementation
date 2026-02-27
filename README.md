# CSE 250A: Chord DHT Project

## Overview

This project is a C++ implementation of the **Chord Distributed Hash Table (DHT)**, based on the landmark paper by Stoica et al. The goal was to build a peer-to-peer system where nodes can join, leave, and route lookups efficiently in a circular identifier space.

For this specific project, we used an **8-bit identifier space** (0-255) to keep the simulation manageable and grading-friendly. We omitted the standard SHA-1 hashing to focus purely on the structural logic of the Chord ring and routing algorithms.

---

## Core Implementations

### 1. The Chord Node (`Node` class)

Each node maintains its own local state, including:

- **Finger Table:** A routing table used to achieve $O(\log N)$ lookups.
- **Predecessor/Successor:** Pointers to immediate neighbors in the ring.
- **Local Keys:** A map of 8-bit keys and values that the node is responsible for.

### 2. Node Join (`join(Node* bootstrap)`)

When a node joins the network:

- It initializes its finger table by contacting an existing node (`bootstrap`).
- It finds its place in the ring and updates its successor and predecessor.
- **Key Migration:** It takes responsibility for keys that now fall into its identifier range (the interval between its predecessor and itself). The console prints these specific migrations (e.g., `migrate key 100 from node 110 to node 100`).

### 3. Routing & Lookups (`find(uint8_t key)`)

I implemented lookups using the **Recursive RPC Mimicry** style suggested in the project tips:

- Each node uses its finger table to find the "closest preceding node" to the target key.
- The request is forwarded recursively (`remoteLookup` pattern) until it reaches the node managing the key.
- **Path Tracking:** The program tracks the sequence of hops. As per the sample requirements, the output simplifies this to show the origin and the destination: `Look-up result of key X from node Y with path [Y, Z]`.

### 4. Node Leave (`leave()`) - _Optional Task_

I implemented the voluntary node departure logic:

- The leaving node migrates all its local keys to its immediate successor.
- It patches the ring by notifying its neighbors to "link up," bypassing itself.
- It attempts to notify other nodes to update their finger tables to maintain system integrity.

### 5. Space Shuffle Simulation (S2) - _Optional Task_

This project also includes a simulation of the **Space Shuffle** protocol.

- Nodes are assigned coordinates in a 3D space.
- Routing is performed using "Greediest Routing" based on the Minimum Chordal Distance (MCD).
- This is demonstrated at the end of the `main` execution loop.

---

## How to Run

### Manual Compilation

From the project root, run:

```bash
g++ -std=c++17 src/main.cpp src/node.cpp -Isrc -o chord_dht && ./chord_dht
```

---
