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

   A Blossom stores the original node ids that form the odd cycle and a
   chosen cycle path used during unshrinking to reconstruct the matching in
   the original graph.
*/
struct Blossom {
   NodeId supernode_id;
   std::vector<NodeId> members;  // Original node ids in this blossom
   std::vector<NodeId> path;     // Odd cycle path for unshrinking
};

/**
   @brief The Perfect Matching algorithm engine using Edmonds' Blossom algorithm.
   Optimized for O(nm + n²log n) runtime.
*/
class BlossomMatcher {
public:
   BlossomMatcher(const Graph& graph);
   ~BlossomMatcher();
   
   /// @brief Find a maximum matching in the graph.
   /// @return A vector of matched node pairs representing the perfect matching or empty if none exists.
   std::vector<std::pair<NodeId, NodeId>> find_perfect_matching();
   
private:
   const Graph& _graph;
   Matching _matching;
   AlternatingTree* _tree;
   UnionFind* _uf;
   std::vector<Blossom> _blossoms;
   
   // Contracted graph information
   std::map<NodeId, std::vector<NodeId>> _contracted_adj;
   std::map<NodeId, NodeId> _supernode_map;
   
   // Grow alternating tree from root (returns true if augmenting path found)
   bool grow_alternating_tree();
   
   // Augment matching along path
   void augment_along_path(const std::vector<NodeId>& path);
   
   // Shrink (contract) an odd cycle/blossom efficiently
   void shrink_blossom(const std::vector<NodeId>& cycle);
   
   // Unshrink: recursively reconstruct original matching from contracted version
   void unshrink_blossom(const Blossom& b, 
                         std::vector<std::pair<NodeId, NodeId>>& result);
   
   // Find lowest common ancestor in tree efficiently
   NodeId find_lca(NodeId u, NodeId v);
   
   // Get effective neighbors considering contractions
   std::vector<NodeId> get_contracted_neighbors(NodeId node);
   
   // Check if node is exposed in contracted graph
   bool is_exposed_contracted(NodeId node);
   
   // Trace path in contracted graph back to original
   std::vector<NodeId> trace_path_contracted(const std::vector<NodeId>& path);
};

} // namespace ED

#endif /* BLOSSOM_HPP */
