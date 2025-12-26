#pragma once

#include "Basic.hxx"
#include "Memory.hxx"
#include "Objects.hxx"

#define PICK_LIST_CONTROL_CANCEL_BUTTON (-1)
#define PICK_LIST_CONTROL_INVALID_HOT_KEY (-1)

namespace UI
{
    typedef void* (CDECLAPI* STRINGLISTCONTROLDISPOSE) (void* self, Memory::DisposeMode mode);
    typedef const void (CDECLAPI* STRINGLISTCONTROLREMOVE) (void* self, const s32 indx1, const s32 indx2);
    typedef const void (CDECLAPI* STRINGLISTCONTROLSORT) (void* self, const s32 indx1, const s32 indx2);
    typedef const void (CDECLAPI* STRINGLISTCONTROLINSERT) (void* self, const s32 indx, const char* value);
    typedef const void (CDECLAPI* STRINGLISTCONTROLSWAP) (void* self, const s32 indx1, const s32 indx2);
    typedef const void (CDECLAPI* STRINGLISTCONTROLRELEASE) (void* self);

    void* StringListControlDisposeWrapper(void* self, const Memory::DisposeMode mode);
    void StringListControlRemoveWrapper(void* self, const s32 start, const s32 end);
    void StringListControlSortWrapper(void* self, const s32 start, const s32 end);
    void StringListControlInsertWrapper(void* self, const s32 indx, const char* text);
    void StringListControlSwapWrapper(void* self, const s32 src, const s32 dsc);
    void StringListControlReleaseWrapper(void* self);

    struct StringListControlSelf
    {
        STRINGLISTCONTROLDISPOSE Dispose = (STRINGLISTCONTROLDISPOSE)&StringListControlDisposeWrapper;// 0x00453c20;//TODO
        STRINGLISTCONTROLREMOVE Remove = (STRINGLISTCONTROLREMOVE)&StringListControlRemoveWrapper;// 0x00454050;//TODO
        STRINGLISTCONTROLSORT Sort = (STRINGLISTCONTROLSORT)&StringListControlSortWrapper;// 0x00454150;//TODO
        STRINGLISTCONTROLINSERT Insert = (STRINGLISTCONTROLINSERT)&StringListControlInsertWrapper;// 0x00453e30;//TODO
        STRINGLISTCONTROLSWAP Swap = (STRINGLISTCONTROLSWAP)&StringListControlSwapWrapper;// 0x00454540;//TODO
        STRINGLISTCONTROLRELEASE Release = (STRINGLISTCONTROLRELEASE)&StringListControlReleaseWrapper;// 0x00453d00;//TODO
    };

    struct StringListControl
    {
        s32 Count;
        s32 Size;
        char** Items;
        StringListControlSelf* Self;
    };

    struct StringListControlContainer
    {
        StringListControlSelf StringListControlSelf;
        Objects::AbstractObjectInitializer* _Initializer = (Objects::AbstractObjectInitializer*)0x006109b0;//TODO
    };

    extern StringListControlContainer StringListControlState;

    StringListControl* StringListControlInitialize(StringListControl* self);
    StringListControl* StringListControlAssign(StringListControl* self, StringListControl* other);
    void StringListControlAllocate(StringListControl* self, const s32 size);

    void* StringListControlDispose(StringListControl* self, const Memory::DisposeMode mode);
    void StringListControlRemove(StringListControl* self, const s32 start, const s32 end);
    void StringListControlSort(StringListControl* self, const s32 start, const s32 end);
    void StringListControlInsert(StringListControl* self, const s32 indx, const char* text);
    void StringListControlSwap(StringListControl* self, const s32 src, const s32 dsc);
    void StringListControlRelease(StringListControl* self);

    char* StringListControlAcquire(StringListControl* self, const s32 indx);
    void StringListControlAttach(StringListControl* self, const char* text);



    struct ButtonControl
    {
        s32 Unk1;
        s32 Unk2;
        char Unk3;
        char Unk3a[3];
        s32 Unk7;
        
        s32 X;
        s32 Y;
        s32 Width;
        s32 Height;

        BOOL ActiveMode;

