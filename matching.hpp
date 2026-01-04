#ifndef MATCHING_HPP
#define MATCHING_HPP

#include "graph.hpp"
#include <vector>
#include <set>
#include <queue>
#include <utility>

namespace ED {

/**
   @brief Represents a matching and provides operations for augmentation and validation.
*/
class Matching {
public:
   Matching(NodeId num_nodes);
   
   // Basic operations
   bool is_exposed(NodeId node) const;
   NodeId get_mate(NodeId node) const;
   void set_mate(NodeId u, NodeId v);
   size_type size() const { return _match_size; }
   
   // Get all matched edges
   std::vector<std::pair<NodeId, NodeId>> get_edges() const;
   
   // Check if matching is perfect
   bool is_perfect(NodeId num_nodes) const;
   
private:
   std::vector<NodeId> _mate;  // _mate[u] = v means u-v are matched
   size_type _match_size;
};

/**
   @brief Represents an alternating tree rooted at an exposed node.
   Tree nodes are at alternating distances from root:
   - Even distance: outer nodes (matched via tree edges to odd nodes)
   - Odd distance: inner nodes (paired in matching M)
*/
class AlternatingTree {
public:
   AlternatingTree(NodeId root, NodeId num_nodes);
   
   bool contains(NodeId node) const { return _in_tree[node]; }
   bool is_even(NodeId node) const;
   bool is_odd(NodeId node) const;
   NodeId get_parent(NodeId node) const { return _parent[node]; }
   NodeId get_root() const { return _root; }
   
   // Add node to tree via parent with specific parity (even/odd)
   void add_even_node(NodeId node, NodeId parent);
   void add_odd_node(NodeId node, NodeId parent);
   
   // Get path from root to node
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
   
   // Get all nodes in same component
   std::set<NodeId> get_component(NodeId node) const;
   
private:
   mutable std::vector<NodeId> _parent;
   std::vector<int> _rank;
};

} // namespace ED

#endif /* MATCHING_HPP */
