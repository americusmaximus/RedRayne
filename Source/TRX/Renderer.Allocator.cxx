#include "Renderer.hxx"
#include "Renderer.Allocator.hxx"
#include "Memory.hxx"
#include "Logger.hxx"

#define VERTEX_BUFFER_MEMORY_ALIGNMENT_VALUE 64
#define VERTEX_BUFFER_MEMORY_ALIGNMENT_MASK (~(VERTEX_BUFFER_MEMORY_ALIGNMENT_VALUE-1))

#define ALLOCATOR_MEMORY_BUFFER_START_OFFSET (-5)
#define ALLOCATOR_MEMORY_BUFFER_COUNT_OFFSET (-4)

#define ALLOCATOR_MEMORY_BUFFER_ELEMENT_SIZE 32
#define ALLOCATOR_MEMORY_BUFFER_ELEMENT_MASK (~(ALLOCATOR_MEMORY_BUFFER_ELEMENT_SIZE-1))
#define ALLOCATOR_MEMORY_BUFFER_ELEMENT_INVERTED_MASK (ALLOCATOR_MEMORY_BUFFER_ELEMENT_SIZE-1)

#define ALLOCATOR_MEMORY_BUFFER_HEADER_SIZE 36

using namespace Memory;
using namespace Logger;


namespace Renderer::Allocator
{
    void AllocateVertexBuffer2(void)
    {
        if (*RendererState.Allocated.Vertexes._AllocatedMemory1 != NULL) { return; }

        void* ptr = AllocateMemory(RENDERER_TEN_THOUSAND_ELEMENT_COUNT * sizeof(ProperTLVertex) + VERTEX_BUFFER_MEMORY_ALIGNMENT_VALUE);

        if (ptr == NULL)
        {
            LogError("Unable to allocate vertex memory.");
        }

        void* aligned = (void*)(((addr)ptr + VERTEX_BUFFER_MEMORY_ALIGNMENT_VALUE) & VERTEX_BUFFER_MEMORY_ALIGNMENT_MASK);

        *RendererState.Allocated.Vertexes._AllocatedMemory1 = ptr;
        *RendererState.Allocated.Vertexes._AlignedAllocatedMemory1 = aligned;
        *RendererState.Allocated.Vertexes._AlignedAllocatedMemory1Copy = *RendererState.Allocated.Vertexes._AlignedAllocatedMemory1;
    }

    void ReleaseVertexeBuffer(void)
    {
        ReleaseAllocatedRendererMemory2(*RendererState.Allocated._AllocatedMemory2);

        *RendererState.Allocated._AllocatedMemory2 = NULL;

        if (*RendererState.Allocated.Vertexes._AllocatedMemory1 == NULL) { return; }

        ReleaseMemory1(*RendererState.Allocated.Vertexes._AllocatedMemory1);

        *RendererState.Allocated.Vertexes._AllocatedMemory1 = NULL;
        *RendererState.Allocated.Vertexes._AlignedAllocatedMemory1 = NULL;
        *RendererState.Allocated.Vertexes._AlignedAllocatedMemory1Copy = NULL;
    }

    void* AllocateRendererMemory1(const s32 count)
    {
        if (count < MIN_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT || MAX_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT < count)
        {
            LogError("Unable to allocate memory for %d vertexes, maximum is %d.", count, MAX_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT);
        }

        const auto ptr = AllocateMemory(count * ALLOCATOR_MEMORY_BUFFER_ELEMENT_SIZE + ALLOCATOR_MEMORY_BUFFER_HEADER_SIZE);

        if (ptr == NULL)
        {
            LogError("Unable to allocate memory for %d vertexes.", count);
        }

        const auto original = (addr)ptr;
        const auto address = ((original + ALLOCATOR_MEMORY_BUFFER_HEADER_SIZE) & ALLOCATOR_MEMORY_BUFFER_ELEMENT_MASK);

        *(s8*)(address + ALLOCATOR_MEMORY_BUFFER_START_OFFSET) = (address - original);
        *(s32*)(address + ALLOCATOR_MEMORY_BUFFER_COUNT_OFFSET) = count;

        return (void*)address;
    }

    void ReleaseAllocatedRendererMemory2(const void* ptr)
    {
        if (ptr == NULL) { return; }

        const auto address = (addr)ptr;

        if ((address & ALLOCATOR_MEMORY_BUFFER_ELEMENT_INVERTED_MASK) != 0)
        {
            LogError("Unable to release memory, the pointer isn't aligned.");
        }

        s8 start = *(s8*)(address + ALLOCATOR_MEMORY_BUFFER_START_OFFSET);
        s32 count = *(s32*)(address + ALLOCATOR_MEMORY_BUFFER_COUNT_OFFSET);

        if (start < (ALLOCATOR_MEMORY_BUFFER_HEADER_SIZE + 1)
            && (ALLOCATOR_MEMORY_BUFFER_HEADER_SIZE - ALLOCATOR_MEMORY_BUFFER_ELEMENT_SIZE) < start
            && (MIN_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT - 1) < count
            && count < (MAX_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT + 1))
        {
            ReleaseMemory1((void*)(address - start));

            return;
        }

        LogError("Unable to release corrupted memory. Address is %d, count is %d.", start, count);
    }

