# Chord DHT Implementation

A C++ implementation of the Chord Distributed Hash Table (DHT) protocol, as described in the original paper by Stoica et al. This project was developed as part of the CSE 250A coursework.

## Features

- **8-bit Identifier Space**: Uses a 256-node circular identifier space.
- **Dynamic Node Join**: New nodes can join the network by contacting an existing bootstrap node.
- **Finger Table Routing**: Implements the scalable $O(\log N)$ routing mechanism using finger tables.
- **Key Migration**: Automatically redistributes keys when new nodes join to maintain the DHT property.
- **Lookup Visualization**: Displays the path of nodes traversed during a key search.
- **Key-Value Storage**: Supports basic `insert`, `remove`, and `find` operations.

## Project Structure

- `node.h` / `node.cpp`: Core logic for the Chord node, including finger table management and routing.
- `main.cpp`: Test driver simulating a Chord network with sample join and lookup operations.
- `chord_paper.pdf`: Reference material for the Chord protocol.

## Getting Started

### Prerequisites

- A C++ compiler (e.g., `g++` or `clang++`) supporting C++17.
- **Optional**: [CMake](https://cmake.org/) for standardized builds.
- **Optional**: [VS Code](https://code.visualstudio.com/) with the "C/C++" extension for one-click debugging.

### Compilation

#### Method 1: VS Code (Recommended)
1. Open this folder in VS Code.
2. Press `F5` or go to the "Run and Debug" tab and click "Debug Chord DHT".
3. The project will automatically compile and run.

#### Method 2: CMake
```bash
mkdir build && cd build
cmake ..
make
./chord_dht
```

#### Method 3: Manual g++
```bash
g++ -std=c++17 main.cpp node.cpp -o chord_dht
```

### Running the Simulation

Run the compiled executable to see a demonstration of nodes joining and performing lookups:

```bash
./chord_dht
```

## How It Works

### Node Join

When a node joins, it:

1.  Initializes its finger table by querying a bootstrap node.
2.  Updates the predecessor and successor pointers in the ring.
3.  Migrates keys from its successor that now fall within its range.

### Key Lookup

The `find(key)` operation:

1.  Checks if the key is managed by the local node or its immediate successor.
2.  If not, it uses the finger table to jump to the node that most closely precedes the key.
3.  Repeats the process until the successor of the current node owns the key.

## References

- [Chord: A Scalable Peer-to-peer Lookup Service for Internet Applications](https://conferences.sigcomm.org/sigcomm/2001/p12-stoica.pdf)