        char Unk8;

        char Unk100[199];
    };

    ButtonControl* ButtonControlInitialize(ButtonControl* self);

    void* ButtonControlDispose(ButtonControl* self, const Memory::DisposeMode mode);

    void ButtonControlRelease(ButtonControl* self);

    BOOL ButtonControlAcquireActiveMode(ButtonControl* self);

    struct ScrollControl
    {
        u32 Unk1;
        s32 Unk2;
        s32 Unk3;
        s32 Unk4;
        s32 Unk5;

        s32 X;
        s32 Y;
        s32 Width;
        s32 Height;

        s32 Unk10;
        s32 Unk11;
        s32 Unk12;
        s32 Unk13;

        s32 Unk34;
    };

    ScrollControl* ScrollControlInitialize(ScrollControl* self);

    void* ScrollControlDispose(ScrollControl* self, const Memory::DisposeMode mode);

    void ScrollControlRelease(ScrollControl* self);


    typedef void* (CDECLAPI* PICKLISTCONTROLDISPOSE) (void* self, Memory::DisposeMode mode);
    typedef const void (CDECLAPI* PICKLISTCONTROLREMOVE) (void* self, const s32 indx1, const s32 indx2);
    typedef const void (CDECLAPI* PICKLISTCONTROLSORT) (void* self, const s32 indx1, const s32 indx2);
    typedef const void (CDECLAPI* PICKLISTCONTROLINSERT) (void* self, const s32 indx, const char* value);
    typedef const void (CDECLAPI* PICKLISTCONTROLSWAP) (void* self, const s32 indx1, const s32 indx2);
    typedef const void (CDECLAPI* PICKLISTCONTROLRELEASE) (void* self);

    void* PickListControlDisposeWrapper(void* self, const Memory::DisposeMode mode);
    void PickListControlRemoveWrapper(void* self, const s32 indx1, const s32 indx2);
    void PickListControlSortWrapper(void* self, const s32 indx1, const s32 indx2);
    void PickListControlInsertWrapper(void* self, const s32 indx, const char* value);
    void PickListControlSwapWrapper(void* self, const s32 indx1, const s32 indx2);
    void PickListControlReleaseWrapper(void* self);

    struct PickListControlSelf
    {
        PICKLISTCONTROLDISPOSE Dispose = (PICKLISTCONTROLDISPOSE)&PickListControlDisposeWrapper;// 0x00455160;//TODO
        PICKLISTCONTROLREMOVE Remove = (PICKLISTCONTROLREMOVE)&PickListControlRemoveWrapper;// 0x00456e60;//TODO
        PICKLISTCONTROLSORT Sort = (PICKLISTCONTROLSORT)&PickListControlSortWrapper;// 0x00456f30;//TODO
        PICKLISTCONTROLINSERT Insert = (PICKLISTCONTROLINSERT)&PickListControlInsertWrapper;// 0x00456de0;//TODO
        PICKLISTCONTROLSWAP Swap = (PICKLISTCONTROLSWAP)&PickListControlSwapWrapper;// 0x00456fc0;//TODO
        PICKLISTCONTROLRELEASE Release = (PICKLISTCONTROLRELEASE)&PickListControlReleaseWrapper;// 0x00456ec0;//TODO
        void* Unk1 = (void*)0x00455430;//TODO
    };

    // NOTE: Inherited from StringListControl
    struct PickListControl
    {
        s32 Count;
        s32 Size;

        char** Items;

        PickListControlSelf* Self;

        char Unknown1001[68]; // TODO

        s32 Unknown54;
        s32 Unknown58;

        char OKText[100]; // TODO
        char CancelText[100]; // TODO

        char Unknown1002[136]; // TODO

        s32 ItemStateCount;
        BOOL* ItemStates;

        s32 ItemHotKeyCount;
        s32* ItemHotKeys;
        
        ScrollControl Scroll;

        ButtonControl OKButton;
        ButtonControl CancelButton;

        s32 SelectedButtonIndex;
    };

    struct PickListControlContainer
    {
        PickListControlSelf PickListControlSelf;
        Objects::AbstractObjectInitializer* _Initializer = (Objects::AbstractObjectInitializer*)0x00610a30;//TODO
    };

