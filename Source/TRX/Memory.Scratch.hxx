#pragma once

#include "Basic.hxx"

namespace Memory::Scratch
{
    void InitializeScratch(void);
    void ReleaseScratch(void);

    void* AllocateScratchMemory(const u32 size);
    void ReleaseScratchMemory(const u32 size);
}