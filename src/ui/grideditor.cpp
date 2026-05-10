#include "ui/grideditor.hpp"
#include <fstream>
#include <sstream>

namespace ui {

GridEditor::GridEditor() {
    palette_ = {
        {'X', "Wall",       sf::Color(60, 60, 70)},
        {'*', "Floor",      sf::Color(180, 200, 220)},
        {'Z', "Start",      sf::Color(50, 150, 255)},
        {'O', "Goal",       sf::Color(50, 200, 80)},
        {'L', "Lava",       sf::Color(200, 50, 50)},
        {'C', "Checkpoint", sf::Color(255, 200, 50)},
    };

    initGrid();
    loadFont();
    cellSize_ = 64.f;
    rebuildWindow();
}

void GridEditor::initGrid() {
    grid_.assign(rows_, std::vector<char>(cols_, '*'));
    weights_.assign(rows_, std::vector<int>(cols_, 1));
    for (int r = 0; r < rows_; r++) {
        grid_[r][0] = 'X';
        grid_[r][cols_ - 1] = 'X';
        for (int c = 0; c < cols_; c++) {
            weights_[r][c] = 1;
        }
    }
    for (int c = 0; c < cols_; c++) {
        grid_[0][c] = 'X';
        grid_[rows_ - 1][c] = 'X';
    }
    grid_[1][1] = 'Z';
    grid_[rows_ - 2][cols_ - 2] = 'O';
}

void GridEditor::rebuildWindow() {
    float paletteWidth = PALETTE_CELL + PALETTE_GAP + 200.f;
    float gridW = cols_ * cellSize_;
    float gridH = rows_ * cellSize_;
    int winW = static_cast<int>(gridW + paletteWidth + 120);
    int winH = static_cast<int>(gridH + 220);

    gridOffX_ = 40.f;
    gridOffY_ = 120.f;
    paletteOffX_ = gridOffX_ + gridW + 40.f;

    window_.create(sf::VideoMode(winW, winH), "Grid Editor - Sliding Ice Puzzle");
    window_.setFramerateLimit(60);
}

void GridEditor::loadFont() {
    const char* paths[] = {
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
        "C:/Windows/Fonts/arial.ttf",
    };
    for (const char* p : paths) {
        if (font_.loadFromFile(p)) break;
    }
}

std::pair<int, int> GridEditor::getCellFromMouse(int x, int y) const {
    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);
    int col = static_cast<int>((fx - gridOffX_) / cellSize_);
    int row = static_cast<int>((fy - gridOffY_) / cellSize_);
    return {row, col};
}

int GridEditor::getPaletteFromMouse(int x, int y) const {
    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);
    for (int i = 0; i < static_cast<int>(palette_.size()); i++) {
        float py = gridOffY_ + i * (PALETTE_CELL + PALETTE_GAP);
        sf::FloatRect rect(paletteOffX_, py, PALETTE_CELL + 190.f, PALETTE_CELL);
        if (rect.contains(fx, fy)) return i;
    }
    return -1;
}

bool GridEditor::isInsideGrid(int row, int col) const {
    return row >= 0 && row < rows_ && col >= 0 && col < cols_;
}

void GridEditor::paintCell(int row, int col) {
    if (!isInsideGrid(row, col)) return;
    char tile = palette_[selectedPalette_].tile;

    if (tile == 'C') {
        tile = '0' + checkpointNumber_;
    }

    if (tile == 'Z') {
        for (int r = 0; r < rows_; r++)
            for (int c = 0; c < cols_; c++)
                if (grid_[r][c] == 'Z') grid_[r][c] = '*';
    }
    if (tile == 'O') {
        for (int r = 0; r < rows_; r++)
            for (int c = 0; c < cols_; c++)
                if (grid_[r][c] == 'O') grid_[r][c] = '*';
    }

    grid_[row][col] = tile;
    if (tile == 'X' || tile == 'L') {
        weights_[row][col] = 999;
    } else if (weights_[row][col] == 999) {
        weights_[row][col] = 1;
    }
}

void GridEditor::cycleWeight(int row, int col) {
    if (!isInsideGrid(row, col)) return;
    char tile = grid_[row][col];
    if (tile == 'X' || tile == 'L') return;
    weights_[row][col]++;
    if (weights_[row][col] > 20) weights_[row][col] = 1;
}

