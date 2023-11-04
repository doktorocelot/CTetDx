#include <iostream>
#include <sstream>
#include "die.hpp"
#include "windows.h"

void die(const char *reason) {
    std::ostringstream errorMsg;
    errorMsg << "Critical Error: " << reason << std::endl;
    OutputDebugStringA(errorMsg.str().c_str());
    std::cerr << errorMsg.str();
    MessageBoxA(nullptr, errorMsg.str().c_str(), "Critical Error", MB_ICONERROR | MB_OK);
    exit(-1);
}
