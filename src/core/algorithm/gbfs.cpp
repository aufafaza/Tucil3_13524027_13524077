#include "core/algorithm/gbfs.hpp"
#include <chrono>
#include <queue>
#include <unordered_map>
#include <cmath>

namespace algorithm {

// heuristik: manhattan distance checkpoint aware
static int manhattan(const core::State& state, const core::Graph& graph) {
    const auto& checkpoints = graph.getCheckpointPositions();
    int totalCp = graph.getCheckpointCount();

    
    core::Point target = graph.getGoalPos();
    for (int i = 0; i < totalCp; i++) {
        if (!(state.visitedCheckpoints & (1 << i))) {
            target = checkpoints[i];
            break;
        }
    }

    return std::abs(state.pos.row - target.row)
         + std::abs(state.pos.col - target.col);
}

struct GBFSNode {
    core::State state;
    int hCost;  // h(n)

    
    bool operator>(const GBFSNode& other) const {
        return hCost > other.hCost;
    }
};

ui::SolutionLog runGBFS(core::Graph& graph) {
    auto startTime = std::chrono::high_resolution_clock::now();

    core::State initState = graph.getInitialState();


    std::priority_queue<GBFSNode, std::vector<GBFSNode>, std::greater<GBFSNode>> pq;
    std::unordered_map<core::State, core::State, core::StateHash> parent;
    std::unordered_map<core::State, char, core::StateHash> parentMove;

    int initH = manhattan(initState, graph);
    pq.push({initState, initH});
    parent[initState] = initState;
    parentMove[initState] = 'S';

    core::State goalState = initState;
    bool found = false;
    int iterations = 0;

    while (!pq.empty() && !found) {
        GBFSNode cur = pq.top();
        pq.pop();
        iterations++;

        if (graph.isGoal(cur.state)) {
            goalState = cur.state;
            found = true;
            break;
        }

        for (const auto& succ : graph.getSuccessors(cur.state)) {
            
            if (parent.find(succ.state) == parent.end()) {
                parent[succ.state] = cur.state;
                parentMove[succ.state] = succ.move;
                pq.push({succ.state, manhattan(succ.state, graph)});
            }
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    long long execMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    ui::SolutionLog log;
    log.algorithmName = "GBFS";
    log.found = found;
    log.iterations = iterations;
    log.executionTimeMs = execMs;

    if (found) {
        std::vector<std::pair<core::State, char>> path;
        core::State cur = goalState;

        while (!(cur == initState)) {
            char move = parentMove[cur];
            core::State par = parent[cur];
            path.push_back({cur, move});
            cur = par;
        }
        std::reverse(path.begin(), path.end());

        log.snapshots.push_back({initState.pos, initState.visitedCheckpoints, 'S', 0});

        int stepNum = 1;
        for (auto& [st, mv] : path) {
            log.snapshots.push_back({st.pos, st.visitedCheckpoints, mv, stepNum++});
        }

        log.totalSteps = static_cast<int>(path.size());
    } else {
        log.snapshots.push_back({initState.pos, initState.visitedCheckpoints, 'S', 0});
        log.totalSteps = 0;
    }

    return log;
}

} // namespace algorithm