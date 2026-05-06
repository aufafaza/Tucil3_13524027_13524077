#include "ui/gameui.hpp"
#include <sstream>

namespace ui {

GameUI::GameUI(const core::Graph& graph, const RenderConfig& config)
    : graph_(graph), config_(config) {
    int gridPixelW = graph_.getCols() * config_.cellSize;
    int gridPixelH = graph_.getRows() * config_.cellSize;
    int winW = gridPixelW + 2 * config_.windowPadding;
    int winH = gridPixelH + 2 * config_.windowPadding + config_.infoPanelHeight;

    cellWidth_ = static_cast<float>(config_.cellSize);
    cellHeight_ = static_cast<float>(config_.cellSize);
    offsetX_ = static_cast<float>(config_.windowPadding);
    offsetY_ = static_cast<float>(config_.windowPadding);

    window_.create(sf::VideoMode(winW, winH), "Sliding Ice Puzzle");
    window_.setFramerateLimit(60);

    const char* fontPaths[] = {
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
        "C:/Windows/Fonts/arial.ttf",
    };
    bool fontLoaded = false;
    for (const char* path : fontPaths) {
        if (font_.loadFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
    (void)fontLoaded;

    sliderTrackRect_ = sf::FloatRect(
        offsetX_,
        offsetY_ + static_cast<float>(gridPixelH) + 80,
        static_cast<float>(gridPixelW),
        static_cast<float>(config_.sliderHeight)
    );

    weightButtonRect_ = sf::FloatRect(
        offsetX_,
        sliderTrackRect_.top + static_cast<float>(config_.sliderHeight) + static_cast<float>(config_.sliderPadding),
        160.f,
        static_cast<float>(config_.buttonHeight)
    );
}

void GameUI::drawCell(int row, int col, const Snapshot& snapshot) {
    char tile = graph_.getTile(row, col);
    sf::RectangleShape rect(sf::Vector2f(cellWidth_ - 2, cellHeight_ - 2));
    rect.setPosition(offsetX_ + col * cellWidth_ + 1, offsetY_ + row * cellHeight_ + 1);

    if (tile == 'X') {
        rect.setFillColor(config_.wallColor);
        window_.draw(rect);
        return;
    }

    if (tile == 'L') {
        rect.setFillColor(config_.lavaColor);
        window_.draw(rect);
        sf::Text label("L", font_, 20);
        label.setPosition(rect.getPosition().x + cellWidth_ / 2 - 6,
                          rect.getPosition().y + cellHeight_ / 2 - 10);
        label.setFillColor(sf::Color::White);
        window_.draw(label);
        return;
    }

    if (tile == 'Z') {
        bool isPlayer = (snapshot.playerPos.row == row && snapshot.playerPos.col == col);
        if (isPlayer) {
            rect.setFillColor(config_.playerColor);
            window_.draw(rect);
            sf::Text label("P", font_, 22);
            label.setPosition(rect.getPosition().x + cellWidth_ / 2 - 6,
                              rect.getPosition().y + cellHeight_ / 2 - 12);
            label.setFillColor(sf::Color::White);
            window_.draw(label);
        } else {
            rect.setFillColor(config_.floorColor);
            window_.draw(rect);
            sf::Text label("Z", font_, 18);
            label.setPosition(rect.getPosition().x + cellWidth_ / 2 - 5,
                              rect.getPosition().y + cellHeight_ / 2 - 10);
            label.setFillColor(sf::Color(150, 150, 170));
            window_.draw(label);
        }
        return;
    }

    bool isPlayer = (snapshot.playerPos.row == row && snapshot.playerPos.col == col);

    if (tile >= '0' && tile <= '9') {
        int cpIdx = tile - '0';
        bool visited = (snapshot.visitedCheckpoints & (1 << cpIdx)) != 0;

        if (isPlayer) {
            rect.setFillColor(config_.playerColor);
            window_.draw(rect);
            sf::Text playerLabel("P", font_, 22);
            playerLabel.setPosition(rect.getPosition().x + cellWidth_ / 2 - 6,
                                    rect.getPosition().y + cellHeight_ / 2 - 12);
            playerLabel.setFillColor(sf::Color::White);
            window_.draw(playerLabel);

            std::string cpStr(1, tile);
            sf::Text cpSmall(cpStr, font_, 12);
            cpSmall.setPosition(rect.getPosition().x + cellWidth_ - 16,
                                rect.getPosition().y + 2);
            cpSmall.setFillColor(sf::Color(255, 255, 255, 180));
            window_.draw(cpSmall);
        } else if (visited) {
            rect.setFillColor(config_.visitedCheckpointColor);
            window_.draw(rect);

            sf::CircleShape check(8.f);
            check.setFillColor(sf::Color(50, 130, 50));
            check.setPosition(rect.getPosition().x + 4,
                              rect.getPosition().y + 2);
            window_.draw(check);

            std::string cpStr(1, tile);
            sf::Text cpNum(cpStr, font_, 22);
            cpNum.setPosition(rect.getPosition().x + cellWidth_ / 2 - 6,
                              rect.getPosition().y + cellHeight_ / 2 - 4);
            cpNum.setFillColor(sf::Color::White);
            window_.draw(cpNum);
        } else {
            rect.setFillColor(config_.checkpointColor);
            window_.draw(rect);

            sf::CircleShape circle(10.f);
            circle.setFillColor(sf::Color(220, 170, 30));
            circle.setPosition(rect.getPosition().x + cellWidth_ / 2 - 10,
                               rect.getPosition().y + cellHeight_ / 2 - 10);
            window_.draw(circle);

            std::string cpStr(1, tile);
            sf::Text cpNum(cpStr, font_, 22);
            cpNum.setPosition(rect.getPosition().x + cellWidth_ / 2 - 6,
                              rect.getPosition().y + cellHeight_ / 2 - 12);
            cpNum.setFillColor(sf::Color::Black);
            window_.draw(cpNum);
        }
        return;
    }

    bool isGoal = (row == graph_.getGoalPos().row && col == graph_.getGoalPos().col);

    if (isPlayer) {
        rect.setFillColor(config_.playerColor);
        window_.draw(rect);
        sf::Text playerLabel("P", font_, 22);
        playerLabel.setPosition(rect.getPosition().x + cellWidth_ / 2 - 6,
                                rect.getPosition().y + cellHeight_ / 2 - 12);
        playerLabel.setFillColor(sf::Color::White);
        window_.draw(playerLabel);
    } else if (isGoal) {
        rect.setFillColor(config_.goalColor);
        window_.draw(rect);
        sf::Text goalLabel("O", font_, 22);
        goalLabel.setPosition(rect.getPosition().x + cellWidth_ / 2 - 6,
                              rect.getPosition().y + cellHeight_ / 2 - 12);
        goalLabel.setFillColor(sf::Color::White);
        window_.draw(goalLabel);
    } else {
        rect.setFillColor(config_.floorColor);
        window_.draw(rect);
        if (tile == '*' && showWeights_) {
            sf::Text weightLabel(std::to_string(graph_.getWeight(row, col)), font_, 14);
            weightLabel.setPosition(rect.getPosition().x + cellWidth_ / 2 - 6,
                                   rect.getPosition().y + cellHeight_ / 2 - 8);
            weightLabel.setFillColor(sf::Color(80, 80, 110));
            window_.draw(weightLabel);
        }
    }
}

void GameUI::drawGrid(const Snapshot& snapshot) {
    for (int r = 0; r < graph_.getRows(); r++)
        for (int c = 0; c < graph_.getCols(); c++)
            drawCell(r, c, snapshot);
}

void GameUI::drawInfoPanel(const PlaybackController& controller) {
    int gridPixelH = graph_.getRows() * config_.cellSize;
    float panelY = offsetY_ + static_cast<float>(gridPixelH) + 10;

    std::ostringstream oss;
    const Snapshot& snap = controller.current();
    oss << "Step: " << controller.currentStep() << "/" << controller.totalSteps() - 1
        << "  Move: " << snap.move
        << "  Pos: (" << snap.playerPos.row << "," << snap.playerPos.col << ")"
        << "  Checkpoints: ";

    int totalCP = graph_.getCheckpointCount();
    bool anyVisited = false;
    for (int i = 0; i < totalCP; i++) {
        if (snap.visitedCheckpoints & (1 << i)) {
            oss << i << " ";
            anyVisited = true;
        }
    }
    if (!anyVisited) oss << "none";

    sf::Text info(oss.str(), font_, 16);
    info.setPosition(offsetX_, panelY);
    info.setFillColor(config_.textColor);
    window_.draw(info);

    sf::Text controls("Left/Right:step  Home/End:jump  Drag slider  W:weights  Q:quit", font_, 12);
    controls.setPosition(offsetX_, panelY + 22);
    controls.setFillColor(sf::Color(150, 150, 160));
    window_.draw(controls);
}

void GameUI::drawSlider(const PlaybackController& controller) {
    sf::RectangleShape track(sf::Vector2f(sliderTrackRect_.width, sliderTrackRect_.height));
    track.setPosition(sliderTrackRect_.left, sliderTrackRect_.top);
    track.setFillColor(config_.sliderTrackColor);
    track.setOutlineColor(sf::Color(120, 120, 150));
    track.setOutlineThickness(1.f);
    window_.draw(track);

    float handleW = 16.f;
    int total = controller.totalSteps();
    float fraction = (total > 1) ? static_cast<float>(controller.currentStep()) / (total - 1) : 0.f;
    float maxOffset = sliderTrackRect_.width - handleW;
    float handleX = sliderTrackRect_.left + fraction * maxOffset;

    sliderHandleRect_ = sf::FloatRect(handleX, sliderTrackRect_.top, handleW, sliderTrackRect_.height);

    sf::RectangleShape handle(sf::Vector2f(handleW, sliderTrackRect_.height));
    handle.setPosition(handleX, sliderTrackRect_.top);
    handle.setFillColor(draggingSlider_ ? sf::Color(160, 180, 240) : config_.sliderHandleColor);
    handle.setOutlineColor(sf::Color(180, 180, 220));
    handle.setOutlineThickness(1.f);
    window_.draw(handle);

    if (total > 1) {
        for (int i = 0; i < total; i++) {
            float tickX = sliderTrackRect_.left + (static_cast<float>(i) / (total - 1)) * maxOffset + handleW / 2.f;
            sf::RectangleShape tick(sf::Vector2f(2.f, sliderTrackRect_.height * 0.4f));
            tick.setPosition(tickX - 1.f, sliderTrackRect_.top + sliderTrackRect_.height * 0.6f);
            tick.setFillColor(sf::Color(140, 140, 170));
            window_.draw(tick);
        }
    }
}

void GameUI::drawWeightButton() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window_);
    bool hovered = isMouseOverButton(mousePos, weightButtonRect_);

    sf::RectangleShape button(sf::Vector2f(weightButtonRect_.width, weightButtonRect_.height));
    button.setPosition(weightButtonRect_.left, weightButtonRect_.top);
    button.setFillColor(hovered ? config_.buttonHoverColor : config_.buttonColor);
    button.setOutlineColor(sf::Color(140, 140, 180));
    button.setOutlineThickness(1.f);
    window_.draw(button);

    std::string label = showWeights_ ? "Weights: ON" : "Weights: OFF";
    sf::Text btnText(label, font_, 16);
    btnText.setPosition(weightButtonRect_.left + 12, weightButtonRect_.top + 5);
    btnText.setFillColor(config_.textColor);
    window_.draw(btnText);
}

bool GameUI::isMouseOverButton(const sf::Vector2i& mousePos, const sf::FloatRect& rect) const {
    return static_cast<float>(mousePos.x) >= rect.left &&
           static_cast<float>(mousePos.x) <= rect.left + rect.width &&
           static_cast<float>(mousePos.y) >= rect.top &&
           static_cast<float>(mousePos.y) <= rect.top + rect.height;
}

void GameUI::run(const SolutionLog& log) {
    if (log.snapshots.empty()) return;

    PlaybackController controller(log);
    int totalSteps = controller.totalSteps();

    while (window_.isOpen()) {
        sf::Event event;
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window_.close();
            } else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Right:
                        controller.next();
                        break;
                    case sf::Keyboard::Left:
                        controller.prev();
                        break;
                    case sf::Keyboard::Home:
                        controller.goTo(0);
                        break;
                    case sf::Keyboard::End:
                        controller.goTo(totalSteps - 1);
                        break;
                    case sf::Keyboard::W:
                        showWeights_ = !showWeights_;
                        break;
                    case sf::Keyboard::Q:
                    case sf::Keyboard::Escape:
                        window_.close();
                        break;
                    default:
                        break;
                }
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    float mx = static_cast<float>(event.mouseButton.x);
                    float my = static_cast<float>(event.mouseButton.y);
                    if (sliderTrackRect_.contains(mx, my)) {
                        draggingSlider_ = true;
                    }
                    if (draggingSlider_ && totalSteps > 0) {
                        float handleW = 16.f;
                        float maxOffset = sliderTrackRect_.width - handleW;
                        float fraction = (mx - sliderTrackRect_.left - handleW / 2.f) / maxOffset;
                        if (fraction < 0.f) fraction = 0.f;
                        if (fraction > 1.f) fraction = 1.f;
                        int step = static_cast<int>(fraction * (totalSteps - 1) + 0.5f);
                        controller.goTo(step);
                    }
                    if (weightButtonRect_.contains(mx, my)) {
                        showWeights_ = !showWeights_;
                    }
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    draggingSlider_ = false;
                }
            } else if (event.type == sf::Event::MouseMoved) {
                if (draggingSlider_ && totalSteps > 0) {
                    float mx = static_cast<float>(event.mouseMove.x);
                    float handleW = 16.f;
                    float maxOffset = sliderTrackRect_.width - handleW;
                    float fraction = (mx - sliderTrackRect_.left - handleW / 2.f) / maxOffset;
                    if (fraction < 0.f) fraction = 0.f;
                    if (fraction > 1.f) fraction = 1.f;
                    int step = static_cast<int>(fraction * (totalSteps - 1) + 0.5f);
                    controller.goTo(step);
                }
            }
        }

        window_.clear(config_.bgColor);
        drawGrid(controller.current());
        drawInfoPanel(controller);
        drawSlider(controller);
        drawWeightButton();
        window_.display();
    }
}

} // namespace ui
