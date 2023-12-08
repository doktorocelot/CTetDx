#include "win32_memory.hpp"
#include <Windows.h>

void *win32_allocateMemory(size_t amount) {
    return VirtualAlloc(NULL, amount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void win32_deallocateMemory(void *memory) {
    VirtualFree(memory, 0, MEM_RELEASE);
}