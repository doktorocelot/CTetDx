#include "win32_files.hpp"

#include <Shlwapi.h>

bool fileExists(const wchar_t *filePath) {
    const DWORD fileAttributes = GetFileAttributes(filePath);
    return fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

static void setDirectoryPath(WCHAR *directoryPath, const int size) {
    GetModuleFileName(nullptr, directoryPath, size);
    PathRemoveFileSpec(directoryPath);
}

void win32_setCompleteFilePath(std::wstring *dest, const LPCWSTR relativeFilePath) {
    WCHAR directoryPath[MAX_PATH];
    setDirectoryPath(directoryPath, MAX_PATH);
    dest->append(directoryPath);
    dest->append(L"\\");
    dest->append(relativeFilePath);
}