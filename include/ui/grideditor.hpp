#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "utils/fileIO.hpp"

namespace ui {

enum class AlgorithmChoice {
    NONE = 0,
    UCS = 1,
    ASTAR = 2,
    GBFS = 3
};

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

    bool dialogOpen_ = false;
    bool dialogIsSave_ = false;
    std::string dialogPath_ = "puzzle_output.txt";
    std::string dialogStatus_;

    bool algoDialogOpen_ = false;
    AlgorithmChoice selectedAlgorithm_ = AlgorithmChoice::NONE;

    float cellSize_;
    float gridOffX_;
    float gridOffY_;
    float paletteOffX_;

    static constexpr int WIN_W = 1200;
    static constexpr int WIN_H = 800;
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
    void drawToolbarButtons();
    void drawWeightToggle();
    void drawSizeControls();
    void drawDialog();
    void drawAlgorithmDialog();

    void paintCell(int row, int col);
    void cycleWeight(int row, int col);
    void saveToFile(const std::string& path);
    void loadFromFile(const std::string& path);
    void ensureValidGrid();

    bool isInsideGrid(int row, int col) const;
    void drawButton(float x, float y, float w, float h, const std::string& label,
                    const sf::Color& bg, const sf::Color& outline, bool hovered);

    bool playRequested_ = false;

public:
    GridEditor();

    fileUtil::PuzzleData getResult() const;
    AlgorithmChoice getSelectedAlgorithm() const;
    bool run();
};

} // namespace ui