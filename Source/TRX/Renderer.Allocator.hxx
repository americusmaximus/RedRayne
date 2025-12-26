#pragma once

#include "Basic.hxx"

#define MIN_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT 1
#define MAX_ALLOCATOR_VERTEX_BUFFER_VERTEX_COUNT 50000

// TODO: names
#define UNKNOWN_MEMORY_3_ELEMENT_COUNT 3200
#define UNKNOWN_MEMORY_4_ELEMENT_COUNT 80
#define UNKNOWN_MEMORY_5_ELEMENT_COUNT 800
#define UNKNOWN_MEMORY_6_ELEMENT_COUNT 128
#define UNKNOWN_MEMORY_7_ELEMENT_COUNT 5000

namespace Renderer::Allocator
{
    void AllocateVertexBuffer2(void);
    void ReleaseVertexeBuffer(void);
    void ReleaseAllocatedRendererMemory2(const void* ptr);
    void* AllocateRendererMemory1(const s32 count);
    void AllocateRendererMemory2(void);
    void* ReAllocateRendererMemory(void* ptr, const s32 count);
    void ReleaseAllocatedRendererMemory3(void);
    void AllocateRendererMemory3(void);
    void AllocateRendererMemory4(void);
}