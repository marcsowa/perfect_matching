#include "perfect_matching.hpp"
using namespace ED;

bool IsPerfectMatching(Graph const & graph)
{
    if(graph.num_edges() == graph.num_nodes() / 2){
        return true;
    }
    else{
        return false;
    }
    
}


Graph GetPerfectMatchingGraph(Graph const & graph)
{
    Graph matching_graph(graph.num_nodes());
    Graph alternating_tree(graph.num_nodes());
    NodeId root = 0;
    std::vector<int> partition_class(graph.num_nodes(), -1);
    for (NodeId neighbour : graph.node(root).neighbors()){}
    if (IsPerfectMatching(matching_graph)){
        //std::cout << "Perfect matching found!" << std::endl;
        return matching_graph;
    }
}
    