#include <iostream>
#include <sstream>
#include "die.hpp"
#include "windows.h"

void die(const wchar_t *reason) {
    std::wostringstream errorMsg;
    errorMsg << "Critical Error: " << reason << std::endl;
    OutputDebugString(errorMsg.str().c_str());
    std::wcerr << errorMsg.str();
    MessageBox(nullptr, errorMsg.str().c_str(), L"Critical Error", MB_ICONERROR | MB_OK);
    exit(-1);
}
