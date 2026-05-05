#pragma once
#include <functional>
#include <unordered_set>
#include <vector>
#include "utils/fileIO.hpp"

namespace core {

struct Point {
    int row;
    int col;

    bool operator==(const Point& other) const {
        return row == other.row && col == other.col;
    }
};

struct State {
    Point pos;
    int visitedCheckpoints;

    bool operator==(const State& other) const {
        return pos == other.pos && visitedCheckpoints == other.visitedCheckpoints;
    }
};

struct StateHash {
    size_t operator()(const State& s) const {
        size_t h1 = std::hash<int>()(s.pos.row);
        size_t h2 = std::hash<int>()(s.pos.col);
        size_t h3 = std::hash<int>()(s.visitedCheckpoints);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

using StateSet = std::unordered_set<State, StateHash>;

struct Successor {
    State state;
    int cost;
    char move;
};

class Graph {
private:
    int rows_;
    int cols_;
    std::vector<std::vector<char>> grid_;
    std::vector<std::vector<int>> weights_;
    Point startPos_;
    Point goalPos_;
    std::vector<Point> checkpoints_;
    int totalCheckpoints_;

    bool isInBounds(int row, int col) const;
    bool isBlocked(int row, int col) const;
    int getNextRequiredCheckpoint(int visitedBitmask) const;
    int getCheckpointIndex(int row, int col) const;

public:
    explicit Graph(const fileUtil::PuzzleData& data);

    State getInitialState() const;
    bool isGoal(const State& state) const;
    std::vector<Successor> getSuccessors(const State& state) const;

    Point getStartPos() const;
    Point getGoalPos() const;
    int getCheckpointCount() const;
    char getTile(int row, int col) const;
    int getWeight(int row, int col) const;
    int getRows() const;
    int getCols() const;
};

} // namespace core