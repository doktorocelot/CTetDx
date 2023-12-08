#pragma once
struct FpsCounter {
    int length;
    unsigned int ptr;
    int totalPushed;
    double frameTimes[];
};

size_t fpsCounter_getSize(int maxHistory);
void fpsCounter_init(FpsCounter *counter, int maxHistory);
void fpsCounter_pushFrameTime(FpsCounter *counter, double frameTime);
double fpsCounter_getFps(FpsCounter *counter);
double fpsCounter_getAverageFrameTime(FpsCounter *counter);
