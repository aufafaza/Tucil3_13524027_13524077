#include "utils/fileIO.hpp"
#include "core/graph.hpp"
#include "ui/snapshot.hpp"
#include "ui/gameui.hpp"
#include "ui/grideditor.hpp"
#include "core/algorithm/aStar.hpp"
#include <iostream>

static void runGame(const fileUtil::PuzzleData& pz) {
    core::Graph graph(pz);
    ui::SolutionLog log = algorithm::aStar(graph);

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