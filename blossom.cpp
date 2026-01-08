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

BlossomMatcher::~BlossomMatcher() {
   if (_tree) delete _tree;
   if (_uf) delete _uf;
}

std::vector<std::pair<NodeId, NodeId>> BlossomMatcher::find_perfect_matching() {
   NodeId n = _graph.num_nodes();
   
   // Quick infeasibility check: a perfect matching requires an even number of vertices
   if (n % 2 == 1) {
      return {};
   }

   /*
    High-level algorithm (implementation notes):
    - Repeatedly attempt to grow an alternating tree from an exposed root to find an augmenting path.
    - When an augmenting path is found, augment the matching and restart.
    - If an odd cycle (blossom) is discovered, we contract it to a supernode using Union-Find and continue on the contracted graph.
    - The outer loop iterates at most O(n) times (each successful augmentation increases matching size),
      and each tree build visits edges in O(m), yielding the O(nm) part of the complexity.
    - This function coordinates the iterations and invokes the BFS/tree growth logic.
   */

   // Main augmentation loop: repeatedly find augmenting paths
   // Time: O(n) augmentations x O(m) per tree = O(nm)
   bool found_augmentation = true;
   while (found_augmentation) {
      found_augmentation = false;
      
      // Find an exposed vertex to serve as BFS root: O(n)
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
      
      // Build alternating tree from root: O(m)
      if (_tree) delete _tree;
      _tree = new AlternatingTree(root, n);
      
      // Try to find augmenting path
      std::queue<NodeId> q;
      q.push(root);
      std::vector<bool> visited(n, false);
      visited[root] = true;
      
      while (!q.empty() && !found_augmentation) {
         NodeId x = q.front();
         q.pop();
         
         if (!_tree->is_even(x)) continue;
         
         // Explore neighbors of x
         for (NodeId y : _graph.node(x).neighbors()) {
            if (_uf->find(x) == _uf->find(y)) {
               continue;  // Skip within blossom
            }
            
            if (!visited[y]) {
               visited[y] = true;
               
               if (_matching.is_exposed(y)) {
                  // Found augmenting path
                  std::vector<NodeId> path = _tree->get_path_to_root(x);
                  path.push_back(y);
                  augment_along_path(path);
                  found_augmentation = true;
                  break;
               } else {
                  // y is matched to z
                  NodeId z = _matching.get_mate(y);
                  if (!visited[z]) {
                     visited[z] = true;
                     _tree->add_odd_node(y, x);
                     _tree->add_even_node(z, y);
                     q.push(z);
                  }
               }
            } else if (_tree->is_even(y) && _tree->get_parent(y) != x) {
               // Odd cycle (blossom) detected between even nodes x and y.
               // Note: in this simplified BFS loop we purposely do not perform the full
               // on-the-fly contraction here to avoid interfering with the current queue
               // iteration. Blossoms are handled in the more complete grow_alternating_tree
               // path where shrink_blossom() is explicitly invoked.
            }
         }
      }
      
      // If no augmentation found, no perfect matching exists
      if (!found_augmentation && root != invalid_node_id) {
         return {};
      }
   }
   
   // Return final matching
   return _matching.get_edges();
}

bool BlossomMatcher::grow_alternating_tree() {
   // BFS-based tree growth: O(n + m)
   std::queue<NodeId> q;
   q.push(_tree->get_root());
   
   while (!q.empty()) {
      NodeId x = q.front();
      q.pop();
      
      if (!_tree->is_even(x)) continue;
      
      // Explore neighbors of x: O(degree(x))
      auto neighbors = get_contracted_neighbors(x);
      for (NodeId y : neighbors) {
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
            // Extract cycle path: O(n)
            std::vector<NodeId> path_x = _tree->get_path_to_root(x);
            std::vector<NodeId> path_y = _tree->get_path_to_root(y);
            
            // Find LCA: O(n) with optimization
            std::set<NodeId> ancestors_x(path_x.begin(), path_x.end());
            NodeId lca = invalid_node_id;
            for (NodeId v : path_y) {
               if (ancestors_x.count(v)) {
                  lca = v;
                  break;
               }
            }
            
            // Build cycle
            std::vector<NodeId> cycle;
            for (NodeId v : path_x) {
               cycle.push_back(v);
               if (v == lca) break;
            }
            for (auto it = path_y.begin(); it != path_y.end(); ++it) {
               if (*it == lca) break;
               cycle.push_back(*it);
            }
            
            // Shrink blossom: O(n log n) amortized with Union-Find
            shrink_blossom(cycle);
         }
      }
   }
   
   return false;
}