void GridEditor::ensureValidGrid() {
    bool hasStart = false, hasGoal = false;
    for (int r = 0; r < rows_; r++) {
        for (int c = 0; c < cols_; c++) {
            if (grid_[r][c] == 'Z') hasStart = true;
            if (grid_[r][c] == 'O') hasGoal = true;
        }
    }
    if (!hasStart) {
        for (int r = 1; r < rows_ - 1 && !hasStart; r++)
            for (int c = 1; c < cols_ - 1 && !hasStart; c++)
                if (grid_[r][c] == '*') { grid_[r][c] = 'Z'; hasStart = true; }
    }
    if (!hasGoal) {
        for (int r = rows_ - 2; r >= 1 && !hasGoal; r--)
            for (int c = cols_ - 2; c >= 1 && !hasGoal; c--)
                if (grid_[r][c] == '*') { grid_[r][c] = 'O'; hasGoal = true; }
    }
}

void GridEditor::saveToFile(const std::string& path) {
    ensureValidGrid();
    std::ofstream out(path);
    if (!out.is_open()) return;
    out << rows_ << " " << cols_ << "\n";
    for (int r = 0; r < rows_; r++) {
        for (int c = 0; c < cols_; c++)
            out << grid_[r][c];
        out << "\n";
    }
    for (int r = 0; r < rows_; r++) {
        for (int c = 0; c < cols_; c++) {
            if (c > 0) out << "\t";
            out << weights_[r][c];
        }
        out << "\n";
    }
    out.close();
}

void GridEditor::loadFromFile(const std::string& path) {
    try {
        fileUtil::PuzzleData data = fileUtil::fileIO::readFile(path);
        rows_ = data.rows;
        cols_ = data.cols;
        grid_ = data.board;
        weights_ = data.cost;
        rebuildWindow();
    } catch (const std::exception&) {
        dialogStatus_ = "Error: Cannot read " + path;
    }
}

void GridEditor::drawCell(int row, int col) {
    char tile = grid_[row][col];
    float x = gridOffX_ + col * cellSize_;
    float y = gridOffY_ + row * cellSize_;

    sf::RectangleShape rect(sf::Vector2f(cellSize_ - 2, cellSize_ - 2));
    rect.setPosition(x + 1, y + 1);

    if (tile == 'X') {
        rect.setFillColor(sf::Color(60, 60, 70));
        window_.draw(rect);
        sf::Text label("X", font_, 18);
        label.setPosition(x + cellSize_ / 2 - 6, y + cellSize_ / 2 - 10);
        label.setFillColor(sf::Color(120, 120, 130));
        window_.draw(label);
    } else if (tile == 'L') {
        rect.setFillColor(sf::Color(200, 50, 50));
        window_.draw(rect);
        sf::Text label("L", font_, 18);
        label.setPosition(x + cellSize_ / 2 - 5, y + cellSize_ / 2 - 10);
        label.setFillColor(sf::Color::White);
        window_.draw(label);
    } else if (tile == 'Z') {
        rect.setFillColor(sf::Color(50, 150, 255));
        window_.draw(rect);
        sf::Text label("Z", font_, 18);
        label.setPosition(x + cellSize_ / 2 - 5, y + cellSize_ / 2 - 10);
        label.setFillColor(sf::Color::White);
        window_.draw(label);
    } else if (tile == 'O') {
        rect.setFillColor(sf::Color(50, 200, 80));
        window_.draw(rect);
        sf::Text label("O", font_, 18);
        label.setPosition(x + cellSize_ / 2 - 5, y + cellSize_ / 2 - 10);
        label.setFillColor(sf::Color::White);
        window_.draw(label);
    } else if (tile >= '0' && tile <= '9') {
        rect.setFillColor(sf::Color(255, 200, 50));
        window_.draw(rect);
        std::string s(1, tile);
        sf::Text label(s, font_, 22);
        label.setPosition(x + cellSize_ / 2 - 6, y + cellSize_ / 2 - 12);
        label.setFillColor(sf::Color::Black);
        window_.draw(label);
    } else {
        rect.setFillColor(sf::Color(180, 200, 220));
        window_.draw(rect);
        if (showWeights_) {
            sf::Text wt(std::to_string(weights_[row][col]), font_, 14);
            wt.setPosition(x + cellSize_ / 2 - 6, y + cellSize_ / 2 - 8);
            wt.setFillColor(sf::Color(80, 80, 110));
            window_.draw(wt);
        }
    }
}

void GridEditor::drawGrid() {
    for (int r = 0; r < rows_; r++)
        for (int c = 0; c < cols_; c++)
            drawCell(r, c);

    sf::RectangleShape outline(sf::Vector2f(cols_ * cellSize_, rows_ * cellSize_));
    outline.setPosition(gridOffX_, gridOffY_);
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineColor(sf::Color(150, 150, 180));
    outline.setOutlineThickness(2.f);
    window_.draw(outline);
}

