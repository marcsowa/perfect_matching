#ifndef MATCHING_H
#define MATCHING_H

/**
   @file Matching.h

   @brief This file provides matching-related functionalities.
**/

#pragma once
#include "graph.hpp"
using namespace ED;

class Matching {
public:
    explicit Matching(std::size_t n);

    bool is_exposed(NodeId v) const;
    NodeId mate_of(NodeId v) const;

    void match(NodeId u, NodeId v);
    void unmatch(NodeId u, NodeId v);

private:
    std::vector<NodeId> mate;
};

#endif /* MATCHING_H */