#pragma once

#include <windows.h>

bool fileExists(const WCHAR *filePath);

void win32_setCompleteFilePath(WCHAR *dest, int destSize, LPCWSTR relativeFilePath);