void GridEditor::drawPalette() {
    sf::Text title("Palette", font_, 18);
    title.setPosition(paletteOffX_, gridOffY_ - 30);
    title.setFillColor(sf::Color(220, 220, 220));
    window_.draw(title);

    float y = gridOffY_;

    for (int i = 0; i < static_cast<int>(palette_.size()); i++) {
        float py = y + i * (PALETTE_CELL + PALETTE_GAP);

        sf::RectangleShape cell(sf::Vector2f(PALETTE_CELL, PALETTE_CELL));
        cell.setPosition(paletteOffX_, py);
        cell.setFillColor(palette_[i].color);
        if (i == selectedPalette_) {
            cell.setOutlineColor(sf::Color(255, 255, 100));
            cell.setOutlineThickness(3.f);
        } else {
            cell.setOutlineColor(sf::Color(100, 100, 120));
            cell.setOutlineThickness(1.f);
        }
        window_.draw(cell);

        if (palette_[i].tile == 'C') {
            std::string cpStr = std::to_string(checkpointNumber_);
            sf::Text tileLabel(cpStr, font_, 24);
            tileLabel.setPosition(paletteOffX_ + PALETTE_CELL / 2 - 6, py + PALETTE_CELL / 2 - 13);
            tileLabel.setFillColor(sf::Color::Black);
            window_.draw(tileLabel);
        } else {
            std::string label(1, palette_[i].tile);
            sf::Text tileLabel(label, font_, 22);
            tileLabel.setPosition(paletteOffX_ + PALETTE_CELL / 2 - 6, py + PALETTE_CELL / 2 - 12);
            tileLabel.setFillColor(palette_[i].tile == 'X' ? sf::Color(150, 150, 160) : sf::Color::Black);
            window_.draw(tileLabel);
        }

        sf::Text nameLabel(palette_[i].label, font_, 16);
        nameLabel.setPosition(paletteOffX_ + PALETTE_CELL + 8, py + PALETTE_CELL / 2 - 8);
        nameLabel.setFillColor(sf::Color(200, 200, 210));
        window_.draw(nameLabel);

        if (palette_[i].tile == 'C') {
            float ddX = paletteOffX_ + PALETTE_CELL + 100;
            float ddW = 90.f;
            float ddH = PALETTE_CELL;

            sf::RectangleShape ddBg(sf::Vector2f(ddW, ddH));
            ddBg.setPosition(ddX, py);
            ddBg.setFillColor(sf::Color(50, 50, 65));
            ddBg.setOutlineColor(sf::Color(100, 100, 130));
            ddBg.setOutlineThickness(1.f);
            window_.draw(ddBg);

            sf::Text ddLabel("CP: " + std::to_string(checkpointNumber_), font_, 16);
            ddLabel.setPosition(ddX + 8, py + PALETTE_CELL / 2 - 8);
            ddLabel.setFillColor(sf::Color(220, 220, 240));
            window_.draw(ddLabel);

            sf::Text upArrow("^", font_, 14);
            upArrow.setPosition(ddX + ddW - 26, py + 2);
            upArrow.setFillColor(sf::Color(180, 180, 200));
            window_.draw(upArrow);

            sf::Text downArrow("v", font_, 14);
            downArrow.setPosition(ddX + ddW - 26, py + PALETTE_CELL / 2 + 2);
            downArrow.setFillColor(sf::Color(180, 180, 200));
            window_.draw(downArrow);
        }
    }
}

void GridEditor::drawToolbar() {
    sf::Text title("Grid Editor", font_, 22);
    title.setPosition(gridOffX_, 10);
    title.setFillColor(sf::Color(220, 220, 240));
    window_.draw(title);

    sf::Text hint("Left-click: paint | Right-click: cycle weight", font_, 13);
    hint.setPosition(gridOffX_, 42);
    hint.setFillColor(sf::Color(160, 160, 170));
    window_.draw(hint);

    sf::Text hint2("S:Save  L:Load  R:Reset  W:Weights", font_, 13);
    hint2.setPosition(gridOffX_, 60);
    hint2.setFillColor(sf::Color(160, 160, 170));
    window_.draw(hint2);

    sf::Text hint3("PLAY button or Enter to select algorithm", font_, 13);
    hint3.setPosition(gridOffX_, 78);
    hint3.setFillColor(sf::Color(100, 200, 100));
    window_.draw(hint3);
}

