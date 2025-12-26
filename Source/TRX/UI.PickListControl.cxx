#include "UI.hxx"
#include "Logger.hxx"

#include <string.h>

#define PICK_LIST_CONTROL_OK_BUTTON_TEXT "OK"
#define PICK_LIST_CONTROL_CANCEL_BUTTON_TEXT "Cancel"

#define PICK_LIST_CONTROL_INVALID_BUTTON_INDEX (-1)

using namespace Logger;
using namespace Memory;

namespace UI
{
    PickListControlContainer PickListControlState;

    PickListControl* PickListControlInitialize(PickListControl* self)
    {
        StringListControlInitialize((StringListControl*)self);

        ScrollControlInitialize(&self->Scroll);

        self->Scroll.Unk34 = 0;

        ButtonControlInitialize(&self->OKButton);
        ButtonControlInitialize(&self->CancelButton);

        self->Self = &PickListControlState.PickListControlSelf;

        self->SelectedButtonIndex = PICK_LIST_CONTROL_INVALID_BUTTON_INDEX;

        self->ItemStateCount = 0;
        self->ItemStates = NULL;

        self->ItemHotKeyCount = 0;
        self->ItemHotKeys = NULL;

        strcpy(self->OKText, PICK_LIST_CONTROL_OK_BUTTON_TEXT);
        strcpy(self->CancelText, PICK_LIST_CONTROL_CANCEL_BUTTON_TEXT);

        self->Unknown54 = -1;
        self->Unknown58 = 0;

        return self;
    }

    void* PickListControlDisposeWrapper(void* self, const Memory::DisposeMode mode)
    {
        return PickListControlDispose((PickListControl*)self, mode);
    }

    void* PickListControlDispose(PickListControl* self, const Memory::DisposeMode mode)
    {
        if ((mode & DisposeMode::Collection) != DisposeMode::None)
        {
            ReleaseMemory3(ReleaseObject1(self, PickListControlState._Initializer));

            return self;
        }

        self->Self = &PickListControlState.PickListControlSelf;

        PickListControlRelease(self);

        ButtonControlDispose(&self->OKButton, DisposeMode::None);
        ButtonControlDispose(&self->CancelButton, DisposeMode::None);

        ScrollControlDispose(&self->Scroll, DisposeMode::None);

        StringListControlDispose((StringListControl*)self, DisposeMode::Unknown1);

        if ((mode & DisposeMode::Unknown2) != DisposeMode::None)
        {
            ReleaseMemory2(self);
        }

        return self;
    }

    void PickListControlReleaseWrapper(void* self)
    {
        PickListControlRelease((PickListControl*)self);
    }

    void PickListControlRelease(PickListControl* self)
    {
        if (self->ItemStates != NULL)
        {
            ReleaseMemory1(self->ItemStates);

            self->ItemStates = NULL;
        }

        self->ItemStateCount = 0;

        if (self->ItemHotKeys != NULL)
        {
            ReleaseMemory1(self->ItemHotKeys);

            self->ItemHotKeys = NULL;
        }

        self->ItemHotKeyCount = 0;

        StringListControlRelease((StringListControl*)self);
    }

    void PickListControlRemoveWrapper(void* self, const s32 indx1, const s32 indx2)
    {
        PickListControlRemove((PickListControl*)self, indx1, indx2);
    }

    BOOL PickListControlAcquireState(PickListControl* self, const s32 indx)
    {
        if (indx < 0 || self->Count <= indx)
        {
            LogError("Unable to acquire collection element state, invalid index %d.", indx);
        }

        if (self->ItemStateCount <= indx) { return TRUE; }

        return self->ItemStates[indx];
    }

    void PickListControlSelectState(PickListControl* self, const s32 indx, const BOOL mode)
    {
        if (indx < 0 || self->Count <= indx)
        {
            LogError("Unable to select collection element state, invalid index %d.", indx);
        }

        if (self->ItemStateCount <= indx)
        {
            if (mode) { return; }

            self->ItemStates = (BOOL*)ReallocateMemory(self->ItemStates, self->Count * sizeof(BOOL));

            if (self->ItemStates == NULL)
            {
                LogError("Unable to allocate memory to update an element in the collection.");
            }

            while (self->ItemStateCount < self->Count)
            {
                self->ItemStates[self->ItemStateCount] = TRUE;
                self->ItemStateCount = self->ItemStateCount + 1;
            }
        }

        self->ItemStates[indx] = mode;
    }

