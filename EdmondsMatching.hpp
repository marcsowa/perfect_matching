#ifndef EDMONDSMATCHING_HPP
#define EDMONDSMATCHING_HPP

/**
   @file EdmondsMatching.hpp

   @brief This file provides a simple class @c EdmondsMatching to compute a perfect matching (if existing) for a given graph.
**/

#pragma once

#include <vector>
#include <queue>
#include <unordered_map>

#include "Blossom.h"
#include "Matching.h"
#include "RollbackDSU.hpp"
#include "graph.hpp"
using namespace ED;

class EdmondsMatching {
public:
    explicit EdmondsMatching(const Graph& G);

    // Empty vector = no perfect matching
    std::vector<std::pair<NodeId, NodeId>> compute_perfect_matching();

private:
    const Graph& G;

    // Matching
    std::vector<NodeId> mate;

    // Alternating tree
    enum class Parity { None, Even, Odd };
    std::vector<Parity> parity;
    std::vector<NodeId> parent;
    std::queue<NodeId> bfs_queue;

    // Blossoms
    RollbackDSU dsu;
    std::unordered_map<NodeId, Blossom> blossoms;

    // Helper for efficient LCA computation when finding blossom base: a per-node visitation mark
    std::vector<int> lca_vis;
    int lca_iter;

    // Helper methods
    void init_tree(NodeId root);
    bool bfs_augment(NodeId root);
    void augment_path(NodeId u, NodeId v);
    void shrink_blossom(NodeId x, NodeId y);
};

#endif /* EDMONDSMATCHING_HPP */