#include <cstdlib>
#include <algorithm>
#include "fps-counter.hpp"
#include "..\win32\win32_kill-program.hpp"

static double sumFrameTimes(const FpsCounter *counter) {
    double frameDeltas = 0;
    for (int i = 0; i < counter->totalPushed; i++) {
        frameDeltas += counter->frameTimes[i];
    }
    return frameDeltas;
}

FpsCounter* fpsCounter_create(int maxHistory) {
    size_t size = sizeof(FpsCounter) + maxHistory * sizeof(double);
    if (auto *counter = static_cast<FpsCounter *>(malloc(size)); !counter) {
        win32_killProgram(L"Could not allocate memory for FPS Counter.");
        return nullptr;
    }
    else {
        counter->length = maxHistory;
        counter->totalPushed = 0;
        counter->ptr = 0;
        return counter;
    }
}

void fpsCounter_destroy(FpsCounter *counter) {
    free(counter);
}

void fpsCounter_pushFrameTime(FpsCounter *counter, double frameTime) {
    counter->frameTimes[counter->ptr++] = frameTime;
    counter->ptr %= counter->length;
    counter->totalPushed = std::min(counter->totalPushed + 1, counter->length);
}

double fpsCounter_getFps(FpsCounter *counter) {
    double frameDeltas = sumFrameTimes(counter);
    return counter->totalPushed / frameDeltas;
}

double fpsCounter_getAverageFrameTime(FpsCounter *counter) {
    double frameDeltas = sumFrameTimes(counter);
    return frameDeltas / counter->totalPushed;
}
