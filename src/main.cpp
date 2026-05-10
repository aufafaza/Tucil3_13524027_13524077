#include "utils/fileIO.hpp"
#include "core/graph.hpp"
#include "ui/snapshot.hpp"
#include "ui/gameui.hpp"
#include "ui/grideditor.hpp"
#include "core/algorithm/aStar.hpp"
#include "core/algorithm/ucs.hpp"
#include "core/algorithm/gbfs.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

static std::string generateLogFilename(const std::string& algoName) {
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm* lt = std::localtime(&nowTime);
    std::ostringstream oss;
    oss << "log_" << algoName
        << "_" << std::setfill('0') << std::setw(4) << (lt->tm_year + 1900)
        << std::setw(2) << (lt->tm_mon + 1)
        << std::setw(2) << lt->tm_mday
        << "_" << std::setw(2) << lt->tm_hour
        << std::setw(2) << lt->tm_min
        << std::setw(2) << lt->tm_sec
        << ".txt";
    return oss.str();
}

static void saveLog(const ui::SolutionLog& log, const fileUtil::PuzzleData& pz) {
    fileUtil::SolutionLogData data;
    data.algorithmName = log.algorithmName;
    data.totalSteps = log.totalSteps;
    data.iterations = log.iterations;
    data.executionTimeMs = log.executionTimeMs;
    data.found = log.found;
    data.rows = pz.rows;
    data.cols = pz.cols;
    data.board = pz.board;
    data.cost = pz.cost;
    for (const auto& snap : log.snapshots) {
        data.snapshots.push_back({
            snap.playerPos.row,
            snap.playerPos.col,
            snap.visitedCheckpoints,
            snap.move,
            snap.stepNumber
        });
    }
    std::string filename = generateLogFilename(log.algorithmName);
    fileUtil::fileIO::writeSolutionLog(filename, data);
    std::cout << "Log saved: " << filename << std::endl;
}

static ui::SolutionLog solveWithAlgorithm(core::Graph& graph, ui::AlgorithmChoice algo) {
    switch (algo) {
        case ui::AlgorithmChoice::UCS:
            return algorithm::runUCS(graph);
        case ui::AlgorithmChoice::ASTAR:
            return algorithm::aStar(graph);
        case ui::AlgorithmChoice::GBFS:
            return algorithm::runGBFS(graph);
        default:
            return algorithm::aStar(graph);
    }
}

static void runGame(const fileUtil::PuzzleData& pz, ui::AlgorithmChoice algo) {
    core::Graph graph(pz);
    ui::SolutionLog log = solveWithAlgorithm(graph, algo);

    std::cout << "Algorithm: " << log.algorithmName << std::endl;
    std::cout << "Found: " << (log.found ? "YES" : "NO") << std::endl;
    std::cout << "Steps: " << log.totalSteps << std::endl;
    std::cout << "Iterations: " << log.iterations << std::endl;
    std::cout << "Time: " << log.executionTimeMs << " ms" << std::endl;

    saveLog(log, pz);

    ui::RenderConfig config;
    config.cellSize = 80;
    ui::GameUI ui(graph, config);
    ui.run(log);
}

static ui::AlgorithmChoice chooseAlgorithm() {
    std::cout << "\nSelect algorithm:" << std::endl;
    std::cout << "1. UCS  (Uniform Cost Search)" << std::endl;
    std::cout << "2. A*   (A* Search)" << std::endl;
    std::cout << "3. GBFS (Greedy Best-First Search)" << std::endl;
    std::cout << "Choice [1-3]: ";

    int choice = 2;
    std::cin >> choice;
    switch (choice) {
        case 1: return ui::AlgorithmChoice::UCS;
        case 3: return ui::AlgorithmChoice::GBFS;
        default: return ui::AlgorithmChoice::ASTAR;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        while (true) {
            ui::GridEditor editor;
            bool play = editor.run();
            if (!play) break;

            ui::AlgorithmChoice algo = editor.getSelectedAlgorithm();
            if (algo == ui::AlgorithmChoice::NONE) {
                algo = ui::AlgorithmChoice::ASTAR;
            }

            fileUtil::PuzzleData pz = editor.getResult();
            runGame(pz, algo);
        }
        return 0;
    }

    const std::string file_loc = argv[1];
    fileUtil::PuzzleData pz = fileUtil::fileIO::readFile(file_loc);

    ui::AlgorithmChoice algo = chooseAlgorithm();
    runGame(pz, algo);
    return 0;
}
