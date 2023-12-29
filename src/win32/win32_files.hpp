#pragma once

#include <cstdint>
#include <windows.h>

bool win32_fileExists(const WCHAR *filePath);

void win32_setCompleteFilePath(WCHAR *dest, int destSize, LPCWSTR relativeFilePath);

HANDLE win32_openFile(const WCHAR *filePath);

uint32_t win32_getFileSize(HANDLE file);

void win32_closeFile(HANDLE file);

unsigned char *win32_loadFileIntoNewVirtualBuffer(const HANDLE fileHandle);