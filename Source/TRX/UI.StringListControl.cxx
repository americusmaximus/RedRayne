#include "UI.hxx"
#include "Logger.hxx"
#include "Native.Basic.hxx"

#include <stdlib.h>

#define STRING_LIST_CONTROL_ALLOCATION_INCREMENT 16

using namespace Logger;
using namespace Memory;

namespace UI
{
    StringListControlContainer StringListControlState;

    s32 StringComparator(void const* a, void const* b)
    {
        return strcmpi((char*)a, (char*)b);
    }

    StringListControl* StringListControlInitialize(StringListControl* self)
    {
        self->Size = 0;
        self->Count = 0;

        self->Items = NULL;

        self->Self = &StringListControlState.StringListControlSelf;

        return self;
    }

    StringListControl* StringListControlAssign(StringListControl* self, StringListControl* other)
    {
        self->Size = 0;
        self->Count = 0;

        self->Items = NULL;

        if (self != other)
        {
            self->Self->Release(self);

            if (other->Count != 0)
            {
                StringListControlAllocate(self, other->Count);

                for (u32 x = 0; x < other->Count; x++)
                {
                    StringListControlAttach(self, StringListControlAcquire(other, x));
                }
            }
        }

        return self;
    }

    void StringListControlAllocate(StringListControl* self, const s32 size)
    {
        if (size < self->Count)
        {
            LogError("Unable to reduce the size of the collection.");
        }

        if (size < 1)
        {
            LogError("Unable to allocate element collection, invalid count %d.", size);
        }

        if (self->Size < size)
        {
            self->Size = size;

            self->Items = (char**)ReallocateMemory(self->Items, size * sizeof(char*));

            if (self->Items == NULL)
            {
                LogError("Unable to allocate memory to insert an element into the collection.");
            }
        }
    }

    void* StringListControlDisposeWrapper(void* self, const DisposeMode mode)
    {
        return StringListControlDispose((StringListControl*)self, mode);
    }

    void* StringListControlDispose(StringListControl* self, const DisposeMode mode)
    {
        if ((mode & DisposeMode::Collection) != DisposeMode::None)
        {
            ReleaseMemory3(ReleaseObject1(self, StringListControlState._Initializer));

            return self;
        }

        self->Self = (StringListControlSelf*)&StringListControlState.StringListControlSelf;

        StringListControlRelease(self);

        if ((mode & DisposeMode::Unknown2) != DisposeMode::None)
        {
            ReleaseMemory2(self);
        }

        return self;
    }

    void StringListControlSortWrapper(void* self, const s32 start, const s32 end)
    {
        StringListControlSort((StringListControl*)self, start, end);
    }

    void StringListControlSort(StringListControl* self, const s32 start, const s32 end)
    {
        if (start < end)
        {
            if (start < 0 || self->Count <= end)
            {
                LogError("Unable to sort elements in the collection, invalid index(es) %d and %d.", start, end);
            }

            qsort(self->Items[start], (end - start) + 1, sizeof(char*), StringComparator);
        }
    }

    void StringListControlInsertWrapper(void* self, const s32 indx, const char* text)
    {
        StringListControlInsert((StringListControl*)self, indx, text);
    }

    void StringListControlInsert(StringListControl* self, const s32 indx, const char* text)
    {
        if (indx < 0 || self->Count < indx)
        {
            LogError("Unable to insert an element into the collection, invalid index %d.", indx);
        }

        self->Count = self->Count + 1;

        if (self->Size < self->Count)
        {
            StringListControlAllocate(self, self->Size + STRING_LIST_CONTROL_ALLOCATION_INCREMENT);
        }

        CopyMemory(&self->Items[indx + 1], &self->Items[indx], (self->Count - indx - 1) * sizeof(char*));

        self->Items[indx] = (char*)AllocateMemory(strlen(text));

        if (self->Items[indx] == NULL)
        {
            LogError("Unable to allocate memory to insert an element into the collection.");
        }

        strcpy(self->Items[indx], text);
    }

    void StringListControlSwapWrapper(void* self, const s32 src, const s32 dst)
    {
        StringListControlSwap((StringListControl*)self, src, dst);
    }

    void StringListControlSwap(StringListControl* self, const s32 src, const s32 dst)
    {
        if (src < 0 || self->Count <= src || dst < 0 || self->Count <= dst)
        {
            LogError("Unable to swap elements in the collection, invalid index(es) %d and %d.", src, dst);
        }

        if (src == dst) { return; }

        auto item = self->Items[src];

        self->Items[src] = self->Items[dst];
        self->Items[dst] = item;
    }

    void StringListControlReleaseWrapper(void* self)
    {
        StringListControlRelease((StringListControl*)self);
    }

    void StringListControlRelease(StringListControl* self)
    {
        if (self->Items != NULL)
        {
            for (u32 x = 0; x < self->Count; x++)
            {
                auto ptr = self->Items[x];

                if (ptr != NULL)
                {
                    ReleaseMemory1(ptr);

                    self->Items[x] = NULL;
                }
            }

            ReleaseMemory1(self->Items);
        }

        self->Items = NULL;

        self->Size = 0;
        self->Count = 0;
    }

    void StringListControlRemoveWrapper(void* self, const s32 start, const s32 end)
    {
        StringListControlRemove((StringListControl*)self, start, end);
    }

    void StringListControlRemove(StringListControl* self, const s32 start, const s32 end)
    {
        if (start < 0 || self->Count <= end || end < start)
        {
            LogError("Unable to remove elements from the collection, invalid index(es) %d and %d.", start, end);
        }

        for (u32 x = start; start <= end; x++)
        {
            if (self->Items[x] != NULL)
            {
                ReleaseMemory1(self->Items[x]);
            }
        }

        self->Count = self->Count - (end - start + 1);

        CopyMemory(self->Items[start], self->Items[end + 1], (self->Count - start) * sizeof(char*));
    }

    char* StringListControlAcquire(StringListControl* self, const s32 indx)
    {
        return self->Items[indx];
    }

    void StringListControlAttach(StringListControl* self, const char* text)
    {
        self->Self->Insert(self, self->Count, text);
    }
}