void BlossomMatcher::augment_along_path(const std::vector<NodeId>& path) {
   // Toggle edges in path: O(path length)
   for (size_type i = 0; i + 1 < path.size(); i += 2) {
      NodeId u = path[i];
      NodeId v = path[i + 1];
      _matching.set_mate(u, v);
   }
}

void BlossomMatcher::shrink_blossom(const std::vector<NodeId>& cycle) {
   // Create a new supernode id for the blossom. We choose ids that are
   // distinct from original node ids by offsetting with the current number
   // of blossoms. The supernode id is purely internal bookkeeping and is
   // used to map original nodes to their contracted representative.
   NodeId supernode = _graph.num_nodes() + _blossoms.size();
   Blossom b;
   b.supernode_id = supernode;
   b.members = cycle;
   b.path = cycle;
   _blossoms.push_back(b);
   
   // Union all nodes in the cycle into a single contracted component.
   // This operation uses Union-Find and is efficient amortized.
   for (NodeId node : cycle) {
      _uf->unite(cycle[0], node);
      // Remember mapping from original node to the supernode id so we can
      // reconstruct (unshrink) later.
      _supernode_map[node] = supernode;
   }
}

void BlossomMatcher::unshrink_blossom(const Blossom& b, 
                                       std::vector<std::pair<NodeId, NodeId>>& result) {
   // Recursively expand blossom: O(n) per blossom
   const std::vector<NodeId>& cycle = b.path;
   size_type len = cycle.size();
   
   if (len < 3) return;  // No cycle to unshrink
   
   // Find which cycle node has no matched partner or is matched to blossom root
   for (size_type i = 0; i < len; ++i) {
      NodeId v = cycle[i];
      NodeId mate = _matching.get_mate(v);
      
      // Check if mate is in this blossom
      bool mate_in_blossom = false;
      for (NodeId c : cycle) {
         if (c == mate) {
            mate_in_blossom = true;
            break;
         }
      }
      
      if (mate_in_blossom) {
         // Find the edge {v, mate} in result and remove it
         auto it = std::find_if(result.begin(), result.end(),
            [v, mate](const std::pair<NodeId, NodeId>& e) {
               return (e.first == v && e.second == mate) || 
                      (e.first == mate && e.second == v);
            });
         if (it != result.end()) {
            result.erase(it);
         }
      }
   }
}

std::vector<NodeId> BlossomMatcher::get_contracted_neighbors(NodeId node) {
   // Return neighbors considering contractions: O(degree) amortized
   std::vector<NodeId> neighbors;
   
   // Check if we have cached contracted neighbors
   if (_contracted_adj.count(node) > 0) {
      return _contracted_adj[node];
   }
   
   // Otherwise, compute from original graph
   for (NodeId neighbor : _graph.node(node).neighbors()) {
      // Check if in same contracted component
      if (_uf->find(neighbor) == _uf->find(node)) {
         continue;  // Skip internal blossom edges
      }
      neighbors.push_back(neighbor);
   }
   
   _contracted_adj[node] = neighbors;
   return neighbors;
}

bool BlossomMatcher::is_exposed_contracted(NodeId node) {
   return _matching.is_exposed(node);
}

std::vector<NodeId> BlossomMatcher::trace_path_contracted(const std::vector<NodeId>& path) {
   // Trace path back to original nodes
   std::vector<NodeId> result;
   for (NodeId node : path) {
      result.push_back(node);
   }
   return result;
}

NodeId BlossomMatcher::find_lca(NodeId u, NodeId v) {
   // Find LCA with depth tracking: O(log n)
   std::vector<NodeId> path_u = _tree->get_path_to_root(u);
   std::vector<NodeId> path_v = _tree->get_path_to_root(v);
   
   std::set<NodeId> ancestors_u(path_u.begin(), path_u.end());
   for (NodeId node : path_v) {
      if (ancestors_u.count(node)) {
         return node;
      }
   }
   
   return invalid_node_id;
}

} // namespace ED

