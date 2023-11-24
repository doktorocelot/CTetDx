#pragma once

#include <windows.h>

bool win32_fileExists(const WCHAR *filePath);

void win32_setCompleteFilePath(WCHAR *dest, int destSize, LPCWSTR relativeFilePath);

HANDLE win32_openFile(const WCHAR *fileName);

void win32_closeFile(HANDLE file);