#include "core/algorithm/aStar.hpp"
#include "core/graph.hpp"
#include "ui/snapshot.hpp"
#include <chrono> 
#include <queue> 
#include <unordered_map> 
#include <algorithm> 

namespace algorithm {

static int heuristic(const core::State& state, const core::Graph& graph) {
    int minWeight = graph.getMinWeight(); 
    const auto& checkpoints = graph.getCheckpointPositions(); 
    int totalCp = graph.getCheckpointCount(); 
    // int allVisited = (1 << totalCp) - 1; 

    core::Point current = state.pos; 
    int totalDist = 0; 
    int visited = state.visitedCheckpoints; 

    for (int i = 0; i < totalCp; i++){ 
        if (visited & (1 << i)) continue; 
        totalDist += std::abs(current.row - checkpoints[i].row)
                + std::abs(current.col - checkpoints[i].col); 
        current = checkpoints[i];
    }
    totalDist += std::abs(current.row - graph.getGoalPos().row) + std::abs(current.col - graph.getGoalPos().col); 

    return minWeight * totalDist; 
}


struct ACompare {   
    bool operator()(const AStarNode& a, const AStarNode& b) const { 
        return a.f > b.f;
    } 
};

ui::SolutionLog aStar(core::Graph& graph){ 
    auto startTime = std::chrono::high_resolution_clock::now(); 
    core::State initState = graph.getInitialState(); 

    std::priority_queue<AStarNode, std::vector<AStarNode>, ACompare> openList;
    core::StateSet closedList; 
    std::unordered_map<core::State, int, core::StateHash> gScore; 
    std::unordered_map<core::State, core::State, core::StateHash> cameFrom; 
    std::unordered_map<core::State, char, core::StateHash> cameFromMove;

    int initH = heuristic(initState, graph); 
    openList.push({initState, 0,  initH, 'S'}); 
    gScore[initState] = 0; 
    cameFrom[initState] = initState; 
    cameFromMove[initState] = 'S'; 
    core::State goalState = initState;
    bool found = false; 
    int iter = 0; 

    while (!openList.empty() && !found){ 
        AStarNode cur = openList.top(); 
        openList.pop(); 
        iter++; 

        auto g = gScore.find(cur.state); 
        // skip stale g
        if (g != gScore.end() && cur.g > g->second){
            continue;
        } 

        // if alreaday expanded 
        if (closedList.count(cur.state)){
            continue;
        }
        closedList.insert(cur.state); 

        if (graph.isGoal(cur.state)){
            goalState = cur.state; 
            found = true; 
            break; 
        } 

        for (const auto& succ : graph.getSuccessors(cur.state)){
            if (closedList.count(succ.state)){
                continue;
            }

            int possibleG = cur.g + succ.cost; 
            auto succG = gScore.find(succ.state); 

            // if new state 
            if (succG == gScore.end() || possibleG < succG->second){
                gScore[succ.state] = possibleG; 
                cameFrom[succ.state] = cur.state; 
                cameFromMove[succ.state] = succ.move; 
                // calculate f 
                int f = possibleG + heuristic(succ.state, graph); 
                openList.push({succ.state, possibleG, f, succ.move}); 
            }
        }
    }
    auto endTime = std::chrono::high_resolution_clock::now(); 
    auto execMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    ui::SolutionLog log; 
    log.algorithmName = "A*"; 
    log.found = found; 
    log.iterations = iter; 
    log.executionTimeMs = execMs; 

    if (found) { 
        std::vector<std::pair<core::State, char>> path;
        core::State cur = goalState; 
        while (!(cur == initState)){
            char move = cameFromMove[cur];
            core::State par = cameFrom[cur]; 
            path.push_back({cur, move}); 
            cur = par; 
        } 

        std::reverse(path.begin(), path.end()); 

        log.snapshots.push_back({initState.pos, initState.visitedCheckpoints, 'S', 0}); 

        int stepNum = 1; 
        for (auto& [state, move] : path){ 
            log.snapshots.push_back({state.pos, state.visitedCheckpoints, move, stepNum++});
        }
        log.totalSteps = static_cast<int>(path.size()); 
    }else{
        log.snapshots.push_back({initState.pos, initState.visitedCheckpoints, 'S', 0}); 
        log.totalSteps = 0; 
    } 

    return log; 
    }
} // namespace algorithm