void GridEditor::drawWeightToggle() {
    float y = gridOffY_ + rows_ * cellSize_ + 20;
    sf::RectangleShape btn(sf::Vector2f(140.f, 30.f));
    btn.setPosition(gridOffX_, y);
    btn.setFillColor(showWeights_ ? sf::Color(70, 130, 70) : sf::Color(130, 70, 70));
    btn.setOutlineColor(sf::Color(160, 160, 180));
    btn.setOutlineThickness(1.f);
    window_.draw(btn);

    std::string label = showWeights_ ? "Weights: ON" : "Weights: OFF";
    sf::Text btnText(label, font_, 16);
    btnText.setPosition(gridOffX_ + 12, y + 5);
    btnText.setFillColor(sf::Color::White);
    window_.draw(btnText);
}

void GridEditor::drawSizeControls() {
    float y = gridOffY_ + rows_ * cellSize_ + 20;

    sf::Text rowsLabel("Rows:", font_, 16);
    rowsLabel.setPosition(gridOffX_ + 160, y);
    rowsLabel.setFillColor(sf::Color(200, 200, 210));
    window_.draw(rowsLabel);

    sf::RectangleShape rowsBox(sf::Vector2f(40, 26));
    rowsBox.setPosition(gridOffX_ + 215, y + 2);
    rowsBox.setFillColor(sf::Color(50, 50, 65));
    rowsBox.setOutlineColor(sf::Color(120, 120, 150));
    rowsBox.setOutlineThickness(1.f);
    window_.draw(rowsBox);

    sf::Text rowsVal(std::to_string(rows_), font_, 16);
    rowsVal.setPosition(gridOffX_ + 225, y + 4);
    rowsVal.setFillColor(sf::Color::White);
    window_.draw(rowsVal);

    sf::Text colsLabel("Cols:", font_, 16);
    colsLabel.setPosition(gridOffX_ + 270, y);
    colsLabel.setFillColor(sf::Color(200, 200, 210));
    window_.draw(colsLabel);

    sf::RectangleShape colsBox(sf::Vector2f(40, 26));
    colsBox.setPosition(gridOffX_ + 315, y + 2);
    colsBox.setFillColor(sf::Color(50, 50, 65));
    colsBox.setOutlineColor(sf::Color(120, 120, 150));
    colsBox.setOutlineThickness(1.f);
    window_.draw(colsBox);

    sf::Text colsVal(std::to_string(cols_), font_, 16);
    colsVal.setPosition(gridOffX_ + 325, y + 4);
    colsVal.setFillColor(sf::Color::White);
    window_.draw(colsVal);

    float btnY = y + 35;

    sf::RectangleShape rowUpBtn(sf::Vector2f(34, 26));
    rowUpBtn.setPosition(gridOffX_ + 215, btnY);
    rowUpBtn.setFillColor(sf::Color(70, 80, 110));
    rowUpBtn.setOutlineColor(sf::Color(140, 140, 180));
    rowUpBtn.setOutlineThickness(1.f);
    window_.draw(rowUpBtn);
    sf::Text rowUpLabel("+", font_, 18);
    rowUpLabel.setPosition(gridOffX_ + 226, btnY + 2);
    rowUpLabel.setFillColor(sf::Color::White);
    window_.draw(rowUpLabel);

    sf::RectangleShape rowDownBtn(sf::Vector2f(34, 26));
    rowDownBtn.setPosition(gridOffX_ + 255, btnY);
    rowDownBtn.setFillColor(sf::Color(70, 80, 110));
    rowDownBtn.setOutlineColor(sf::Color(140, 140, 180));
    rowDownBtn.setOutlineThickness(1.f);
    window_.draw(rowDownBtn);
    sf::Text rowDownLabel("-", font_, 18);
    rowDownLabel.setPosition(gridOffX_ + 269, btnY + 2);
    rowDownLabel.setFillColor(sf::Color::White);
    window_.draw(rowDownLabel);

    sf::RectangleShape colUpBtn(sf::Vector2f(34, 26));
    colUpBtn.setPosition(gridOffX_ + 315, btnY);
    colUpBtn.setFillColor(sf::Color(70, 80, 110));
    colUpBtn.setOutlineColor(sf::Color(140, 140, 180));
    colUpBtn.setOutlineThickness(1.f);
    window_.draw(colUpBtn);
    sf::Text colUpLabel("+", font_, 18);
    colUpLabel.setPosition(gridOffX_ + 326, btnY + 2);
    colUpLabel.setFillColor(sf::Color::White);
    window_.draw(colUpLabel);

    sf::RectangleShape colDownBtn(sf::Vector2f(34, 26));
    colDownBtn.setPosition(gridOffX_ + 355, btnY);
    colDownBtn.setFillColor(sf::Color(70, 80, 110));
    colDownBtn.setOutlineColor(sf::Color(140, 140, 180));
    colDownBtn.setOutlineThickness(1.f);
    window_.draw(colDownBtn);
    sf::Text colDownLabel("-", font_, 18);
    colDownLabel.setPosition(gridOffX_ + 369, btnY + 2);
    colDownLabel.setFillColor(sf::Color::White);
    window_.draw(colDownLabel);
}

