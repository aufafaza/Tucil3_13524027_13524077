#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "utils/fileIO.hpp"

namespace ui {

struct EditorPaletteEntry {
    char tile;
    std::string label;
    sf::Color color;
};

class GridEditor {
private:
    int rows_ = 7;
    int cols_ = 7;
    std::vector<std::vector<char>> grid_;
    std::vector<std::vector<int>> weights_;

    int selectedPalette_ = 0;
    int checkpointNumber_ = 0;
    bool showWeights_ = true;
    bool editMode_ = true;

    bool dialogOpen_ = false;
    bool dialogIsSave_ = false;
    std::string dialogPath_ = "puzzle_output.txt";
    std::string dialogStatus_;

    float cellSize_;
    float gridOffX_;
    float gridOffY_;
    float paletteOffX_;

    static constexpr float PALETTE_CELL = 48.f;
    static constexpr float PALETTE_GAP = 6.f;

    sf::RenderWindow window_;
    sf::Font font_;

    std::vector<EditorPaletteEntry> palette_;

    void initGrid();
    void rebuildWindow();
    void loadFont();
    std::pair<int, int> getCellFromMouse(int x, int y) const;
    int getPaletteFromMouse(int x, int y) const;

    void drawGrid();
    void drawCell(int row, int col);
    void drawPalette();
    void drawToolbar();
    void drawWeightToggle();
    void drawSizeControls();
    void drawDialog();

    void paintCell(int row, int col);
    void cycleWeight(int row, int col);
    void saveToFile(const std::string& path);
    void loadFromFile(const std::string& path);
    void ensureValidGrid();

    bool isInsideGrid(int row, int col) const;

bool playRequested_ = false;

public:
    GridEditor();

    fileUtil::PuzzleData getResult() const;
    bool run();
};

} // namespace ui