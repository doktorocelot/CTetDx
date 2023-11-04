#pragma once

#include <unordered_map>

enum Control {
    Control_SHIFT_LEFT,
    Control_SHIFT_RIGHT,
    Control_SOFT_DROP,
    Control_HARD_DROP,
    Control_ROTATE_LEFT,
    Control_ROTATE_RIGHT,
    Control_RETRY,
    Control_LENGTH,
};

struct ControlTracker {
    bool currentControls[Control_LENGTH];
    bool prevControls[Control_LENGTH];
    std::unordered_map<char, Control> keyAssign;
};

void controlTracker_updateCurrent(ControlTracker *tracker);

void controlTracker_copyCurrentToPrev(ControlTracker *tracker);

bool keyDown(ControlTracker *tracker, Control control);
bool keyPressed(ControlTracker *tracker, Control control);
bool keyReleased(ControlTracker *tracker, Control control);
