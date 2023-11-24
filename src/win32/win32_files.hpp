#pragma once

#include <windows.h>
#include <string>

bool fileExists(const wchar_t *filePath);

void win32_setCompleteFilePath(std::wstring *dest, LPCWSTR relativeFilePath);