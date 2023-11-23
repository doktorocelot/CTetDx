#include "win32_complete-file-path.hpp"

#include <fstream>
#include <Shlwapi.h>

static void setDirectoryPath(WCHAR *directoryPath, int size) {
    GetModuleFileName(nullptr, directoryPath, size);
    PathRemoveFileSpec(directoryPath);
}

void setCompleteFilePath(std::wstring *dest, LPCWSTR relativeFilePath) {
    WCHAR directoryPath[MAX_PATH];
    setDirectoryPath(directoryPath, MAX_PATH);
    dest->append(directoryPath);
    dest->append(L"\\");
    dest->append(relativeFilePath);
}