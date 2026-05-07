/*
g++ -std=c++17 -I include/ tests/test_main.cpp src/utils/fileIO.cpp src/core/graph.cpp src/core/algorithm/gbfs.cpp -o test_gbfs
*/
#include "utils/fileIO.hpp"
#include "core/graph.hpp"
#include "core/algorithm/ucs.hpp"
#include "core/algorithm/gbfs.hpp"
#include <iostream>
#include <string>

void printBoard(const core::Graph& graph, const ui::Snapshot& snap) {
    int rows = graph.getRows();
    int cols = graph.getCols();
    const auto& checkpoints = graph.getCheckpointPositions();
    core::Point startPos = graph.getStartPos();

    for (int r = 0; r < rows; r++) {
        std::cout << "  ";
        for (int c = 0; c < cols; c++) {
            // Tampilkan Z hanya di posisi player saat ini
            if (r == snap.playerPos.row && c == snap.playerPos.col) {
                std::cout << 'Z';
                continue;
            }
            // Posisi start asli tampilkan sebagai '*' jika player sudah pindah
            if (r == startPos.row && c == startPos.col) {
                std::cout << '*';
                continue;
            }
            // Checkpoint: '.' jika sudah dikunjungi, angka asli jika belum
            bool printedCp = false;
            for (int i = 0; i < (int)checkpoints.size(); i++) {
                if (checkpoints[i].row == r && checkpoints[i].col == c) {
                    char display = (snap.visitedCheckpoints & (1 << i)) ? '.' : graph.getTile(r, c);
                    std::cout << display;
                    printedCp = true;
                    break;
                }
            }
            if (!printedCp) std::cout << graph.getTile(r, c);
        }
        std::cout << '\n';
    }
}

int calcTotalCost(core::Graph& graph, const ui::SolutionLog& log) {
    if (!log.found || log.snapshots.size() < 2) return -1;

    int total = 0;
    for (int i = 0; i + 1 < (int)log.snapshots.size(); i++) {
        core::State from = {log.snapshots[i].playerPos, log.snapshots[i].visitedCheckpoints};
        core::State to   = {log.snapshots[i+1].playerPos, log.snapshots[i+1].visitedCheckpoints};
        char mv = log.snapshots[i+1].move;

        for (const auto& succ : graph.getSuccessors(from)) {
            if (succ.state == to && succ.move == mv) {
                total += succ.cost;
                break;
            }
        }
    }
    return total;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <puzzle_file.txt>\n";
        return 1;
    }

    const std::string filepath = argv[1];

    std::cout << "==============================================\n";
    std::cout << "  GBFS Solver — Ice Sliding Puzzle\n";
    std::cout << "==============================================\n";
    std::cout << "File: " << filepath << "\n\n";

    fileUtil::PuzzleData pz = fileUtil::fileIO::readFile(filepath);
    core::Graph graph(pz);

    std::cout << "Board (" << pz.rows << "x" << pz.cols << "):\n";
    core::State init = graph.getInitialState();
    ui::Snapshot initSnap = {init.pos, init.visitedCheckpoints, 'S', 0};
    printBoard(graph, initSnap);

    std::cout << "\nStart      : (" << graph.getStartPos().row << ", " << graph.getStartPos().col << ")\n";
    std::cout << "Goal       : (" << graph.getGoalPos().row  << ", " << graph.getGoalPos().col  << ")\n";
    std::cout << "Checkpoints: " << graph.getCheckpointCount() << "\n";
    const auto& cps = graph.getCheckpointPositions();
    for (int i = 0; i < (int)cps.size(); i++) {
        std::cout << "  CP[" << i << "] = (" << cps[i].row << ", " << cps[i].col << ")\n";
    }

    std::cout << "\nRunning GBFS...\n";
    ui::SolutionLog log = algorithm::runGBFS(graph);

    std::cout << "\n==============================================\n";
    std::cout << "  RESULT\n";
    std::cout << "==============================================\n";
    std::cout << "Found      : " << (log.found ? "YES" : "NO") << "\n";
    std::cout << "Steps      : " << log.totalSteps << "\n";
    std::cout << "Iterations : " << log.iterations << "\n";
    std::cout << "Time       : " << log.executionTimeMs << " ms\n";

    if (log.found) {
        int totalCost = calcTotalCost(graph, log);
        std::cout << "Total cost : " << totalCost << "\n";

        std::cout << "\nMoves      : ";
        for (int i = 1; i < (int)log.snapshots.size(); i++)
            std::cout << log.snapshots[i].move;
        std::cout << "\n";

        std::cout << "\n==============================================\n";
        std::cout << "  PLAYBACK\n";
        std::cout << "==============================================\n";

        for (const auto& snap : log.snapshots) {
            if (snap.stepNumber == 0) {
                std::cout << "Initial state:\n";
            } else {
                std::cout << "Step " << snap.stepNumber
                          << " [" << snap.move << "]"
                          << "  bitmask=" << snap.visitedCheckpoints << ":\n";
            }
            printBoard(graph, snap);
            std::cout << '\n';
        }
    }

    return log.found ? 0 : 1;
}