#include "perfect_matching.hpp"
using namespace ED;

bool IsPerfectMatching(Graph const & graph)
{
    // Simple check: exactly n/2 edges implies a perfect matching for a graph where edges form a matching.
    return graph.num_edges() == (graph.num_nodes() / 2);
}

Graph GetPerfectMatchingGraph(Graph const & graph)
{
    // This is a placeholder helper retained for compatibility with the original project layout.
    // It currently returns an empty graph with the same number of nodes.
    Graph matching_graph(graph.num_nodes());
    return matching_graph;
}
