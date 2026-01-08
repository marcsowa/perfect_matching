#include "EdmondsMatching.hpp"
#include <iostream>


// Constructor of EdmondsMatching class
EdmondsMatching::EdmondsMatching(const Graph& G) : G(G), mate(G.num_nodes(), invalid_node_id), parity(G.num_nodes(), Parity::None), parent(G.num_nodes(), invalid_node_id), dsu(G.num_nodes()), lca_vis(G.num_nodes(), 0), lca_iter(0) {}

// Implementation of the main algorithm
std::vector<std::pair<NodeId, NodeId>> EdmondsMatching::compute_perfect_matching() {
    for (NodeId v = 0; v < G.num_nodes(); ++v) {
        if (mate[v] == invalid_node_id) {       // if there is an exposed vertex
            init_tree(v);                       // initialize alternating tree with root v
            // Try to find an augmenting path starting from this root. If found, the matching grows.
            // If not found, we continue to other roots; only after all roots are tried do we decide
            // whether a perfect matching exists.
            (void)bfs_augment(v);
        }
    }

    // Construct the matching result
    std::vector<std::pair<NodeId, NodeId>> matching;
    for (NodeId v = 0; v < G.num_nodes(); ++v) {
        if (mate[v] != invalid_node_id && v < mate[v]) {
            matching.emplace_back(v, mate[v]);
        }
    }
    return matching;
}

// HELPER METHODS

// Creates alternating tree rooted at 'root'
void EdmondsMatching::init_tree(NodeId root) {
    std::fill(parity.begin(), parity.end(), Parity::None);      // reset parities
    std::fill(parent.begin(), parent.end(), invalid_node_id);   // reset parents
    while (!bfs_queue.empty()) {                                // clear BFS queue
        bfs_queue.pop();
    }

    parity[root] = Parity::Even;                                // mark root as even
    bfs_queue.push(root);                                       // insert root into BFS queue
}


// Performs breadth-first search to get alternating tree and find an augmenting path
bool EdmondsMatching::bfs_augment(NodeId root) {
    (void)root; // parameter unused in current implementation; keep name for API clarity
    while (!bfs_queue.empty()) {
        NodeId u = bfs_queue.front();
        bfs_queue.pop();

        for (NodeId v : G.node(u).neighbors()) {
            if (parity[v] == Parity::None) {
                // Unvisited neighbor: we set its parent and mark it as odd (it is reached from an even vertex u).
                parent[v] = u;
                parity[v] = Parity::Odd;

                NodeId m = mate[v];
                if (m == invalid_node_id) {
                    // If v is exposed (no mate), we found an augmenting path from the root to v.
                    augment_path(u, v);     // augment the matching along the path from root to v
                    return true;
                } else {
                    // Otherwise, reveal the matched partner as an even vertex and continue BFS from it.
                    parity[m] = Parity::Even;
                    parent[m] = v;
                    bfs_queue.push(m);
                }
            }
            else if (parity[v] == Parity::Even && dsu.find(u) != dsu.find(v)) {
                // Found a blossom (edge between two even vertices in different DSU components), shrink it.
                shrink_blossom(u, v);
            }
        }
    }
    // If we are here there are no more vertices to explore and no augmenting path was found, i.e., no perfect matching exists
    return false;
}


// Augments the matching along the path from root to v
void EdmondsMatching::augment_path(NodeId u, NodeId v) {
    // Rename variables for clarity
    NodeId curr = u;
    NodeId next = v;

    while (curr != invalid_node_id) {
        NodeId temp = mate[curr];       // store the next vertex in the path
        mate[curr] = next;              // augment the matching
        mate[next] = curr;    
        next = temp;                    // move to the next pair in the path
        curr = parent[curr];
    }
}


