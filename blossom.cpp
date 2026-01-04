#include "blossom.hpp"
#include <algorithm>
#include <queue>
#include <set>
#include <map>
#include <iostream>

namespace ED {

BlossomMatcher::BlossomMatcher(const Graph& graph)
   : _graph(graph), _matching(graph.num_nodes()), 
     _tree(nullptr), _uf(nullptr)
{
   _uf = new UnionFind(graph.num_nodes());
}

std::vector<std::pair<NodeId, NodeId>> BlossomMatcher::find_perfect_matching() {
   NodeId n = _graph.num_nodes();
   
   // If n is odd, no perfect matching possible
   if (n % 2 == 1) {
      return {};
   }
   
   // Main augmentation loop
   size_type augmentations = 0;
   while (augmentations < n) {
      // Find an exposed vertex
      NodeId root = invalid_node_id;
      for (NodeId v = 0; v < n; ++v) {
         if (_matching.is_exposed(v)) {
            root = v;
            break;
         }
      }
      
      // All vertices matched -> perfect matching found
      if (root == invalid_node_id) {
         return _matching.get_edges();
      }
      
      // Build alternating tree from root
      if (_tree) delete _tree;
      _tree = new AlternatingTree(root, n);
      
      // Grow tree until we find augmenting path or no augmentation possible
      if (!grow_alternating_tree()) {
         // No augmenting path exists for this root -> no perfect matching
         return {};
      }
      
      augmentations++;
   }
   
   // Fallback
   auto edges = _matching.get_edges();
   if (edges.size() * 2 == n) {
      return edges;
   }
   return {};
}

bool BlossomMatcher::grow_alternating_tree() {
   std::queue<NodeId> q;
   q.push(_tree->get_root());
   
   while (!q.empty()) {
      NodeId x = q.front();
      q.pop();
      
      if (!_tree->is_even(x)) continue;
      
      // x is even; explore all edges from x
      for (NodeId y : _graph.node(x).neighbors()) {
         if (!_tree->contains(y)) {
            // y not in tree
            if (_matching.is_exposed(y)) {
               // Found augmenting path: root -> ... -> x -> y
               std::vector<NodeId> path = _tree->get_path_to_root(x);
               path.push_back(y);
               augment_along_path(path);
               return true;
            } else {
               // y is matched; add y and its mate z
               NodeId z = _matching.get_mate(y);
               _tree->add_odd_node(y, x);
               _tree->add_even_node(z, y);
               q.push(z);
            }
         } else if (_tree->is_even(y)) {
            // Both x and y are even -> odd cycle detected
            // Shrink the blossom (simplified: ignore for now)
         }
         // If y is odd and in tree, no action needed
      }
   }
   
   return false;
}

void BlossomMatcher::augment_along_path(const std::vector<NodeId>& path) {
   // Toggle edges in path: unmatched become matched, matched become unmatched
   for (size_type i = 0; i + 1 < path.size(); i += 2) {
      NodeId u = path[i];
      NodeId v = path[i + 1];
      
      // This edge goes from unmatched to matched (or vice versa in augmentation)
      // In augmentation, path alternates matched-unmatched-matched-...-unmatched
      if (i + 2 < path.size()) {
         // This is a matched edge in the alternating path (becomes unmatched)
         // Actually, path from root is: root(even)-unmatched-node-unmatched-node...
         // So odd indices are unmatched edges in original matching
         _matching.set_mate(u, v);
      } else {
         // Last edge is unmatched (becomes matched)
         _matching.set_mate(u, v);
      }
   }
}

void BlossomMatcher::shrink_blossom(NodeId /*u*/, NodeId /*v*/, const std::vector<NodeId>& cycle) {
   // Create new supernode
   NodeId supernode = _graph.num_nodes() + _blossoms.size();
   Blossom b;
   b.supernode_id = supernode;
   b.members = cycle;
   _blossoms.push_back(b);
   
   // Union all nodes in cycle
   for (NodeId node : cycle) {
      _uf->unite(cycle[0], node);
   }
}

std::vector<std::pair<NodeId, NodeId>> BlossomMatcher::unshrink_matching() {
   return _matching.get_edges();
}

std::set<NodeId> BlossomMatcher::get_members(NodeId node) {
   std::set<NodeId> members;
   members.insert(node);
   
   for (const auto& b : _blossoms) {
      if (std::find(b.members.begin(), b.members.end(), node) != b.members.end()) {
         for (NodeId m : b.members) {
            members.insert(m);
         }
      }
   }
   
   return members;
}

} // namespace ED

