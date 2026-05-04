#include "utils/fileIO.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc > 2) { 
        return 0; 
    } 
    const std::string file_loc = argv[1];
    fileUtil::PuzzleData pz = fileUtil::fileIO::readFile(file_loc);
    std::cout << "rows=" << pz.rows << " cols=" << pz.cols << "\n";
    std::cout << "Board:\n";
    for (int i = 0; i < pz.rows; i++) {
        for (int j = 0; j < pz.cols; j++)
            std::cout << pz.board[i][j];
        std::cout << "\n";
    }
    std::cout << "Cost:\n";
    for (int i = 0; i < pz.rows; i++) {
        for (int j = 0; j < pz.cols; j++)
            std::cout << pz.cost[i][j] << "\t";
        std::cout << "\n";
    }
} 
