#include "win32_memory.hpp"
#include <Windows.h>

#include "win32_kill-program.hpp"

void *win32_allocateMemory(const size_t amount) {
    const auto ptr = VirtualAlloc(nullptr, amount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (ptr == nullptr) win32_killProgram(L"Out of memory.");
    return ptr;
}

void win32_deallocateMemory(void *memory) {
    VirtualFree(memory, 0, MEM_RELEASE);
}