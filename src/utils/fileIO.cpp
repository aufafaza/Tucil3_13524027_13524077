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
// line 3: rows cols
// line 4..rows+3: board rows (chars, no spaces)
// line rows+4..2*rows+3: cost rows (tab-separated)
// then per step: blank line, "Step N [move] visited=M", grid state
void fileIO::writeSolutionLog(const std::string& filepath, const SolutionLogData& log) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }

    file << log.algorithmName << "\n";
    file << log.totalSteps << " " << log.iterations << " "
         << log.executionTimeMs << " " << (log.found ? 1 : 0) << "\n";
    file << log.rows << " " << log.cols << "\n";

    for (int r = 0; r < log.rows; r++) {
        for (int c = 0; c < log.cols; c++)
            file << log.board[r][c];
        file << "\n";
    }
    for (int r = 0; r < log.rows; r++) {
        for (int c = 0; c < log.cols; c++) {
            if (c > 0) file << "\t";
            file << log.cost[r][c];
        }
        file << "\n";
    }

    for (const auto& snap : log.snapshots) {
        file << "\n";
        file << "Step " << snap.stepNumber << " [" << snap.move
             << "] visited=" << snap.visitedCheckpoints << "\n";
        for (int r = 0; r < log.rows; r++) {
            for (int c = 0; c < log.cols; c++) {
                if (r == snap.playerRow && c == snap.playerCol)
                    file << 'Z';
                else
                    file << log.board[r][c];
            }
            file << "\n";
        }
    }
    file << "\n--- END ---\n";
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

    file >> log.rows >> log.cols;
    log.board.resize(log.rows, std::vector<char>(log.cols));
    log.cost.resize(log.rows, std::vector<int>(log.cols));

    for (int r = 0; r < log.rows; r++)
        for (int c = 0; c < log.cols; c++)
            file >> log.board[r][c];

    for (int r = 0; r < log.rows; r++)
        for (int c = 0; c < log.cols; c++)
            file >> log.cost[r][c];

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
