#pragma once
#include <stdexcept>
#include <string>
#include <vector>

namespace fileUtil {

struct PuzzleData {
    int rows;
    int cols;
    std::vector<std::vector<char>> board;
    std::vector<std::vector<int>> cost;
};

struct SnapshotData {
    int playerRow;
    int playerCol;
    int visitedCheckpoints;
    char move;
    int stepNumber;
};

struct SolutionLogData {
    std::string algorithmName;
    int totalSteps;
    int iterations;
    long long executionTimeMs;
    bool found;
    std::vector<SnapshotData> snapshots;
    int rows;
    int cols;
    std::vector<std::vector<char>> board;
    std::vector<std::vector<int>> cost;
};

class fileIO {
public:
    static PuzzleData readFile(const std::string& filepath);
    static void writeSolutionLog(const std::string& filepath, const SolutionLogData& log);
    static SolutionLogData readSolutionLog(const std::string& filepath);
};

} // namespace fileUtil
