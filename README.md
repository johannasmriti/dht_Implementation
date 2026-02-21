# Chord DHT Implementation

A C++ implementation of the Chord Distributed Hash Table (DHT) protocol.

## Features

- **8-bit Identifier Space**: Uses a 256-node circular identifier space.
- **Dynamic Node Join**: New nodes can join the network via bootstrap nodes.
- **Finger Table Routing**: Optimized $O(\log N)$ routing.
- **Visualized Output**: Color-coded terminal output for lookups and joins.

## Setup & Running (VS Code)

This project is configured for a seamless VS Code experience.

1.  **Open Folder**: Open this repository in VS Code.
2.  **Build & Run**: Press **`F5`** on your keyboard.
    *   This will automatically compile the source code using `g++`.
    *   The simulation will start in the **Integrated Terminal**.

## Project Structure

- `src/`: Core logic (`node.cpp`, `node.h`) and test driver (`main.cpp`).
- `docs/`: Assignment documentation and the original Chord paper.
- `README.md`: Setup instructions.

## Output Breakdown

The program outputs a color-coded simulation:
- **BLUE**: Node join operations.
- **CYAN**: Finger Table displays.
- **YELLOW**: Key lookup paths.
- **GREEN**: Success messages and key migrations.
- **MAGENTA/RED**: Insert/Remove operations.
