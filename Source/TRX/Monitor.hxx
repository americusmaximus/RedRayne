#pragma once

#include "Basic.hxx"
#include "Native.Basic.hxx"

#define MAX_DISPLAY_OPTIONS_COUNT 32

namespace Monitor
{
    struct MonitorContainer
    {
        s32* _Width = (s32*)0x0061e9b0;// TODO DEFAULT VALUE!
        s32* _Height = (s32*)0x0061e9b4;// TODO DEFAULT VALUE!
        s32* _BitsPerPixel = (s32*)0x0061e9b8;// TODO DEFAULT VALUE!
    };

    extern MonitorContainer MonitorState;

    struct DisplayOptions
    {
        s32 Width;
        s32 Height;
        s32 BitsPerPixel;
    };

    struct DisplayContainer
    {
        u32 Count;
        DisplayOptions Options[MAX_DISPLAY_OPTIONS_COUNT];
    };

    extern DisplayContainer DisplayState;

    void AcquireDisplayOptions(const HWND hwnd);
    s32 DisplayOptionsComparator(void const*, void const*);
}