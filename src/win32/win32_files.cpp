#include "win32_files.hpp"

#include "win32_memory.hpp"

#include <Shlwapi.h>

bool win32_fileExists(const WCHAR *filePath) {
    const DWORD fileAttributes = GetFileAttributes(filePath);
    return fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

static void setDirectoryPath(WCHAR *directoryPath, const int size) {
    GetModuleFileName(nullptr, directoryPath, size);
    PathRemoveFileSpec(directoryPath);
}

static int getWideStringLength(const WCHAR *str) {
    int length = 0;
    while (*str != L'\0') {
        ++length;
        ++str;
    }
    return length;
}

static void wideStringCopy(WCHAR *dest, const WCHAR *src, int size) {
    for (int i = 0; i < size; i++) {
        dest[i] = src[i];
    }
    dest[size] = L'\0';
}

void win32_setCompleteFilePath(WCHAR *dest, const int destSize, const WCHAR *relativeFilePath) {
    setDirectoryPath(dest, destSize);
    const int directoryPathLength = getWideStringLength(dest);
    const int relativeFilePathLen = getWideStringLength(relativeFilePath);
    if (const int totalPathLength = directoryPathLength + relativeFilePathLen + 1;
        totalPathLength < destSize) {
        dest[directoryPathLength] = L'\\';
        wideStringCopy(&dest[directoryPathLength + 1], relativeFilePath, relativeFilePathLen);
    }
}

HANDLE win32_openFile(const WCHAR *filePath) {
    const HANDLE file = CreateFile(
        filePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    return file;
}

uint32_t win32_getFileSize(const HANDLE file) {
    return GetFileSize(file, nullptr);
}

void win32_closeFile(const HANDLE file) {
    CloseHandle(file);
}

unsigned char *win32_loadFileIntoNewVirtualBuffer(const HANDLE fileHandle) {
    uint32_t fileSize = win32_getFileSize(fileHandle);
    auto fileData = static_cast<unsigned char *>(win32_allocateMemory(fileSize));
    ReadFile(fileHandle, fileData, fileSize, nullptr, nullptr);
    return fileData;
}