void GridEditor::drawDialog() {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window_.getSize().x), static_cast<float>(window_.getSize().y)));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window_.draw(overlay);

    float boxW = 500.f;
    float boxH = 180.f;
    float boxX = (static_cast<float>(window_.getSize().x) - boxW) / 2.f;
    float boxY = (static_cast<float>(window_.getSize().y) - boxH) / 2.f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(40, 40, 55));
    box.setOutlineColor(sf::Color(120, 140, 200));
    box.setOutlineThickness(2.f);
    window_.draw(box);

    std::string dlgTitle = dialogIsSave_ ? "Save Puzzle" : "Load Puzzle";
    sf::Text title(dlgTitle, font_, 20);
    title.setPosition(boxX + 20, boxY + 15);
    title.setFillColor(sf::Color(220, 220, 240));
    window_.draw(title);

    sf::Text pathLabel("File path:", font_, 16);
    pathLabel.setPosition(boxX + 20, boxY + 50);
    pathLabel.setFillColor(sf::Color(180, 180, 200));
    window_.draw(pathLabel);

    sf::RectangleShape inputBox(sf::Vector2f(boxW - 40, 30));
    inputBox.setPosition(boxX + 20, boxY + 75);
    inputBox.setFillColor(sf::Color(30, 30, 45));
    inputBox.setOutlineColor(sf::Color(100, 120, 180));
    inputBox.setOutlineThickness(1.f);
    window_.draw(inputBox);

    std::string displayPath = dialogPath_ + "_";
    sf::Text pathText(displayPath, font_, 16);
    pathText.setPosition(boxX + 26, boxY + 80);
    pathText.setFillColor(sf::Color::White);
    window_.draw(pathText);

    std::string action = dialogIsSave_ ? "Save" : "Load";
    sf::Text hint("Enter: " + action + "  |  Esc: Cancel", font_, 13);
    hint.setPosition(boxX + 20, boxY + 120);
    hint.setFillColor(sf::Color(140, 140, 160));
    window_.draw(hint);

    if (!dialogStatus_.empty()) {
        bool isErr = dialogStatus_.find("Error") != std::string::npos;
        sf::Text status(dialogStatus_, font_, 14);
        status.setPosition(boxX + 20, boxY + 145);
        status.setFillColor(isErr ? sf::Color(255, 80, 80) : sf::Color(80, 220, 80));
        window_.draw(status);
    }
}

void GridEditor::drawButton(float x, float y, float w, float h, const std::string& label,
                            const sf::Color& bg, const sf::Color& outline, bool hovered) {
    sf::RectangleShape rect(sf::Vector2f(w, h));
    rect.setPosition(x, y);
    sf::Color fillBg = hovered
        ? sf::Color(std::min(static_cast<int>(bg.r) + 30, 255),
                    std::min(static_cast<int>(bg.g) + 30, 255),
                    std::min(static_cast<int>(bg.b) + 30, 255))
        : bg;
    rect.setFillColor(fillBg);
    rect.setOutlineColor(outline);
    rect.setOutlineThickness(1.f);
    window_.draw(rect);

    if (!label.empty()) {
        sf::Text btnLabel(label, font_, 16);
        float textW = btnLabel.getLocalBounds().width;
        float textH = btnLabel.getLocalBounds().height;
        btnLabel.setPosition(x + (w - textW) / 2, y + (h - textH) / 2 - 2);
        btnLabel.setFillColor(sf::Color::White);
        window_.draw(btnLabel);
    }
}

void GridEditor::drawToolbarButtons() {
    float btnW = 80.f;
    float btnH = 30.f;
    float btnGap = 10.f;
    float startX = static_cast<float>(window_.getSize().x) - 3 * btnW - 2 * btnGap - 20.f;
    float btnY = 15.f;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window_);
    float mx = static_cast<float>(mousePos.x);
    float my = static_cast<float>(mousePos.y);

    sf::FloatRect loadRect(startX, btnY, btnW, btnH);
    drawButton(startX, btnY, btnW, btnH, "LOAD",
               sf::Color(70, 80, 110), sf::Color(140, 140, 180),
               loadRect.contains(mx, my));

    float saveX = startX + btnW + btnGap;
    sf::FloatRect saveRect(saveX, btnY, btnW, btnH);
    drawButton(saveX, btnY, btnW, btnH, "SAVE",
               sf::Color(70, 80, 110), sf::Color(140, 140, 180),
               saveRect.contains(mx, my));

    float playX = saveX + btnW + btnGap;
    sf::FloatRect playRect(playX, btnY, btnW, btnH);
    drawButton(playX, btnY, btnW, btnH, "PLAY",
               sf::Color(50, 120, 50), sf::Color(100, 200, 100),
               playRect.contains(mx, my));
}

