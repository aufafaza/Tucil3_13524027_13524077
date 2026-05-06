#include "utils/fileIO.hpp"
#include <fstream>

namespace fileUtil {

PuzzleData fileIO::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    PuzzleData data;
    file >> data.rows >> data.cols;

    data.board.resize(data.rows, std::vector<char>(data.cols));
    data.cost.resize(data.rows, std::vector<int>(data.cols));

    for (int i = 0; i < data.rows; i++)
        for (int j = 0; j < data.cols; j++)
            file >> data.board[i][j];

    for (int i = 0; i < data.rows; i++)
        for (int j = 0; j < data.cols; j++)
            file >> data.cost[i][j];

    return data;
}

// Log format:
// line 1: algorithmName
// line 2: totalSteps iterations executionTimeMs found(0/1)
// line 3..N: playerRow playerCol visitedCheckpoints move stepNumber
void fileIO::writeSolutionLog(const std::string& filepath, const SolutionLogData& log) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }

    file << log.algorithmName << "\n";
    file << log.totalSteps << " " << log.iterations << " " << log.executionTimeMs << " " << (log.found ? 1 : 0) << "\n";

    for (const auto& snap : log.snapshots) {
        file << snap.playerRow << " " << snap.playerCol << " "
             << snap.visitedCheckpoints << " " << snap.move << " "
             << snap.stepNumber << "\n";
    }
}

SolutionLogData fileIO::readSolutionLog(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    SolutionLogData log;
    std::getline(file, log.algorithmName);

    int foundFlag;
    file >> log.totalSteps >> log.iterations >> log.executionTimeMs >> foundFlag;
    log.found = (foundFlag != 0);

    for (int i = 0; i < log.totalSteps; i++) {
        SnapshotData snap;
        file >> snap.playerRow >> snap.playerCol
             >> snap.visitedCheckpoints >> snap.move
             >> snap.stepNumber;
        log.snapshots.push_back(snap);
    }

    return log;
}


} // namespace fileUtil
