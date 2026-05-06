#pragma once
#include <string>
#include <vector>
#include "core/graph.hpp"

namespace ui {

struct Snapshot {
    core::Point playerPos;
    int visitedCheckpoints;
    char move;
    int stepNumber;
};

struct SolutionLog {
    std::string algorithmName;
    int totalSteps;
    int iterations;
    long long executionTimeMs;
    bool found;
    std::vector<Snapshot> snapshots;
};

class PlaybackController {
private:
    const SolutionLog& log_;
    int currentStep_;

public:
    explicit PlaybackController(const SolutionLog& log);

    const Snapshot& current() const;
    void next();
    void prev();
    void goTo(int step);
    bool hasNext() const;
    bool hasPrev() const;
    int currentStep() const;
    int totalSteps() const;
};

} // namespace ui