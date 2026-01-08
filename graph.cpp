#include "graph.hpp" // always include corresponding header first

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>


namespace ED
{
/////////////////////////////////////////////
//! \c Node definitions
/////////////////////////////////////////////

void Node::add_neighbor(NodeId const id)
{
   _neighbors.push_back(id);
}

/////////////////////////////////////////////
//! \c Graph definitions
/////////////////////////////////////////////


Graph Graph::build_graph(const std::string & filename)
{
   std::ifstream ifs(filename);
   if (!ifs.is_open())
   {
      throw std::runtime_error("Could not open input file.");
   }

   std::string line;

   // Skip leading comment lines that start with 'c'
   do
   {
      if (!std::getline(ifs, line))
      {
         throw std::runtime_error("Could not find problem line in DIMACS stream.");
      }
   }
   while (line[0] == 'c');

   int num_nodes = 0;
   int num_edges = 0;

   // Expect a problem line of the form: p edge n m
   if (line[0] == 'p')
   {
      std::stringstream stream;
      stream << line;
      std::string tag1, tag2;
      stream >> tag1 >> tag2 >> num_nodes >> num_edges;
      // Accept "edge" in any case (DIMACS files sometimes use uppercase)
      std::transform(tag2.begin(), tag2.end(), tag2.begin(), [](unsigned char c){ return std::tolower(c); });
      if (tag2 != "edge") {
         throw std::runtime_error(std::string("Unsupported problem type in DIMACS stream (expected 'edge'). Found: '") + tag2 + "'");
      }
      if (num_nodes < 0 || num_edges < 0) {
         throw std::runtime_error("Invalid node/edge counts in DIMACS problem line.");
      }
   }
   else
   {
      throw std::runtime_error("Unexpected format of input file: missing problem line.");
   }

   Graph graph(num_nodes);
   while (std::getline(ifs, line))
   {
      if (line.empty() or line[0] != 'e')
      {
         continue;
      }
      std::stringstream stream;
      stream << line;
      char c;
      DimacsId i;
      DimacsId j;
      stream >> c >> i >> j;
      graph.add_edge(from_dimacs_id(i), from_dimacs_id(j));
   }

   return graph;
}

Graph::Graph(NodeId const num_nodes)
   :
   _nodes(num_nodes),
   _num_edges(0)
{}

void Graph::add_edge(NodeId node1_id, NodeId node2_id)
{
   if (node1_id == node2_id)
   {
      throw std::runtime_error("ED::Graph class does not support loops!");
   }

   // Helper lambda: add b as a neighbor of a (used to add both directions)
   auto impl = [this](NodeId a, NodeId b)
   {
      Node & node = _nodes.at(a);
      node.add_neighbor(b);
   };

   impl(node1_id, node2_id);
   impl(node2_id, node1_id);

   ++_num_edges;
}

std::ostream & operator<<(std::ostream & str, Graph const & graph)
{
   str << "c This encodes a graph in DIMACS format\n"
       << "p edge " << graph.num_nodes() << " " << graph.num_edges() << "\n";

   for (NodeId node_id = 0; node_id < graph.num_nodes(); ++node_id)
   {
      auto const & node = graph.node(node_id);

      for (auto const & neighbor_id : node.neighbors())
      {
         // output each edge only once
         if (node_id < neighbor_id)
         {
            str << "e " << to_dimacs_id(node_id) << " " << to_dimacs_id(neighbor_id) << "\n";
         }
      }
   }

   str << std::flush;
   return str;
}


/////////////////////////////////////////////
//! global functions
/////////////////////////////////////////////

NodeId from_dimacs_id(DimacsId const dimacs_id)
{
   if (dimacs_id == 0)
   {
      throw std::runtime_error("Invalid (0) DIMACS id.");
   }

   return static_cast<NodeId>(dimacs_id - 1);
}

DimacsId to_dimacs_id(NodeId const node_id)
{
   if (node_id == std::numeric_limits<NodeId>::max())
   {
      throw std::runtime_error("Invalid (inf) node id.");
   }

   return static_cast<DimacsId>(node_id + 1);
}

} // namespace ED