void GridEditor::drawAlgorithmDialog() {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window_.getSize().x),
                                             static_cast<float>(window_.getSize().y)));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window_.draw(overlay);

    float boxW = 400.f;
    float boxH = 320.f;
    float boxX = (static_cast<float>(window_.getSize().x) - boxW) / 2.f;
    float boxY = (static_cast<float>(window_.getSize().y) - boxH) / 2.f;

    sf::RectangleShape box(sf::Vector2f(boxW, boxH));
    box.setPosition(boxX, boxY);
    box.setFillColor(sf::Color(40, 40, 55));
    box.setOutlineColor(sf::Color(120, 140, 200));
    box.setOutlineThickness(2.f);
    window_.draw(box);

    sf::Text title("Select Algorithm", font_, 22);
    title.setPosition(boxX + 20, boxY + 15);
    title.setFillColor(sf::Color(220, 220, 240));
    window_.draw(title);

    sf::Text hint("Esc: Cancel", font_, 13);
    hint.setPosition(boxX + 20, boxY + 45);
    hint.setFillColor(sf::Color(140, 140, 160));
    window_.draw(hint);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window_);
    float mx = static_cast<float>(mousePos.x);
    float my = static_cast<float>(mousePos.y);

    float optY = boxY + 70;
    float optW = boxW - 40;
    float optH = 50.f;
    float optGap = 8.f;

    struct AlgoOption {
        AlgorithmChoice choice;
        const char* name;
        const char* desc;
        sf::Color color;
    };

    AlgoOption options[] = {
        {AlgorithmChoice::UCS, "UCS", "Uniform Cost Search", sf::Color(70, 100, 160)},
        {AlgorithmChoice::ASTAR, "A*", "A* Search", sf::Color(70, 130, 70)},
        {AlgorithmChoice::GBFS, "GBFS", "Greedy Best-First Search", sf::Color(140, 80, 140)},
    };

    for (int i = 0; i < 3; i++) {
        float y = optY + i * (optH + optGap);
        bool selected = (selectedAlgorithm_ == options[i].choice);
        bool hovered = sf::FloatRect(boxX + 20, y, optW, optH).contains(mx, my);

        sf::Color bg = options[i].color;
        if (selected) bg = sf::Color(std::min(static_cast<int>(bg.r) + 50, 255),
                                     std::min(static_cast<int>(bg.g) + 50, 255),
                                     std::min(static_cast<int>(bg.b) + 50, 255));
        else if (hovered) bg = sf::Color(std::min(static_cast<int>(bg.r) + 25, 255),
                                          std::min(static_cast<int>(bg.g) + 25, 255),
                                          std::min(static_cast<int>(bg.b) + 25, 255));

        drawButton(boxX + 20, y, optW, optH, "", bg,
                   selected ? sf::Color(255, 255, 100) : sf::Color(100, 100, 130), hovered);

        sf::Text nameText(options[i].name, font_, 20);
        nameText.setPosition(boxX + 35, y + 8);
        nameText.setFillColor(sf::Color::White);
        window_.draw(nameText);

        sf::Text descText(options[i].desc, font_, 13);
        descText.setPosition(boxX + 35, y + 30);
        descText.setFillColor(sf::Color(180, 180, 200));
        window_.draw(descText);
    }

    float playBtnY = boxY + boxH - 55;
    float playBtnW = 160.f;
    float playBtnH = 40.f;
    float playBtnX = boxX + (boxW - playBtnW) / 2;
    bool canPlay = selectedAlgorithm_ != AlgorithmChoice::NONE;
    sf::Color playBg = canPlay ? sf::Color(50, 140, 50) : sf::Color(80, 80, 80);
    bool playHover = canPlay && sf::FloatRect(playBtnX, playBtnY, playBtnW, playBtnH).contains(mx, my);
    drawButton(playBtnX, playBtnY, playBtnW, playBtnH, "PLAY", playBg,
               canPlay ? sf::Color(100, 200, 100) : sf::Color(100, 100, 100), playHover);
}

AlgorithmChoice GridEditor::getSelectedAlgorithm() const {
    return selectedAlgorithm_;
}

fileUtil::PuzzleData GridEditor::getResult() const {
    fileUtil::PuzzleData result;
    result.rows = rows_;
    result.cols = cols_;
    result.board = grid_;
    result.cost = weights_;
    return result;
}

