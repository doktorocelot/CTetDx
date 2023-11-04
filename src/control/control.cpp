#include <windows.h>
#include "control.hpp"

void controlTracker_updateCurrent(ControlTracker *tracker) {
    for (const auto& entry : tracker->keyAssign) {
        short state = GetAsyncKeyState(entry.first);
        tracker->currentControls[entry.second] = (state & 0x8000) != 0;
    }
}

void controlTracker_copyCurrentToPrev(ControlTracker *tracker) {
    memcpy(tracker->prevControls, tracker->currentControls, sizeof(tracker->prevControls));
}

bool keyDown(ControlTracker *tracker, Control control) {
    return tracker->currentControls[control];
}

bool keyPressed(ControlTracker *tracker, Control control) {
    return !tracker->prevControls[control] && tracker->currentControls[control];
}

bool keyReleased(ControlTracker *tracker, Control control) {
    return tracker->prevControls[control] && !tracker->currentControls[control];
}
