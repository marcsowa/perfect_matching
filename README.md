# Perfect Matching (Edmonds' Blossom Algorithm)

This repository contains an implementation of Edmonds' Perfect Matching (Blossom) algorithm intended as an exercise submission.

## Build & Run

### Compile
```bash
./compile.sh
```

This produces the executable `edmonds.out`.

### Usage
```bash
./edmonds.out <input_file>.dmx
```

### Input / Output format

- Input must be in the DIMACS format for undirected graphs:
  - Lines starting with `c` are comments and ignored.
  - The problem line(first line) has the form `p edge n m` where `n` is the number of vertices and `m` is the number of edges.
  - Each edge line has the form `e i j` (1-based vertex indices).
- Output:
  - If a perfect matching exists, the program prints the DIMACS encoding of the subgraph containing only matching edges (same `p edge n m` header followed by `e i j` lines).
  - If no perfect matching exists the program prints exactly: `No perfect matching` (without quotes).

## Project Structure

- `graph.hpp`, `graph.cpp` — Graph data structure and DIMACS parser
- `matching.hpp`, `matching.cpp` — Matching data structure, alternating trees, and Union-Find
- `blossom.hpp`, `blossom.cpp` — `BlossomMatcher` implementing the core algorithm
- `main.cpp` — Command-line interface and I/O
- `compile.sh` — Simple compile script that uses `g++` with `-std=c++20 -pedantic -Wall -Wextra -Werror`
- `instances/` — Sample DIMACS instances used for testing

### Algorithm Overview

The implementation follows the standard steps of Edmonds' algorithm:

1. Initialize with an empty matching and repeatedly search for augmenting paths.
2. Build an alternating tree rooted at an exposed vertex using BFS.
3. When an augmenting path is found, augment the matching along the path.
4. When an odd cycle (blossom) is discovered, contract it to a supernode and continue the search on the contracted graph.
5. When no augmenting path exists the algorithm reports that no perfect matching exists; otherwise it outputs a perfect matching when all vertices are matched.

The implementation focuses on clarity and correctness while incorporating standard optimizations (Union-Find contraction, neighbor caching, visited arrays).

### Complexity

The optimized implementation attains the expected theoretical complexity:

- Augmentations: O(n) (at most n/2 augmentations)
- Each tree build via BFS: O(n + m)
- Blossom handling using Union-Find and careful bookkeeping: O(n^2 log n) worst-case

Overall: O(nm + n^2 log n)

## Optimizations

Key optimizations used in the codebase:

- Union-Find with path compression for efficient contractions
- Caching of contracted neighbor lists to avoid recomputation
- Using `std::vector`/arrays for visited flags (O(1) checks) instead of log-time sets
- Early termination when an augmenting path is found

## Tests

The `instances/` folder contains several small and medium test instances. All included instances execute successfully and produce either a valid perfect matching or `No perfect matching` as expected.
