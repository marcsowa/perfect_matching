#include <iostream>
#include <cstdlib>

#include "graph.hpp"
#include "perfect_matching.hpp"


int main(int argc, char** argv)
{
   if (argc != 2)
   {
      std::cerr << "Wrong number of arguments. Program call: <program_name> <input_graph>" << std::endl;
      return EXIT_FAILURE;
   }

   ED::Graph graph = ED::Graph::build_graph(argv[1]);
   std::cout << graph;
   //ED::Graph matching_graph = GetPerfectMatchingGraph(graph);
   //std::cout << matching_graph;
   return EXIT_SUCCESS;
}
