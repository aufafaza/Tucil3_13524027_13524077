#include "core/algorithm/ucs.hpp"
#include <chrono>
#include <queue>
#include <unordered_map>

namespace algorithm {

struct UCSNode {
    core::State state;
    int gCost;  // g(n)
    bool operator>(const UCSNode& other) const {
        return gCost > other.gCost;
    }
};

ui::SolutionLog runUCS(core::Graph& graph) {
    auto startTime = std::chrono::high_resolution_clock::now();

    core::State initState = graph.getInitialState();
    std::priority_queue<UCSNode, std::vector<UCSNode>, std::greater<UCSNode>> pq;
    std::unordered_map<core::State, int, core::StateHash> bestCost;
    std::unordered_map<core::State, core::State, core::StateHash> parent;
    std::unordered_map<core::State, char, core::StateHash> parentMove;

    pq.push({initState, 0});
    bestCost[initState] = 0;
    parent[initState] = initState;
    parentMove[initState] = 'S';

    core::State goalState = initState;
    bool found = false;
    int iterations = 0;

    while (!pq.empty() && !found) {
        UCSNode cur = pq.top();
        pq.pop();
        iterations++;
        if (cur.gCost > bestCost[cur.state]) {
            continue;
        }

        if (graph.isGoal(cur.state)) {
            goalState = cur.state;
            found = true;
            break;
        }

        for (const auto& succ : graph.getSuccessors(cur.state)) {
            int newCost = cur.gCost + succ.cost;

            auto it = bestCost.find(succ.state);
            if (it == bestCost.end() || newCost < it->second) {
                bestCost[succ.state] = newCost;
                parent[succ.state] = cur.state;
                parentMove[succ.state] = succ.move;
                pq.push({succ.state, newCost});
            }
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    long long execMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    ui::SolutionLog log;
    log.algorithmName = "UCS";
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
        log.snapshots.push_back({
            initState.pos,
            initState.visitedCheckpoints,
            'S',
            0
        });

        int stepNum = 1;
        for (auto& [st, mv] : path) {
            log.snapshots.push_back({st.pos, st.visitedCheckpoints, mv, stepNum++});
        }
        log.totalSteps = static_cast<int>(path.size());
    } else {
        log.snapshots.push_back({
            initState.pos,
            initState.visitedCheckpoints,
            'S',
            0
        });
        log.totalSteps = 0;
    }

    return log;
}

} // namespace algorithm