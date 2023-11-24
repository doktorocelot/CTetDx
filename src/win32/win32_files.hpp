#pragma once

#include <windows.h>
#include <string>

bool fileExists(const wchar_t *filePath);

void win32_setCompleteFilePath(WCHAR *dest, const int destSize, LPCWSTR relativeFilePath);