#include "RollbackDSU.hpp"

// RollbackDSU definitions
RollbackDSU::RollbackDSU(std::size_t n) : parent(n), size(n, 1) {
    for (std::size_t i = 0; i < n; ++i) {
        parent[i] = i;
    }
}

std::size_t RollbackDSU::find(std::size_t v) const {
    // Iterative find without path compression to preserve rollback semantics and avoid recursion overhead.
    while (parent[v] != v) {
        v = parent[v];
    }
    return v;
}

void RollbackDSU::unite(std::size_t u, std::size_t v) {
    u = find(u);
    v = find(v);
    if (u == v) {
        return;
    }
    if (size[u] < size[v]) {
        std::swap(u, v);
    }
    history.push_back({v, parent[v], size[u]});
    parent[v] = u;
    size[u] += size[v];
}

std::size_t RollbackDSU::checkpoint() const {
    return history.size();
}

void RollbackDSU::rollback(std::size_t checkpoint) {
    while (history.size() > checkpoint) {
        HistoryRecord record = history.back();
        history.pop_back();
        parent[record.child] = record.parent_before;
        size[parent[record.child]] = record.size_before;
    }
}