    s32 PickListControlAcquireHotKey(PickListControl* self, const s32 indx)
    {
        if (indx < 0 || self->Count <= indx)
        {
            LogError("Unable to acquire collection element hot key, invalid index %d.", indx);
        }

        if (self->ItemHotKeyCount <= indx) { return PICK_LIST_CONTROL_INVALID_HOT_KEY; }

        return self->ItemHotKeys[indx];
    }

    void PickListControlSelectHotKey(PickListControl* self, const s32 indx, const s32 key)
    {
        if (indx < 0 || self->Count <= indx)
        {
            LogError("Unable to select collection element hot key, invalid index %d.", indx);
        }

        if (self->ItemHotKeyCount <= indx)
        {
            if (key == PICK_LIST_CONTROL_INVALID_HOT_KEY) { return; }

            self->ItemHotKeys = (s32*)ReallocateMemory(self->ItemHotKeys, self->Count * sizeof(s32));

            if (self->ItemHotKeys == NULL)
            {
                LogError("Unable to allocate memory to update an element in the collection.");
            }

            while (self->ItemHotKeyCount < self->Count)
            {
                self->ItemHotKeys[self->ItemHotKeyCount] = PICK_LIST_CONTROL_INVALID_HOT_KEY;
                self->ItemHotKeyCount = self->ItemHotKeyCount + 1;
            }
        }

        self->ItemHotKeys[indx] = key;
    }

    void PickListControlRemove(PickListControl* self, const s32 indx1, const s32 indx2)
    {
        auto indx = indx1;

        for (auto x = indx2 + 1; x < self->Count; x++)
        {
            PickListControlSelectState(self, indx, PickListControlAcquireState(self, x));
            PickListControlSelectHotKey(self, indx, PickListControlAcquireHotKey(self, x));

            indx = indx + 1;
        }

        StringListControlRemove((StringListControl*)self, indx1, indx2);
    }

    void PickListControlSwapWrapper(void* self, const s32 indx1, const s32 indx2)
    {
        PickListControlSwap((PickListControl*)self, indx1, indx2);
    }

    void PickListControlSwap(PickListControl* self, const s32 indx1, const s32 indx2)
    {
        StringListControlSwap((StringListControl*)self, indx1, indx2);

        {
            const auto v1 = PickListControlAcquireState(self, indx1);
            const auto v2 = PickListControlAcquireState(self, indx2);

            if (v1 != v2)
            {
                PickListControlSelectState(self, indx1, v2);
                PickListControlSelectState(self, indx2, v1);
            }
        }

        {
            const auto v1 = PickListControlAcquireHotKey(self, indx1);
            const auto v2 = PickListControlAcquireHotKey(self, indx2);

            if (v1 != v2)
            {
                PickListControlSelectHotKey(self, indx1, v2);
                PickListControlSelectHotKey(self, indx2, v1);
            }
        }
    }

    void PickListControlInsertWrapper(void* self, const s32 indx, const char* value)
    {
        PickListControlInsert((PickListControl*)self, indx, value);
    }

    void PickListControlInsert(PickListControl* self, const s32 indx, const char* value)
    {
        StringListControlInsert((StringListControl*)self, indx, value);

        for (auto x = self->Count + -2; indx < x; x = x - 1)
        {
            PickListControlSelectState(self, x, PickListControlAcquireState(self, x - 1));
            PickListControlSelectHotKey(self, x, PickListControlAcquireHotKey(self, x - 1));
        }

        PickListControlSelectState(self, indx, TRUE);
        PickListControlSelectHotKey(self, indx, PICK_LIST_CONTROL_INVALID_HOT_KEY);
    }

    void PickListControlSortWrapper(void* self, const s32 indx1, const s32 indx2)
    {
        PickListControlSort((PickListControl*)self, indx1, indx2);
    }

    void PickListControlSort(PickListControl* self, const s32 indx1, const s32 indx2)
    {
        if (self->ItemStateCount <= indx1 && self->ItemHotKeyCount <= indx1)
        {
            StringListControlSort((StringListControl*)self, indx1, indx2);

            return;
        }

        for (auto x = indx2; indx1 < x; x--)
        {
            for (auto xx = indx1; xx < x; xx++)
            {
                if (0 < strcmpi(self->Items[xx], self->Items[xx + 1]))
                {
                    self->Self->Swap(self, xx, xx + 1);
                }
            }
        }
    }
}