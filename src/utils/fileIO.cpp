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


} // namespace fileUtil
