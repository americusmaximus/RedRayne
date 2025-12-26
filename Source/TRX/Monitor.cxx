#include "Logger.hxx"
#include "Monitor.hxx"
#include "Renderers.Basic.hxx"

#include <stdlib.h>

using namespace Logger;

namespace Monitor
{
    MonitorContainer MonitorState;
    DisplayContainer DisplayState;

    s32 DisplayOptionsComparator(void const* l, void const* r)
    {
        const auto a = (DisplayOptions*)l;
        const auto b = (DisplayOptions*)r;

        const auto ahash = a->Width << 17 | a->Height << 5 | a->BitsPerPixel;
        const auto bhash = b->Width << 17 | b->Height << 5 | b->BitsPerPixel;

        return ahash - bhash;
    }

    void AcquireDisplayOptions(const HWND hwnd)
    {
        DisplayState.Count = 0;

        MONITORINFOEXA monitor =
        {
            { .cbSize = sizeof(MONITORINFOEXA) }
        };

        if (!GetMonitorInfoA(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &monitor))
        {
            LogError("Unable to acquire monitor information.");
        }

        DEVMODEA dev =
        {
            .dmSize = sizeof(DEVMODEA)
        };

        for (u32 x = 0; EnumDisplaySettingsA(monitor.szDevice, x, &dev) && x < MAX_DISPLAY_OPTIONS_COUNT; x++)
        {
            if (dev.dmPelsWidth < MIN_GRAPHICS_RESOLUTION_WIDTH || dev.dmPelsHeight < MIN_GRAPHICS_RESOLUTION_HEIGHT) { continue; }
            if (MAX_GRAPHICS_RESOLUTION_WIDTH < dev.dmPelsWidth || MAX_GRAPHICS_RESOLUTION_HEIGHT < dev.dmPelsHeight) { continue; }

            DisplayState.Options[x].Width = dev.dmPelsWidth;
            DisplayState.Options[x].Height = dev.dmPelsHeight;
            DisplayState.Options[x].BitsPerPixel = dev.dmBitsPerPel;

            DisplayState.Count = x + 1;
        }

        if (DisplayState.Count == 0)
        {
            const auto hdc = GetDC(hwnd);

            if (hdc == NULL) { LogError("Unable to acquire monitor information."); }

            DisplayState.Options[0].Width = GetDeviceCaps(hdc, HORZSIZE);
            DisplayState.Options[0].Height = GetDeviceCaps(hdc, VERTSIZE);
            DisplayState.Options[0].BitsPerPixel = GetDeviceCaps(hdc, BITSPIXEL);

            DisplayState.Count = 1;

            ReleaseDC(hwnd, hdc);
        }

        if (DisplayState.Count > 1)
        {
            qsort(&DisplayState.Options, DisplayState.Count, sizeof(DisplayOptions), DisplayOptionsComparator);
        }
    }
}