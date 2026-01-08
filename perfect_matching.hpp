#pragma once

#include "graph.hpp"
using namespace ED;

class M_tree{
    public:
    NodeId root;
};

// Returns true if the provided graph represents a perfect matching (i.e., exactly n/2 edges)
bool IsPerfectMatching(Graph const & graph);

// Placeholder: returns a graph modeling the perfect matching subgraph (if any)
Graph GetPerfectMatchingGraph(Graph const & graph);

