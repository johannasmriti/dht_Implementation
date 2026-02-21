# Chord DHT Implementation

A C++ implementation of the Chord Distributed Hash Table (DHT) protocol.

## Quick Start (Run this Command)

Copy and paste this into your terminal from the project root:

```bash
g++ -std=c++17 src/main.cpp src/node.cpp -Isrc -o chord_dht && ./chord_dht
```

## Features
- **8-bit Identifier Space**: Uses a 256-node circular identifier space.
- **Dynamic Node Join**: New nodes can join the network via bootstrap nodes.
- **Visualized Output**: Color-coded terminal output for lookups and joins.

## Project Structure
- `src/`: Core logic and test driver.
- `docs/`: Assignment documentation and the original Chord paper.