    extern PickListControlContainer PickListControlState;

    PickListControl* PickListControlInitialize(PickListControl* self);

    void* PickListControlDispose(PickListControl* self, const Memory::DisposeMode mode);
    void PickListControlRemove(PickListControl* self, const s32 indx1, const s32 indx2);
    void PickListControlSort(PickListControl* self, const s32 indx1, const s32 indx2);
    void PickListControlInsert(PickListControl* self, const s32 indx, const char* value);
    void PickListControlSwap(PickListControl* self, const s32 indx1, const s32 indx2);
    void PickListControlRelease(PickListControl* self);


    BOOL PickListControlAcquireState(PickListControl* self, const s32 indx);
    void PickListControlSelectState(PickListControl* self, const s32 indx, const BOOL mode);
    s32 PickListControlAcquireHotKey(PickListControl* self, const s32 indx);
    void PickListControlSelectHotKey(PickListControl* self, const s32 indx, const s32 key);








    struct UIContainer
    {
        struct
        {
            u32* _UnknownColor1 = (u32*)0x0067b02c;//TODO
            u32* _UnknownColor2 = (u32*)0x0067b030;//TODO
            u32* _UnknownColor3 = (u32*)0x0067b038;//TODO
            u32* _UnknownColor4 = (u32*)0x0067b028;//TODO
            u32* _UnknownColor5 = (u32*)0x0067b034;//TODO
            u32* _UnknownColor6 = (u32*)0x0067b03c;//TODO
            u32* _UnknownColor7 = (u32*)0x0067b040;//TODO
            u32* _UnknownColor8 = (u32*)0x0067b044;//TODO
            u32* _UnknownColor9 = (u32*)0x0067b04c;//TODO

            u32* _UnknownColor10 = (u32*)0x0067b050;//TODO
            u32* _UnknownColor11 = (u32*)0x0067b054;//TODO
            u32* _UnknownColor12 = (u32*)0x0067b058;//TODO
            u32* _UnknownColor13 = (u32*)0x0067b05c;//TODO
            u32* _UnknownColor14 = (u32*)0x0067b068;//TODO
            u32* _UnknownColor15 = (u32*)0x0067b06c;//TODO
            u32* _UnknownColor16 = (u32*)0x0067b070;//TODO
            u32* _UnknownColor17 = (u32*)0x0067b074;//TODO
            u32* _UnknownColor18 = (u32*)0x0067b078;//TODO
            u32* _UnknownColor19 = (u32*)0x0067b08c;//TODO

            u32* _UnknownColor20 = (u32*)0x0067b098;//TODO
            u32* _UnknownColor21 = (u32*)0x0067b09c;//TODO
            u32* _UnknownColor22 = (u32*)0x0067b048;//TODO
            u32* _UnknownColor23 = (u32*)0x0067b060;// TODO
            u32* _UnknownColor24 = (u32*)0x0067b084;// TODO
            u32* _UnknownColor25 = (u32*)0x0067b088;// TODO
            u32* _UnknownColor26 = (u32*)0x0067b090;// TODO
            u32* _UnknownColor27 = (u32*)0x0067b064;// TODO
            u32* _UnknownColor28 = (u32*)0x0067b07c;// TODO
            u32* _UnknownColor29 = (u32*)0x0067b080;// TODO

            u32* _ControlColor1 = (u32*)0x0067b0a0; // TODO
            u32* _ControlColor2 = (u32*)0x0067b0a4; // TODO
            u32* _ControlColor3 = (u32*)0x0067b094; // TODO
        } Colors;

        u32* _SelectedControl = (u32*)0x0067b0b0;//TODO name
    };

    extern UIContainer UIState;

    void InitializeColors1(void* self); // TODO
    void InitializeColors2(void* self); // TODO

    void RenderVerticalMargin(const s32 x, const s32 y, const s32 width);
    void RenderHorizontalMargin(const s32 x, const s32 y, const s32 height);
    void RenderControl(const s32 x, const s32 y, const s32 width, const s32 height, const BOOL mode);
}