#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "core/graph.hpp"
#include "ui/snapshot.hpp"

namespace ui {

struct RenderConfig {
    int cellSize = 80;
    int windowPadding = 40;
    int infoPanelHeight = 160;
    int sliderHeight = 30;
    int sliderPadding = 10;
    int buttonHeight = 30;
    sf::Color bgColor = sf::Color(30, 30, 40);
    sf::Color wallColor = sf::Color(60, 60, 70);
    sf::Color lavaColor = sf::Color(200, 50, 50);
    sf::Color floorColor = sf::Color(180, 200, 220);
    sf::Color goalColor = sf::Color(50, 200, 80);
    sf::Color playerColor = sf::Color(50, 150, 255);
    sf::Color checkpointColor = sf::Color(255, 200, 50);
    sf::Color visitedCheckpointColor = sf::Color(100, 180, 100);
    sf::Color textColor = sf::Color(220, 220, 220);
    sf::Color sliderTrackColor = sf::Color(80, 80, 100);
    sf::Color sliderHandleColor = sf::Color(120, 140, 200);
    sf::Color buttonColor = sf::Color(70, 80, 110);
    sf::Color buttonHoverColor = sf::Color(100, 110, 150);
};

class GameUI {
private:
    const core::Graph& graph_;
    RenderConfig config_;
    sf::RenderWindow window_;
    sf::Font font_;

    float cellWidth_;
    float cellHeight_;
    float offsetX_;
    float offsetY_;

    bool showWeights_ = true;
    bool draggingSlider_ = false;

    sf::FloatRect sliderTrackRect_;
    sf::FloatRect sliderHandleRect_;
    sf::FloatRect weightButtonRect_;

    void drawGrid(const Snapshot& snapshot);
    void drawInfoPanel(const PlaybackController& controller);
    void drawSlider(const PlaybackController& controller);
    void drawWeightButton();
    void drawCell(int row, int col, const Snapshot& snapshot);

    bool isMouseOverButton(const sf::Vector2i& mousePos, const sf::FloatRect& rect) const;

public:
    GameUI(const core::Graph& graph, const RenderConfig& config = RenderConfig());

    void run(const SolutionLog& log);
};

} // namespace ui