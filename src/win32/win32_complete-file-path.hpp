#pragma once

#include <windows.h>
#include <string>

void win32_setCompleteFilePath(std::wstring *dest, LPCWSTR relativeFilePath);