// Shrinks the blossom found between vertices x and y
void EdmondsMatching::shrink_blossom(NodeId x, NodeId y) {
    /*
       High-level idea:
       - We discovered an edge between two *even* vertices x and y that belong to different DSU components.
       - This indicates the presence of an odd cycle (a blossom) whose base is the least common ancestor (LCA)
         of x and y in the alternating forest (w.r.t. the current parent pointers and DSU representatives).
       - To shrink the blossom we: (1) find the base; (2) for each side (x->base and y->base) contract the path
         into the base by uniting DSU components, updating parent pointers so the alternating-tree structure
         remains valid, flipping parities as needed, and enqueueing affected vertices for further BFS.
    */

    // Add iteration guards and diagnostics to detect potential infinite loops during debugging.
    const std::size_t max_steps = std::max<std::size_t>(1000, G.num_nodes() * 10);

    // Step 1: Find the base (LCA) of x and y using an alternating walk which is more efficient
    // than marking the entire path of x and then walking y. We use a per-call iteration marker
    // in `lca_vis` to avoid clearing the vector every time.
    ++lca_iter;
    NodeId a = x;
    NodeId b = y;
    NodeId base = invalid_node_id;
    while (true) {
        a = dsu.find(a);
        if (lca_vis[a] == lca_iter) { base = a; break; }
        lca_vis[a] = lca_iter;
        if (mate[a] == invalid_node_id) break;
        a = parent[mate[a]];

        // swap roles and continue from the other side
        std::swap(a, b);
    }

    // Defensive fallback
    if (base == invalid_node_id) {
        base = dsu.find(x);
    }

    // Prepare Blossom record for debugging/inspection: store the base and the sequence of vertices in the cycle.
    Blossom blossom;
    blossom.base = base;

    // Create a DSU checkpoint so we can rollback (unshrink) this contraction later when extracting a
    // matching for the original graph. The RollbackDSU's history will be reverted using this value.
    blossom.checkpoint = dsu.checkpoint();

    // Helper: process path from a vertex v up to (but excluding) base and shrink it.
    // 'other' is the opposite vertex (x <-> y) used to set parent pointers across the newly formed blossom.
    auto process_path = [&](NodeId v, NodeId other) {
        NodeId w = v;
        std::size_t steps2 = 0;
        // Continue until the DSU representative of w is the base (i.e., we've contracted up to base).
        while (dsu.find(w) != base) {
            if (++steps2 > max_steps) {
                std::cerr << "DBG: shrink_blossom: process_path exceeded max_steps (" << max_steps << ") v=" << v << " other=" << other << " w=" << w << " base=" << base << "\n";
                break;
            }
            NodeId m = mate[w];
            if (m == invalid_node_id) break; // safety: unmatched vertex, stop

            // Record both endpoints of the matched edge (w - m) as part of the blossom cycle.
            blossom.cycle.push_back(w);
            blossom.cycle.push_back(m);

            // Redirect parent of w so that tree bookkeeping treats the whole contracted blossom as a single node.
            // Setting parent[w] = other links the trimmed part of the path to the opposite side of the blossom,
            // maintaining correctness of subsequent BFS exploration.
            parent[w] = other;

            // If the mate (m) had odd parity it is now visible as an even vertex after contraction and must
            // be enqueued for BFS so that its incident edges are explored later.
            if (parity[m] == Parity::Odd) {
                parity[m] = Parity::Even;
                bfs_queue.push(m);
            }

            // Contract w and its mate m into the base by uniting their DSU representatives with the base.
            // After these unites all nodes in the blossom will share the same DSU rep (the base), ensuring
            // future finds treat the blossom as a single contracted super-vertex.
            dsu.unite(w, base);
            dsu.unite(m, base);

            // Move two steps up the alternating tree to continue processing the path (w <- parent[m]).
            if (parent[m] == invalid_node_id) break; // safety
            w = parent[m];
        }
    };

    // Process both sides of the detected blossom (x up to base, and y up to base).
    process_path(x, y);
    process_path(y, x);

    // Save the blossom information keyed by its base; useful for debugging or later unshrinking logic.
    blossoms[base] = blossom;
}
