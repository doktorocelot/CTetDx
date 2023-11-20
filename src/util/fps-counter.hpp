#pragma once
struct FpsCounter;

FpsCounter *fpsCounter_create(int maxHistory);
void fpsCounter_pushFrameTime(FpsCounter *counter, double frameTime);
double fpsCounter_getFps(FpsCounter *counter);
double fpsCounter_getAverageFrameTime(FpsCounter *counter);
void fpsCounter_destroy(FpsCounter *counter);