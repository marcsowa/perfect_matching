#ifndef BLOSSOM_H
#define BLOSSOM_H

/**
   @file Blossom.h

   @brief This file provides a simple struct @c Blossom to model a blossom in Edmonds' matching algorithm.
**/

#pragma once
#include "graph.hpp"
using namespace ED;

struct Blossom {
    NodeId base;                    // closest vertex to tree root
    std::vector<NodeId> cycle;      // vertices of odd cycle
};



#endif /* BLOSSOM_H */