    void AllocateRendererMemory2(void)
    {
        if (*RendererState.Allocated._AllocatedMemory3 == NULL)
        {
            *RendererState.Allocated._AllocatedMemory3 = AllocateRendererMemory1(UNKNOWN_MEMORY_3_ELEMENT_COUNT);//TODO
        }

        if (*RendererState.Allocated._AllocatedMemory4 == NULL)
        {
            *RendererState.Allocated._AllocatedMemory4 = AllocateRendererMemory1(UNKNOWN_MEMORY_4_ELEMENT_COUNT);//TODO
        }

        if (*RendererState.Allocated._AllocatedMemory5 == NULL)
        {
            *RendererState.Allocated._AllocatedMemory5 = AllocateRendererMemory1(UNKNOWN_MEMORY_5_ELEMENT_COUNT);//TODO
        }

        if (*RendererState.Allocated._AllocatedMemory6 == NULL)
        {
            *RendererState.Allocated._AllocatedMemory6 = AllocateRendererMemory1(UNKNOWN_MEMORY_6_ELEMENT_COUNT);//TODO
        }
    }

    void* ReAllocateRendererMemory(void* ptr, const s32 count)
    {
        if (count == 0)
        {
            ReleaseAllocatedRendererMemory2(ptr);

            return NULL;
        }

        if (count < MIN_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT || MAX_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT < count)
        {
            LogError("Unable to allocate memory for %d vertexes, maximum is %d.", count, MAX_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT);
        }

        if (ptr == NULL) { return AllocateRendererMemory1(count); }

        const auto address = (addr)ptr;

        if ((address & ALLOCATOR_MEMORY_BUFFER_ELEMENT_INVERTED_MASK) != 0)
        {
            LogError("Unable to allocate memory, the pointer isn't aligned.");
        }

        s8 start = *(s8*)(address + ALLOCATOR_MEMORY_BUFFER_START_OFFSET);
        s32 vcount = *(s32*)(address + ALLOCATOR_MEMORY_BUFFER_COUNT_OFFSET);

        if (ALLOCATOR_MEMORY_BUFFER_HEADER_SIZE < start
            || start < (ALLOCATOR_MEMORY_BUFFER_HEADER_SIZE - ALLOCATOR_MEMORY_BUFFER_ELEMENT_SIZE + 1)
            || vcount < MIN_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT
            || MAX_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT < vcount)
        {
            LogError("Unable to allocate memory. Memory is corrupted at address %d, count is %d.", start, vcount);
        }

        if (count == vcount) { return ptr; }

        const auto result = AllocateRendererMemory1(count);

        s32 actualCount = count;
        if (vcount <= actualCount) { actualCount = vcount; }

        CopyMemory(result, ptr, actualCount);

        return result;
    }

    void ReleaseAllocatedRendererMemory3(void)
    {
        if (*RendererState.Allocated._AllocatedMemory7 == NULL) { return; }

        ReleaseAllocatedRendererMemory2(*RendererState.Allocated._AllocatedMemory7);
        
        *RendererState.Allocated._AllocatedMemory7 = NULL;
    }

    void AllocateRendererMemory3(void)
    {
        *RendererState.Allocated._AllocatedMemory7 = AllocateRendererMemory1(UNKNOWN_MEMORY_7_ELEMENT_COUNT);//TODO
    }

    void AllocateRendererMemory4(void)
    {
        if (*RendererState.Allocated._AllocatedMemory3 != NULL)
        {
            ReleaseAllocatedRendererMemory2(*RendererState.Allocated._AllocatedMemory3);
            *RendererState.Allocated._AllocatedMemory3 = NULL;
        }

        if (*RendererState.Allocated._AllocatedMemory4 != NULL)
        {
            ReleaseAllocatedRendererMemory2(*RendererState.Allocated._AllocatedMemory4);
            *RendererState.Allocated._AllocatedMemory4 = NULL;
        }

        if (*RendererState.Allocated._AllocatedMemory5 != NULL)
        {
            ReleaseAllocatedRendererMemory2(*RendererState.Allocated._AllocatedMemory5);
            *RendererState.Allocated._AllocatedMemory5 = NULL;
        }

        if (*RendererState.Allocated._AllocatedMemory6 != NULL)
        {
            ReleaseAllocatedRendererMemory2(*RendererState.Allocated._AllocatedMemory6);
            *RendererState.Allocated._AllocatedMemory6 = NULL;
        }
    }
}