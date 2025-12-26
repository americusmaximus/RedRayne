#include "Objects.hxx"

using namespace Memory;

// TODO
// Here I see same usage pattern as in renderer allocator
// with -4 == ALLOCATOR_MEMORY_BUFFER_COUNT_OFFSET offsets

// NOTE:
// I think this is constructor/destructor handlers for individual objects and arrays...

namespace Objects
{
    void* InitializeObject(void* self, const u32 count, const AbstractObjectInitializer* initializer)
    {
        auto address = (addr)self;

        for (u32 x = 0; x < count; x++)
        {
            initializer->Initialize((void*)address);

            address = address + initializer->Size;
        }

        return self;
    }

    void* ReleaseObject1(void* self, const AbstractObjectInitializer* initializer)
    {
        if (self == NULL) { return NULL; }

        const auto count = *(u32*)(((addr)self) -4); // TODO -4

        ReleaseObject2(self, count, initializer);

        return (void*)(((addr)self) - 4); // TODO -4
    }

    void* ReleaseObject2(void* self, const u32 count, const AbstractObjectInitializer* initializer)
    {
        for (u32 x = 0; x < count; x++)
        {
            auto ptr = (void*)(((addr)self) + x * initializer->Size);

            initializer->Dispose(ptr, DisposeMode::None);
        }

        return self;
    }
}