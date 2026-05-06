#include "ui/snapshot.hpp"

namespace ui {

PlaybackController::PlaybackController(const SolutionLog& log)
    : log_(log), currentStep_(0) {}

const Snapshot& PlaybackController::current() const {
    return log_.snapshots[currentStep_];
}

void PlaybackController::next() {
    if (hasNext()) currentStep_++;
}

void PlaybackController::prev() {
    if (hasPrev()) currentStep_--;
}

void PlaybackController::goTo(int step) {
    if (step >= 0 && step < static_cast<int>(log_.snapshots.size())) {
        currentStep_ = step;
    }
}

bool PlaybackController::hasNext() const {
    return currentStep_ < static_cast<int>(log_.snapshots.size()) - 1;
}

bool PlaybackController::hasPrev() const {
    return currentStep_ > 0;
}

int PlaybackController::currentStep() const { return currentStep_; }
int PlaybackController::totalSteps() const { return static_cast<int>(log_.snapshots.size()); }

} // namespace ui