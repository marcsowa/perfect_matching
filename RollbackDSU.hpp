#ifndef ROLLBACKDSU_HPP
#define ROLLBACKDSU_HPP

/**
   @file RollbackDSU.hpp

   @brief This file provides a simple class @c RollbackDSU to model a disjoint set union (DSU) data structure with rollback capabilities. This will be used to model the blossom shrinking and ushrinking.
**/

#pragma once
#include "graph.hpp"
using namespace ED;


class RollbackDSU {
public:
    explicit RollbackDSU(std::size_t n);

    std::size_t find(std::size_t v) const;
    void unite(std::size_t u, std::size_t v);

    std::size_t checkpoint() const;
    void rollback(std::size_t checkpoint);

private:
    struct HistoryRecord {
        std::size_t child;
        std::size_t parent_before;
        std::size_t size_before;
    };

    std::vector<std::size_t> parent;
    std::vector<std::size_t> size;
    std::vector<HistoryRecord> history;
};


#endif /* ROLLBACKDSU_HPP */