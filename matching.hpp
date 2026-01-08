#ifndef MATCHING_HPP
#define MATCHING_HPP

#include "graph.hpp"
#include <vector>
#include <set>
#include <queue>
#include <utility>

namespace ED {

/** 
   @class Matching

   @brief Represents a matching and provides operations for augmentation and validation.

   The Matching class stores pairings between vertices via a _mate array where
   _mate[u] = v indicates that u and v are matched. An exposed vertex is
   represented by the sentinel value @c invalid_node_id. The class tracks the
   number of matched pairs to allow O(1) checks for perfection.
   
   @note Clients should ensure NodeId values passed to methods are valid for the instance.
 */
class Matching {
public:
   /// @brief Construct an empty matching for a fixed number of nodes.
   /// @param num_nodes 
   Matching(NodeId num_nodes);
   
   /// @brief Query whether a node is exposed (unmatched) in the current matching.
   /// @param node 
   /// @return true if the node has no mate; false otherwise.
   bool is_exposed(NodeId node) const;

   /// @brief Return the mate of a node in the matching.
   /// @param node The node whose mate is requested.
   /// @return The NodeId matched to @p node.
   /// @pre The caller should typically verify !is_exposed(node) before calling.
   /// @note If @p node is exposed, the return value is unspecified; check is_exposed first.
   NodeId get_mate(NodeId node) const;

   /// @brief Set u and v to be matched to each other.
   /// @param u One endpoint of the matched pair.
   /// @param v The other endpoint of the matched pair.
   /// @pre u and v are valid NodeId values and (typically) distinct.
   /// @post After the call, get_mate(u) == v and get_mate(v) == u. The internal match count is updated.
   void set_mate(NodeId u, NodeId v);

   /// @brief Return the number of matched edges in the matching.
   /// @return The number of pairs (edges) currently in the matching.
   size_type size() const { return _match_size; }

   /// @brief Retrieve all matched edges as a list of node pairs.
   /// @return A vector of (u, v) pairs representing each matched edge. Each matched pair appears exactly once.
   std::vector<std::pair<NodeId, NodeId>> get_edges() const;
   
   /// @brief Check whether the matching is perfect with respect to a given number of nodes.
   /// @param num_nodes The total number of nodes to consider.
   /// @return true if every node among the first @p num_nodes nodes is matched (i.e., no exposed nodes); false otherwise.
   bool is_perfect(NodeId num_nodes) const;
   
private:
   std::vector<NodeId> _mate;  // _mate[u] = v means u-v are matched
   size_type _match_size;
};

/** 
   @brief Represents an alternating tree rooted at an exposed node.

   The tree stores for each vertex whether it is in the tree, its parent in the
   tree, and its depth (distance from the root). Nodes at even depth are outer
   (candidates to be extended) while nodes at odd depth are inner (already
   paired in the matching with their parent). The public methods allow adding
   nodes, querying parity, and retrieving the path from a node to the root.
*/
class AlternatingTree {
public:
   AlternatingTree(NodeId root, NodeId num_nodes);
   
   bool contains(NodeId node) const { return _in_tree[node]; }
   bool is_even(NodeId node) const;
   bool is_odd(NodeId node) const;
   NodeId get_parent(NodeId node) const { return _parent[node]; }
   NodeId get_root() const { return _root; }
   
   void add_even_node(NodeId node, NodeId parent);
   void add_odd_node(NodeId node, NodeId parent);
   
   std::vector<NodeId> get_path_to_root(NodeId node) const;
   
   void clear();
   
private:
   NodeId _root;
   std::vector<bool> _in_tree;
   std::vector<int> _depth;      // depth from root (0 = root)
   std::vector<NodeId> _parent;
};

/** 
   @brief Union-Find structure to handle shrunk blossoms.
   After shrinking, multiple nodes are represented by a single supernode.
*/
class UnionFind {
public:
   UnionFind(NodeId num_nodes);
   
   NodeId find(NodeId node) const;
   void unite(NodeId u, NodeId v);
   
   std::set<NodeId> get_component(NodeId node) const;
   
private:
   mutable std::vector<NodeId> _parent;
   std::vector<int> _rank;
};

} // namespace ED

#endif /* MATCHING_HPP */
