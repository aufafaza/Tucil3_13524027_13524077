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