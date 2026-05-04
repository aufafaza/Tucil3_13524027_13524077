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

class fileIO {
public:
    static PuzzleData readFile(const std::string& filepath);
    // static int writeFile(const std::string& filepath, const std::string& buffer); // later if needed
};

} // namespace fileUtil
