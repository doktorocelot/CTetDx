#pragma once

#include <windows.h>

bool fileExists(const wchar_t *filePath);

void win32_setCompleteFilePath(WCHAR *dest, int destSize, LPCWSTR relativeFilePath);