#include "Memory.hxx"
#include "Native.Basic.hxx"

#include <malloc.h>

namespace Memory
{
    void* AllocateMemory(const u32 size)
    {
        auto ptr = malloc(size);

        if (ptr != NULL) { ZeroMemory(ptr, size); }

        return ptr;
    }

    // TODO: It looks like the game has some extra logic here. Looks like not needed.
    void* AllocateMemory1(const u32 size)
    {
        // TODO: Streamline calls.
        return AllocateMemory(size);
    }

    void* AllocateMemory2(const u32 size)
    {
        // TODO: Streamline calls.
        return AllocateMemory1(size);
    }

    // TODO name
    void ReleaseMemory1(void* ptr) { free(ptr); }

    // TODO name
    void ReleaseMemory2(void* ptr)
    {
        if (ptr == NULL) { return; }

        ReleaseMemory1(ptr);
    }

    // TODO name
    void ReleaseMemory3(void* ptr) { ReleaseMemory2(ptr); }

    void* ReallocateMemory(void* ptr, const u32 size) { return realloc(ptr, size); }
}