#include "Memory.hxx"
#include "Memory.Scratch.hxx"
#include "Logger.hxx"

#define SCRATCH_MEMORY_ALIGNMENT_VALUE 16
#define SCRATCH_MEMORY_ALIGNMENT_MASK (~(SCRATCH_MEMORY_ALIGNMENT_VALUE-1))

// 16KB
#define USABLE_SCRATCH_MEMORY_SIZE (16 * 1024)
#define ALLOCATED_SCRATCH_MEMORY_SIZE (USABLE_SCRATCH_MEMORY_SIZE + SCRATCH_MEMORY_ALIGNMENT_VALUE)

using namespace Logger;

// TODO: Refactor to use indexes.
namespace Memory::Scratch
{
    struct
    {
        void* AllocatedMemory;// = (void**)0x0084b6d4;// TODO

        void* AlignedMemoryStart;// = (void**)0x0084b6d0;// TODO
        void* AlignedMemoryEnd;// = (void**)0x0084b6d8;// TODO
    } ScratchState;

    void InitializeScratch(void)
    {
        void* ptr = AllocateMemory(ALLOCATED_SCRATCH_MEMORY_SIZE);

        if (ptr == NULL)
        {
            LogError("Unable to allocate memory for scratch buffer.");
        }

        void* aligned = (void*)(((addr)ptr + SCRATCH_MEMORY_ALIGNMENT_VALUE) & SCRATCH_MEMORY_ALIGNMENT_MASK);

        ScratchState.AllocatedMemory = ptr;
        ScratchState.AlignedMemoryStart = aligned;
        ScratchState.AlignedMemoryEnd = aligned;
    }

    void* AllocateScratchMemory(const u32 size)
    {
        const auto alignedSize = (size + SCRATCH_MEMORY_ALIGNMENT_VALUE) & SCRATCH_MEMORY_ALIGNMENT_MASK;

        const auto result = ScratchState.AlignedMemoryEnd;
        ScratchState.AlignedMemoryEnd = (void*)((addr)ScratchState.AlignedMemoryEnd + alignedSize);

        if ((addr)ScratchState.AlignedMemoryEnd <= (addr)ScratchState.AlignedMemoryStart + USABLE_SCRATCH_MEMORY_SIZE)
        {
            return result;
        }

        LogError("Unable to allocate memory from scratch buffer memory pool.");
    }

    void ReleaseScratchMemory(const u32 size)
    {
        const auto alignedSize = (size + SCRATCH_MEMORY_ALIGNMENT_VALUE) & SCRATCH_MEMORY_ALIGNMENT_MASK;

        ScratchState.AlignedMemoryEnd = (void*)((addr)ScratchState.AlignedMemoryEnd - alignedSize);

        if ((addr)ScratchState.AlignedMemoryEnd < (addr)ScratchState.AlignedMemoryStart)
        {
            LogError("Unableto release memory to scratch buffer memory pool.");
        }
    }

    void ReleaseScratch(void)
    {
        if (ScratchState.AllocatedMemory != NULL)
        {
            ReleaseMemory1(ScratchState.AllocatedMemory);
        }

        ScratchState.AllocatedMemory = NULL;
        ScratchState.AlignedMemoryStart = NULL;
        ScratchState.AlignedMemoryEnd = NULL;
    }
}