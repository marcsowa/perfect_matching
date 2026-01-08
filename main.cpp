#include <iostream>
#include <cstdlib>

#include "graph.hpp"
#include "blossom.hpp"

int main(int argc, char** argv)
{
   // Simple command-line interface:
   // Usage: ./edmonds.out <input_file>.dmx
   // The program reads the given DIMACS file, runs Edmonds' algorithm, and
   // prints either the DIMACS encoding of the matching subgraph or the exact
   // string "No perfect matching" if none exists.
   if (argc != 2)
   {
      std::cerr << "Usage: " << argv[0] << " <input_graph>.dmx" << std::endl;
      return EXIT_FAILURE;
   }

   try {
      ED::Graph graph = ED::Graph::build_graph(argv[1]);
      ED::BlossomMatcher matcher(graph);
      
      auto matching = matcher.find_perfect_matching();
      
      if (matching.empty()) {
         // Check if graph is perfect (all nodes matched in result)
         bool is_perfect = (matching.size() * 2 == graph.num_nodes());
         if (!is_perfect) {
            std::cout << "No perfect matching" << std::endl;
            return EXIT_SUCCESS;
         }
      }
      
      // Output DIMACS format: subgraph with matching edges only
      std::cout << "p edge " << graph.num_nodes() << " " << matching.size() << std::endl;
      for (const auto& edge : matching) {
         ED::DimacsId du = ED::to_dimacs_id(edge.first);
         ED::DimacsId dv = ED::to_dimacs_id(edge.second);
         std::cout << "e " << du << " " << dv << std::endl;
      }
      
   } catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
      return EXIT_FAILURE;
   }
   
   return EXIT_SUCCESS;
}
