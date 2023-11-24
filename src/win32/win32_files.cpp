#include "win32_files.hpp"

#include <fstream>
#include <Shlwapi.h>

bool fileExists(const wchar_t *filePath) {
    std::ifstream file(filePath);
    return file.is_open();
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