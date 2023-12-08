#include <algorithm>
#include "fps-counter.hpp"

static double sumFrameTimes(const FpsCounter *counter) {
    double frameDeltas = 0;
    for (int i = 0; i < counter->totalPushed; i++) {
        frameDeltas += counter->frameTimes[i];
    }
    return frameDeltas;
}

size_t fpsCounter_getSize(int maxHistory){
    return sizeof(FpsCounter) + maxHistory * sizeof(double);
}

void fpsCounter_init(FpsCounter *counter, int maxHistory) {
    counter->length = maxHistory;
    counter->totalPushed = 0;
    counter->ptr = 0;
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