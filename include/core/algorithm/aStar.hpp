#pragma once
#include "core/graph.hpp"
#include "ui/snapshot.hpp"
#include <queue>
#include <unordered_map>

namespace algorithm {

struct AStarNode {
    core::State state;
    int g;
    int f;
    char move;
};

struct AStarCompare {
    bool operator()(const AStarNode& a, const AStarNode& b) const {
        return a.f > b.f;
    }
};

ui::SolutionLog aStar(core::Graph& graph);

} // namespace algorithm