bool GridEditor::run() {
    while (window_.isOpen()) {
        sf::Event event;
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window_.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (dialogOpen_) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        dialogOpen_ = false;
                        dialogStatus_.clear();
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        if (dialogIsSave_) {
                            dialogStatus_.clear();
                            ensureValidGrid();
                            std::ofstream test(dialogPath_);
                            if (test.is_open()) {
                                test.close();
                                saveToFile(dialogPath_);
                                dialogStatus_ = "Saved to: " + dialogPath_;
                            } else {
                                dialogStatus_ = "Error: Cannot write to " + dialogPath_;
                            }
                        } else {
                            dialogStatus_.clear();
                            std::ifstream test(dialogPath_);
                            if (test.is_open()) {
                                test.close();
                                loadFromFile(dialogPath_);
                                if (dialogStatus_.empty()) {
                                    dialogStatus_ = "Loaded: " + dialogPath_;
                                }
                            } else {
                                dialogStatus_ = "Error: Cannot read " + dialogPath_;
                            }
                        }
                    } else if (event.key.code == sf::Keyboard::Backspace) {
                        if (!dialogPath_.empty()) dialogPath_.pop_back();
                    }
                } else if (algoDialogOpen_) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        algoDialogOpen_ = false;
                    }
                } else {
                    switch (event.key.code) {
                        case sf::Keyboard::W:
                            showWeights_ = !showWeights_;
                            break;
                        case sf::Keyboard::R:
                            initGrid();
                            break;
                        case sf::Keyboard::S:
                            dialogOpen_ = true;
                            dialogIsSave_ = true;
                            dialogPath_ = "puzzle_output.txt";
                            dialogStatus_.clear();
                            break;
                        case sf::Keyboard::L:
                            dialogOpen_ = true;
                            dialogIsSave_ = false;
                            dialogPath_ = "";
                            dialogStatus_.clear();
                            break;
                        case sf::Keyboard::Return:
                            algoDialogOpen_ = true;
                            break;
                        case sf::Keyboard::Escape:
                            window_.close();
                            break;
                        default:
                            break;
                    }
                }
            } else if (event.type == sf::Event::TextEntered && dialogOpen_) {
                if (event.text.unicode >= 32 && event.text.unicode < 127) {
                    dialogPath_ += static_cast<char>(event.text.unicode);
                }
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (algoDialogOpen_ && !dialogOpen_) {
                    float mx = static_cast<float>(event.mouseButton.x);
                    float my = static_cast<float>(event.mouseButton.y);

                    float boxW = 400.f;
                    float boxH = 320.f;
                    float boxX = (static_cast<float>(window_.getSize().x) - boxW) / 2.f;
                    float boxY = (static_cast<float>(window_.getSize().y) - boxH) / 2.f;

                    float optY = boxY + 70;
                    float optW = boxW - 40;
                    float optH = 50.f;
                    float optGap = 8.f;

                    AlgorithmChoice choices[] = {AlgorithmChoice::UCS, AlgorithmChoice::ASTAR, AlgorithmChoice::GBFS};
                    for (int i = 0; i < 3; i++) {
                        float y = optY + i * (optH + optGap);
                        sf::FloatRect optRect(boxX + 20, y, optW, optH);
                        if (optRect.contains(mx, my)) {
                            selectedAlgorithm_ = choices[i];
                        }
                    }

                    float playBtnY = boxY + boxH - 55;
                    float playBtnW = 160.f;
                    float playBtnH = 40.f;
                    float playBtnX = boxX + (boxW - playBtnW) / 2;
                    if (selectedAlgorithm_ != AlgorithmChoice::NONE &&
                        sf::FloatRect(playBtnX, playBtnY, playBtnW, playBtnH).contains(mx, my)) {
                        playRequested_ = true;
                        ensureValidGrid();
                        window_.close();
                    }
                } else if (!dialogOpen_ && !algoDialogOpen_) {
                    float mx = static_cast<float>(event.mouseButton.x);
                    float my = static_cast<float>(event.mouseButton.y);

                    float tbBtnW = 80.f;
                    float tbBtnH = 30.f;
                    float tbBtnGap = 10.f;
                    float tbStartX = static_cast<float>(window_.getSize().x) - 3 * tbBtnW - 2 * tbBtnGap - 20.f;
                    float tbBtnY = 15.f;

                    sf::FloatRect loadRect(tbStartX, tbBtnY, tbBtnW, tbBtnH);
                    sf::FloatRect saveRect(tbStartX + tbBtnW + tbBtnGap, tbBtnY, tbBtnW, tbBtnH);
                    sf::FloatRect playRect(tbStartX + 2 * (tbBtnW + tbBtnGap), tbBtnY, tbBtnW, tbBtnH);

                    if (loadRect.contains(mx, my)) {
                        dialogOpen_ = true;
                        dialogIsSave_ = false;
                        dialogPath_ = "";
                        dialogStatus_.clear();
                    } else if (saveRect.contains(mx, my)) {
                        dialogOpen_ = true;
                        dialogIsSave_ = true;
                        dialogPath_ = "puzzle_output.txt";
                        dialogStatus_.clear();
                    } else if (playRect.contains(mx, my)) {
                        algoDialogOpen_ = true;
                    } else {
                        int palIdx = getPaletteFromMouse(event.mouseButton.x, event.mouseButton.y);
                        if (palIdx >= 0) {
                            if (palette_[palIdx].tile == 'C') {
                                float ddX = paletteOffX_ + PALETTE_CELL + 100;
                                float py = gridOffY_ + palIdx * (PALETTE_CELL + PALETTE_GAP);
                                float ddW = 90.f;
                                sf::FloatRect upRect(ddX + ddW - 26, py, 26, PALETTE_CELL / 2);
                                sf::FloatRect downRect(ddX + ddW - 26, py + PALETTE_CELL / 2, 26, PALETTE_CELL / 2);

                                if (upRect.contains(mx, my)) {
                                    checkpointNumber_++;
                                    if (checkpointNumber_ > 9) checkpointNumber_ = 0;
                                } else if (downRect.contains(mx, my)) {
                                    checkpointNumber_--;
                                    if (checkpointNumber_ < 0) checkpointNumber_ = 9;
                                } else {
                                    selectedPalette_ = palIdx;
                                }
                            } else {
                                selectedPalette_ = palIdx;
                            }
                        } else {
                            auto [row, col] = getCellFromMouse(event.mouseButton.x, event.mouseButton.y);
                            if (isInsideGrid(row, col)) {
                                if (event.mouseButton.button == sf::Mouse::Left) {
                                    paintCell(row, col);
                                } else if (event.mouseButton.button == sf::Mouse::Right) {
                                    cycleWeight(row, col);
                                }
                            }
                        }

                        float weightBtnY = gridOffY_ + rows_ * cellSize_ + 20;
                        sf::FloatRect weightBtnRect(gridOffX_, weightBtnY, 140.f, 30.f);
                        if (weightBtnRect.contains(mx, my)) {
                            showWeights_ = !showWeights_;
                        }

                        float sizeBtnY = gridOffY_ + rows_ * cellSize_ + 55;
                        sf::FloatRect rowUpRect(gridOffX_ + 215, sizeBtnY, 34, 26);
                        sf::FloatRect rowDownRect(gridOffX_ + 255, sizeBtnY, 34, 26);
                        sf::FloatRect colUpRect(gridOffX_ + 315, sizeBtnY, 34, 26);
                        sf::FloatRect colDownRect(gridOffX_ + 355, sizeBtnY, 34, 26);

                        if (rowUpRect.contains(mx, my) && rows_ < 20) {
                            rows_++; initGrid(); rebuildWindow();
                        } else if (rowDownRect.contains(mx, my) && rows_ > 3) {
                            rows_--; initGrid(); rebuildWindow();
                        } else if (colUpRect.contains(mx, my) && cols_ < 20) {
                            cols_++; initGrid(); rebuildWindow();
                        } else if (colDownRect.contains(mx, my) && cols_ > 3) {
                            cols_--; initGrid(); rebuildWindow();
                        }
                    }
                }
            } else if (event.type == sf::Event::MouseWheelScrolled && !dialogOpen_ && !algoDialogOpen_) {
                int palIdx = getPaletteFromMouse(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
                if (palIdx >= 0 && palette_[palIdx].tile == 'C') {
                    if (event.mouseWheelScroll.delta > 0) {
                        checkpointNumber_++;
                        if (checkpointNumber_ > 9) checkpointNumber_ = 0;
                    } else {
                        checkpointNumber_--;
                        if (checkpointNumber_ < 0) checkpointNumber_ = 9;
                    }
                }
            }
        }

        if (!dialogOpen_ && !algoDialogOpen_ && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i pos = sf::Mouse::getPosition(window_);
            auto [row, col] = getCellFromMouse(pos.x, pos.y);
            if (isInsideGrid(row, col)) {
                paintCell(row, col);
            }
        }

        window_.clear(sf::Color(30, 30, 40));
        drawToolbar();
        drawToolbarButtons();
        drawGrid();
        drawPalette();
        drawWeightToggle();
        drawSizeControls();
        if (dialogOpen_) {
            drawDialog();
        }
        if (algoDialogOpen_) {
            drawAlgorithmDialog();
        }
        window_.display();
    }

    return playRequested_;
}

} // namespace ui
