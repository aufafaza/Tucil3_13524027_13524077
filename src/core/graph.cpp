#include "core/graph.hpp"

namespace core {

static const int DR[] = {-1, 1, 0, 0};
static const int DC[] = {0, 0, -1, 1};
static const char MOVES[] = {'U', 'D', 'L', 'R'};

Graph::Graph(const fileUtil::PuzzleData& data)
    : rows_(data.rows), cols_(data.cols),
      grid_(data.board), weights_(data.cost),
      startPos_{-1, -1}, goalPos_{-1, -1}, totalCheckpoints_(0) {

    for (int r = 0; r < rows_; r++) {
        for (int c = 0; c < cols_; c++) {
            char tile = grid_[r][c];
            if (tile == 'Z') {
                startPos_ = {r, c};
            } else if (tile == 'O') {
                goalPos_ = {r, c};
            } else if (tile >= '0' && tile <= '9') {
                int idx = tile - '0';
                if (idx >= static_cast<int>(checkpoints_.size())) {
                    checkpoints_.resize(idx + 1, {-1, -1});
                }
                checkpoints_[idx] = {r, c};
            }
        }
    }

    totalCheckpoints_ = 0;
    for (int i = 0; i < static_cast<int>(checkpoints_.size()); i++) {
        if (checkpoints_[i].row != -1) {
            totalCheckpoints_ = i + 1;
        }
    }
    checkpoints_.resize(totalCheckpoints_);
}

bool Graph::isInBounds(int row, int col) const {
    return row >= 0 && row < rows_ && col >= 0 && col < cols_;
}

bool Graph::isBlocked(int row, int col) const {
    return grid_[row][col] == 'X' || grid_[row][col] == 'L';
}

int Graph::getNextRequiredCheckpoint(int visitedBitmask) const {
    int idx = 0;
    while (idx < totalCheckpoints_ && (visitedBitmask & (1 << idx))) {
        idx++;
    }
    return (idx < totalCheckpoints_) ? idx : -1;
}

int Graph::getCheckpointIndex(int row, int col) const {
    for (int i = 0; i < static_cast<int>(checkpoints_.size()); i++) {
        if (checkpoints_[i].row == row && checkpoints_[i].col == col) {
            return i;
        }
    }
    return -1;
}

State Graph::getInitialState() const {
    return {startPos_, 0};
}

bool Graph::isGoal(const State& state) const {
    return state.pos.row == goalPos_.row && state.pos.col == goalPos_.col
           && state.visitedCheckpoints == ((1 << totalCheckpoints_) - 1);
}

std::vector<Successor> Graph::getSuccessors(const State& state) const {
    std::vector<Successor> successors;

    for (int d = 0; d < 4; d++) {
        int cr = state.pos.row;
        int cc = state.pos.col;
        int nr = cr + DR[d];
        int nc = cc + DC[d];

        if (!isInBounds(nr, nc) || isBlocked(nr, nc)) {
            continue;
        }

        int totalCost = 0;
        int visitedBits = state.visitedCheckpoints;

        while (true) {
            nr = cr + DR[d];
            nc = cc + DC[d];

            if (!isInBounds(nr, nc) || isBlocked(nr, nc)) {
                break;
            }

            cr = nr;
            cc = nc;
            totalCost += weights_[cr][cc];

            int cpIdx = getCheckpointIndex(cr, cc);
            if (cpIdx != -1) {
                int nextReq = getNextRequiredCheckpoint(visitedBits);
                if (nextReq != -1 && cpIdx == nextReq) {
                    visitedBits |= (1 << cpIdx);
                }
            }
        }

        if (cr == state.pos.row && cc == state.pos.col) {
            continue;
        }

        State newState = {{cr, cc}, visitedBits};
        successors.push_back({newState, totalCost, MOVES[d]});
    }

    return successors;
}

Point Graph::getStartPos() const { return startPos_; }
Point Graph::getGoalPos() const { return goalPos_; }
int Graph::getCheckpointCount() const { return totalCheckpoints_; }
char Graph::getTile(int row, int col) const { return grid_[row][col]; }
int Graph::getWeight(int row, int col) const { return weights_[row][col]; }
int Graph::getRows() const { return rows_; }
int Graph::getCols() const { return cols_; }

} // namespace core