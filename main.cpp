#include <iostream>
#include <cstdlib>

#include "graph.hpp"
#include "EdmondsMatching.hpp"

int main(int argc, char** argv)
{
   if (argc != 2)
   {
      std::cerr << "Wrong number of arguments. Program call: <program_name> <input_graph>" << std::endl;
      return EXIT_FAILURE;
   }

   ED::Graph graph = ED::Graph::build_graph(argv[1]);

   // Compute a (perfect) matching using Edmonds' algorithm implemented in EdmondsMatching
   EdmondsMatching solver(graph);
   auto matching = solver.compute_perfect_matching();

   // If no perfect matching exists, print a comment and exit with success (no DIMACS edges produced)
   if (matching.empty()) {
       std::cout << "c No perfect matching found\n";
       return EXIT_SUCCESS;
   }

   // Print matching as a DIMACS subgraph: header 'p edge <num_nodes> <num_edges>' followed by 'e u v' lines
   std::cout << "p edge " << graph.num_nodes() << " " << matching.size() << "\n";
   for (auto const & e : matching) {
       // DIMACS nodes are 1-based
       std::cout << "e " << (e.first + 1) << " " << (e.second + 1) << "\n";
   }

   return EXIT_SUCCESS;
}
