#ifndef BLOSSOM_HPP
#define BLOSSOM_HPP

#include "graph.hpp"
#include "matching.hpp"
#include <vector>
#include <set>
#include <map>

namespace ED {

/**
   @brief Shrunk supernode representing a blossom (odd cycle).
   Keeps track of which original nodes are in this blossom.
*/
struct Blossom {
   NodeId supernode_id;
   std::vector<NodeId> members;  // Original node ids in this blossom
   NodeId parent_in_tree;        // Parent edge in alternating tree
};

/**
   @brief The Perfect Matching algorithm engine using Edmonds' Blossom algorithm.
*/
class BlossomMatcher {
public:
   BlossomMatcher(const Graph& graph);
   
   // Main algorithm: find perfect matching or return empty if none exists
   std::vector<std::pair<NodeId, NodeId>> find_perfect_matching();
   
private:
   const Graph& _graph;
   Matching _matching;
   AlternatingTree* _tree;
   UnionFind* _uf;
   std::vector<Blossom> _blossoms;
   
   // Helper: Check if edge is in contracted graph
   bool edge_in_contracted_graph(NodeId u, NodeId v);
   
   // Main step: Grow alternating tree from root
   bool grow_alternating_tree();
   
   // Augment matching along path
   void augment_along_path(const std::vector<NodeId>& path);
   
   // Shrink (contract) an odd cycle/blossom
   void shrink_blossom(NodeId u, NodeId v, const std::vector<NodeId>& cycle);
   
   // Unshrink: reconstruct original matching from contracted version
   std::vector<std::pair<NodeId, NodeId>> unshrink_matching();
   
   // Find lowest common ancestor in tree
   NodeId find_lca(NodeId u, NodeId v);
   
   // Build contracted graph for next iteration
   Graph build_contracted_graph();
   
   // Utility: Get original nodes represented by a (possibly shrunk) node
   std::set<NodeId> get_members(NodeId node);
};

} // namespace ED

#endif /* BLOSSOM_HPP */
