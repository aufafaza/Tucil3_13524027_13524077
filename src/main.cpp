#include "utils/fileIO.hpp"
#include "core/graph.hpp"
#include "ui/snapshot.hpp"
#include "ui/gameui.hpp"
#include "ui/grideditor.hpp"
#include <iostream>
#include <queue>
#include <unordered_map>

struct StateHash2 {
    size_t operator()(const core::State& s) const {
        size_t h = std::hash<int>()(s.pos.row) ^ (std::hash<int>()(s.pos.col) << 1) ^ (std::hash<int>()(s.visitedCheckpoints) << 2);
        return h;
    }
};

static ui::SolutionLog buildBFSLog(core::Graph& graph) {
    core::State init = graph.getInitialState();
    std::queue<core::State> q;
    std::unordered_map<core::State, core::State, StateHash2> parent;
    std::unordered_map<core::State, char, StateHash2> parentMove;

    q.push(init);
    parent[init] = init;
    parentMove[init] = 'S';

    core::State goalState = init;
    bool found = false;

    while (!q.empty() && !found) {
        core::State cur = q.front();
        q.pop();

        if (graph.isGoal(cur)) {
            goalState = cur;
            found = true;
            break;
        }

        auto succs = graph.getSuccessors(cur);
        for (const auto& s : succs) {
            if (parent.find(s.state) == parent.end()) {
                parent[s.state] = cur;
                parentMove[s.state] = s.move;
                q.push(s.state);
            }
        }
    }

    ui::SolutionLog log;
    log.algorithmName = "BFS-dummy";
    log.found = found;

    if (found) {
        std::vector<std::pair<core::State, char>> path;
        core::State cur = goalState;
        while (!(cur.pos.row == init.pos.row && cur.pos.col == init.pos.col && cur.visitedCheckpoints == init.visitedCheckpoints)) {
            char m = parentMove[cur];
            cur = parent[cur];
            path.push_back({cur, m});
        }
        std::reverse(path.begin(), path.end());

        log.snapshots.push_back({init.pos, init.visitedCheckpoints, 'S', 0});
        int stepNum = 1;
        for (auto& [st, mv] : path) {
            log.snapshots.push_back({st.pos, st.visitedCheckpoints, mv, stepNum++});
        }

        log.totalSteps = static_cast<int>(log.snapshots.size());
        log.iterations = static_cast<int>(parent.size());
    } else {
        core::State cur = init;
        log.snapshots.push_back({cur.pos, cur.visitedCheckpoints, 'S', 0});
        for (int i = 0; i < 9 && !graph.isGoal(cur); i++) {
            auto succs = graph.getSuccessors(cur);
            if (succs.empty()) break;
            cur = succs[0].state;
            log.snapshots.push_back({cur.pos, cur.visitedCheckpoints, succs[0].move, i + 1});
        }
        log.totalSteps = static_cast<int>(log.snapshots.size());
        log.iterations = log.totalSteps;
    }

    log.executionTimeMs = 0;
    return log;
}

static void runGame(const fileUtil::PuzzleData& pz) {
    core::Graph graph(pz);
    ui::SolutionLog log = buildBFSLog(graph);

    ui::RenderConfig config;
    config.cellSize = 80;
    ui::GameUI ui(graph, config);
    ui.run(log);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        while (true) {
            ui::GridEditor editor;
            bool play = editor.run();
            if (!play) break;

            fileUtil::PuzzleData pz = editor.getResult();
            runGame(pz);
        }
        return 0;
    }

    const std::string file_loc = argv[1];
    fileUtil::PuzzleData pz = fileUtil::fileIO::readFile(file_loc);
    runGame(pz);
    return 0;
}