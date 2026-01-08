#include "matching.hpp"

namespace ED {

//============ Matching Implementation ============

Matching::Matching(NodeId num_nodes)
   : _mate(num_nodes, invalid_node_id), _match_size(0)
{}

bool Matching::is_exposed(NodeId node) const {
   return _mate[node] == invalid_node_id;
}

NodeId Matching::get_mate(NodeId node) const {
   return _mate[node];
}

void Matching::set_mate(NodeId u, NodeId v) {
   // Update matching information for edge u-v
   if (is_exposed(u) && is_exposed(v)) {
      _match_size++;
   }
   _mate[u] = v;
   _mate[v] = u;
}


std::vector<std::pair<NodeId, NodeId>> Matching::get_edges() const {
   std::vector<std::pair<NodeId, NodeId>> edges;
   for (NodeId u = 0; u < _mate.size(); ++u) {
      if (_mate[u] != invalid_node_id && u < _mate[u]) {
         edges.push_back({u, _mate[u]});
      }
   }
   return edges;
}

bool Matching::is_perfect(NodeId num_nodes) const {
   return _match_size * 2 == num_nodes;
}

//============ AlternatingTree Implementation ============

AlternatingTree::AlternatingTree(NodeId root, NodeId num_nodes)
   : _root(root), _in_tree(num_nodes, false), _depth(num_nodes, -1),
     _parent(num_nodes, invalid_node_id)
{
   _in_tree[root] = true;
   _depth[root] = 0;
}

bool AlternatingTree::is_even(NodeId node) const {
   if (!_in_tree[node]) return false;
   return _depth[node] % 2 == 0;
}

bool AlternatingTree::is_odd(NodeId node) const {
   if (!_in_tree[node]) return false;
   return _depth[node] % 2 == 1;
}

void AlternatingTree::add_even_node(NodeId node, NodeId parent) {
   _in_tree[node] = true;
   _parent[node] = parent;
   _depth[node] = _depth[parent] + 1;
}

void AlternatingTree::add_odd_node(NodeId node, NodeId parent) {
   _in_tree[node] = true;
   _parent[node] = parent;
   _depth[node] = _depth[parent] + 1;
}

std::vector<NodeId> AlternatingTree::get_path_to_root(NodeId node) const {
   std::vector<NodeId> path;
   NodeId curr = node;
   while (curr != invalid_node_id) {
      path.push_back(curr);
      curr = _parent[curr];
   }
   return path;
}

void AlternatingTree::clear() {
   std::fill(_in_tree.begin(), _in_tree.end(), false);
   std::fill(_depth.begin(), _depth.end(), -1);
   std::fill(_parent.begin(), _parent.end(), invalid_node_id);
}

//============ UnionFind Implementation ============

UnionFind::UnionFind(NodeId num_nodes)
   : _parent(num_nodes), _rank(num_nodes, 0)
{
   for (NodeId i = 0; i < num_nodes; ++i) {
      _parent[i] = i;
   }
}

NodeId UnionFind::find(NodeId node) const {
   if (_parent[node] != node) {
      _parent[node] = find(_parent[node]);  // Path compression
   }
   return _parent[node];
}

void UnionFind::unite(NodeId u, NodeId v) {
   u = find(u);
   v = find(v);
   if (u == v) return;
   
   // Union by rank
   if (_rank[u] < _rank[v]) std::swap(u, v);
   _parent[v] = u;
   if (_rank[u] == _rank[v]) _rank[u]++;
}

std::set<NodeId> UnionFind::get_component(NodeId node) const {
   std::set<NodeId> component;
   NodeId root = find(node);
   for (NodeId i = 0; i < _parent.size(); ++i) {
      if (find(i) == root) {
         component.insert(i);
      }
   }
   return component;
}

} // namespace ED
