#pragma once

#include "Basic.hxx"

namespace Memory
{
    // TODO
    OPTIONS(DisposeMode, u32)
    {
        None = 0,
        Unknown1 = 1,
        Unknown2 = 2,
        Collection = 4
    };

    void* AllocateMemory(const u32 size);
    void* AllocateMemory1(const u32 size);
    void* AllocateMemory2(const u32 size);

    void ReleaseMemory1(void* ptr);
    void ReleaseMemory2(void* ptr);
    void ReleaseMemory3(void* ptr);

    void* ReallocateMemory(void* ptr, const u32 size);
}