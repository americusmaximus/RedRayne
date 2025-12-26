#include "App.Windows.hxx"
#include "Monitor.hxx"
#include "Logger.hxx"
#include "Renderer.hxx"
#include "Memory.hxx"
#include "Renderer.Module.hxx"
#include "Input.hxx"

#include <mmsystem.h>
#include "Input.Controllers.hxx"
#include "Sounds.hxx"
using namespace Renderers;
using namespace Input::Controllers;
using namespace Sounds;

#define ALT_KEY_STATE_BIT 0x20000000

#define APP_WINDOW_BUFFER_ALIGNMENT_VALUE 16
#define APP_WINDOW_BUFFER_ALIGNMENT_MASK (~(APP_WINDOW_BUFFER_ALIGNMENT_VALUE-1))

#define WINDOW_PROCEDURE_SUCCESS 0

using namespace Memory;
using namespace Logger;
using namespace Monitor;
using namespace Renderer;
using namespace Renderer::Module;
using namespace Input;
using namespace App::Windows::Cache;

namespace App::Windows
{
    ApplicationWindowContainer AppWindowState; // TODO name: AppWindowState

    void CreateAppWindow1(void)
    {
        *MonitorState._Width = GetSystemMetrics(SM_CXSCREEN);
        *MonitorState._Height = GetSystemMetrics(SM_CYSCREEN);

        const InitializeApplicationWindowArguments args =
        {
            .StyleEx = WS_EX_APPWINDOW | WS_EX_TOPMOST,
            .Style = WS_POPUP | WS_MINIMIZEBOX | WS_SYSMENU,
            .Class = AppWindowState.ClassName,
            .Name = AppWindowState.WindowName,
            .X = 0,
            .Y = 0,
            .Width = *MonitorState._Width,
            .Height = *MonitorState._Height,
            .Menu = NULL,
            .Parent = NULL,
            .Instance = *AppWindowState._Instance,
            .Mode = RendererWindowMode::On
        };

        *AppWindowState._HWND = CreateAppWindow2(*AppWindowState._ApplicationWindowPointer, &args);

        if (*AppWindowState._HWND == NULL)
        {
            ExitProcess(EXIT_FAILURE);
        }

        BringWindowToTop(*AppWindowState._HWND);

        SHOWCURSOR(FALSE);
    }

    HWND CreateAppWindow2(ApplicationWindow* window, const InitializeApplicationWindowArguments* args)
    {
        window->HWND = CreateWindowExA(args->StyleEx, args->Class, args->Name, args->Style,
            args->X, args->Y, args->Width, args->Height, args->Parent, args->Menu, args->Instance, NULL);

        window->X = args->X;
        window->Y = args->Y;

        window->Memory.FrameBuffer = NULL;
        window->Memory.DepthBuffer = NULL;
        window->Memory.AlignedDepthBuffer = NULL;

        window->Mode = args->Mode;

        if (window->HWND != NULL)
        {
            {
                RECT rect;

                GetClientRect(window->HWND, &rect);

                SetWindowPos(window->HWND, HWND_NOTOPMOST, args->X, args->Y,
                    args->Width * 2 - rect.right, args->Height * 2 - rect.bottom,
                    SWP_SHOWWINDOW | SWP_FRAMECHANGED);
            }

            if (*RendererWindow._WindowMode != RendererWindowMode::Off)
            {
                window->WindowWidth = args->Width;
                window->WindowHeight = args->Height;
            }

            window->ScreenWidth = args->Width;
            window->ScreenHeight = args->Height;

            {
                RECT rect;

                GetWindowRect(window->HWND, &rect);

                window->UnkActualWidth = rect.right - rect.left;
                window->UnkActualHeight = rect.bottom - rect.top;
            }

            if (*RendererWindow._WindowMode != RendererWindowMode::Off && args->Mode != RendererWindowMode::Off)
            {
                if (((args->StyleEx + 2) & 4) != 0) // TODO
                {
                    if (RendererModule.CreateGameWindow != NULL)
                    {
                        window->WindowIndex = RendererModule.CreateGameWindow(window->ScreenWidth, window->ScreenHeight,
                            *MonitorState._BitsPerPixel, RendererWindow._Unk4, window->HWND);
                    }
                }
            }

            if (((args->StyleEx + 2) & 4) != 0) // TODO
            {
                window->WindowIndex = 0;
            }

            SelectAppWindowResolution1(window, window->ScreenWidth, window->ScreenHeight, *MonitorState._BitsPerPixel);
        }

        AppWindowCacheInitialize1(*AppWindowCacheState._WindowCachePointer, window);

        return window->HWND;
    }

    void ReleaseAppWindow1(void)
    {
        ReleaseAppWindow2(*AppWindowState._ApplicationWindowPointer);
        ReleaseRenderer();
    }

    void ReleaseAppWindow2(ApplicationWindow* window)
    {
        ReleaseAppWindowDeviceContext(window);
        ReleaseAppWindowMemory(window);
    }

    void ReleaseAppWindowMemory(ApplicationWindow* window)
    {
        window->Memory.FrameBuffer = NULL;

        if (window->Memory.DepthBuffer == NULL)
        {
            window->Memory.AlignedDepthBuffer = NULL;

            return;
        }

        ReleaseMemory1(window->Memory.DepthBuffer);

        window->Memory.DepthBuffer = NULL;
        window->Memory.AlignedDepthBuffer = NULL;
    }

    void ResizeAppWindow1(ApplicationWindow* window)
    {
        if (*RendererWindow._WindowMode == RendererWindowMode::Off) { return; }

        RECT wr;
        RECT cr;

        GetWindowRect(window->HWND, &wr);
        GetClientRect(window->HWND, &cr);

        window->X = wr.left;
        window->Y = wr.top;

        window->UnkActualWidth = wr.right - wr.left;
        window->UnkActualHeight = wr.bottom - wr.top;

        window->ScreenWidth = cr.right - cr.left;
        window->ScreenHeight = cr.bottom - cr.top;
    }

    void ResizeAppWindow2(ApplicationWindow* window)
    {
        if (*RendererWindow._WindowMode == RendererWindowMode::Off) { return; }

        const auto width = window->ScreenWidth;
        const auto height = window->ScreenHeight;

        RECT wr;
        RECT cr;

        GetWindowRect(window->HWND, &wr);
        GetClientRect(window->HWND, &cr);

        window->X = wr.left;
        window->Y = wr.top;

        window->ScreenWidth = cr.right - cr.left;
        window->ScreenHeight = cr.bottom - cr.top;

        window->UnkActualWidth = wr.right - wr.left;
        window->UnkActualHeight = wr.bottom - wr.top;

        if (width != (cr.right - cr.left) || height != (cr.bottom - cr.top))
        {
            SelectAppWindowResolution1(window, window->ScreenWidth, window->ScreenHeight, window->ScreenBitsPerPixel);
        }
    }

    typedef const void(CDECLAPI* APPWINDOWUNKNOWN103) (ApplicationWindow* window);//TODO
    APPWINDOWUNKNOWN103 Unknown103 = (APPWINDOWUNKNOWN103)0x005cb630;//TODO

    void SelectAppWindowResolution1(ApplicationWindow* window, const u32 width, const u32 height, const u32 bpp)
    {
        ReleaseAppWindowDeviceContext(window);
        ReleaseAppWindowMemory(window);

        *MonitorState._Width = width;
        *MonitorState._Height = height;
        *MonitorState._BitsPerPixel = bpp;

        if (!*RendererState.Options._IsUsingModule || RendererModule.SelectVideoMode == NULL)
        {
            *RendererWindow._WindowMode = RendererWindowMode::On;

            CreateAppWindowBitMap(window);

            InvalidateRect(window->HWND, NULL, TRUE);
        }
        else if (*RendererWindow._WindowMode == RendererWindowMode::Off)
        {
            window->Memory.FrameBuffer = AllocateMemory(width * height * (bpp >> 3));

            if (window->Memory.FrameBuffer == NULL)
            {
                LogError("Unable to allocate frame buffer memory.");
            }
        }
        else
        {
            CreateAppWindowBitMap(window);

            InvalidateRect(window->HWND, NULL, TRUE);
        }

        {
            window->Memory.DepthBuffer = AllocateMemory(width * height * 4 + APP_WINDOW_BUFFER_ALIGNMENT_VALUE * 4);

            if (window->Memory.DepthBuffer == NULL)
            {
                LogError("Unable to allocate depth buffer memory.");
            }

            window->Memory.AlignedDepthBuffer = (void*)(((addr)window->Memory.DepthBuffer + APP_WINDOW_BUFFER_ALIGNMENT_VALUE) & APP_WINDOW_BUFFER_ALIGNMENT_MASK);
        }

        Unknown103(window);// TODO

        if (*RendererState.Options._IsUsingModule && window->Mode != RendererWindowMode::Off)
        {
            SelectRendererVideoMode2(width, height, bpp, window->WindowIndex);
            ReUploadRendererTextures();
        }

        if (*RendererWindow._WindowMode == RendererWindowMode::Off)
        {
            SetWindowPos(window->HWND, HWND_TOPMOST, 0, 0, width, height, NULL);
        }
        else
        {
            if (width != window->ScreenWidth || height != window->ScreenHeight)
            {
                const auto sw = window->ScreenWidth;
                const auto sh = window->ScreenHeight;

                window->ScreenWidth = width;
                window->ScreenHeight = height;

                window->UnkActualWidth = width + (window->UnkActualWidth - sw);
                window->UnkActualHeight = height + (window->UnkActualHeight - sh);

                window->X = (GetSystemMetrics(SM_CXFULLSCREEN) - width) / 2;
                window->Y = (GetSystemMetrics(SM_CYFULLSCREEN) - height) / 2;
            }

            SetWindowPos(window->HWND, HWND_NOTOPMOST, window->X, window->Y, window->UnkActualWidth, window->UnkActualHeight, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
        }

        if (*RendererState._IsActive)
        {
            (*RendererModule._SetColorTable)(RendererState.Colors._RGB, RendererState.Colors16Bit._RGBA);

            SelectRendererColors();
        }
    }

    void ReleaseAppWindowDeviceContext(ApplicationWindow* app)
    {
        if (app->HDC == NULL) { return; }

        DeleteObject(SelectObject(app->HDC, app->HGDIOBJ));
        DeleteDC(app->HDC);

        app->HDC = NULL;
    }

    void CreateAppWindowBitMap(ApplicationWindow* window)
    {
        ReleaseAppWindowDeviceContext(window);

        if (window->HDC == NULL)
        {
            window->BITMAPINFO.Header.biPlanes = 1;
            window->BITMAPINFO.Header.biSize = sizeof(BITMAPINFOHEADER);

            const auto bpp = *MonitorState._BitsPerPixel;
            window->BITMAPINFO.Header.biBitCount = bpp;

            if (bpp != GRAPHICS_BITS_PER_PIXEL_16 && bpp != GRAPHICS_BITS_PER_PIXEL_32)
            {
                LogError("Invalid bits per pixel value %d.", bpp);
            }

            window->BITMAPINFO.Header.biCompression = BI_BITFIELDS;
            window->BITMAPINFO.Header.biSizeImage = 0;
            window->BITMAPINFO.Header.biClrUsed = 0;
            window->BITMAPINFO.Header.biClrImportant = 0;
            window->BITMAPINFO.Header.biWidth = *MonitorState._Width;

            // NOTE:
            // For uncompressed RGB bitmaps, if biHeight is positive,
            // the bitmap is a bottom-up DIB with the origin at the lower left corner.
            // If biHeight is negative, the bitmap is a top-down DIB with the origin at the upper left corner.
            window->BITMAPINFO.Header.biHeight = -(*MonitorState._Height); // TODO

            {
                const auto hdc = GetDC(window->HWND);
                window->HDC = CreateCompatibleDC(hdc);
                ReleaseDC(window->HWND, hdc);
            }
        }

        CreateAppWindowPalette1(window);

        if (window->HBITMAP != NULL)
        {
            DeleteObject(SelectObject(window->HDC, window->HGDIOBJ));
            window->HBITMAP = NULL;
        }

        window->HBITMAP = CreateDIBSection(window->HDC, (BITMAPINFO*)&window->BITMAPINFO, DIB_RGB_COLORS, &window->DIB, NULL, 0);

        if (window->HBITMAP == NULL)
        {
            LogError("Unable to create bitmap %d x %d x %d bpp.", *MonitorState._Width, *MonitorState._Height, *MonitorState._BitsPerPixel);
        }

        window->HGDIOBJ = SelectObject(window->HDC, window->HBITMAP);
        window->Memory.FrameBuffer = window->DIB;

        {
            const auto width = window->BITMAPINFO.Header.biWidth;
            const auto height = window->BITMAPINFO.Header.biHeight;
            const auto bpp = *MonitorState._BitsPerPixel;

            // Convert bits per pixel to bytes.
            const auto size = width * height * ((s32)(bpp >> 3));

            // NOTE: Accommodating for negative height, see note above.
            window->BITMAPINFO.Header.biSizeImage = -size;
        }

        Unknown103(window);// TODO
    }

    void CreateAppWindowPalette1(ApplicationWindow* window)
    {
        switch (*MonitorState._BitsPerPixel)
        {
        case GRAPHICS_BITS_PER_PIXEL_16:
        {
            const u32 r = 0xf800;
            const u32 g = 0x7e0;
            const u32 b = 0x1f;

            window->BITMAPINFO.Masks[0] = r; // TODO ENUM?
            window->BITMAPINFO.Masks[1] = g; // TODO ENUM?
            window->BITMAPINFO.Masks[2] = b; // TODO ENUM?

            CreateAppWindowPalette2(r, g, b);

            return;
        }
        case GRAPHICS_BITS_PER_PIXEL_32:
        {
            const u32 r = 0xff0000;
            const u32 g = 0xff00;
            const u32 b = 0xff;

            window->BITMAPINFO.Masks[0] = r; // TODO ENUM?
            window->BITMAPINFO.Masks[1] = g; // TODO ENUM?
            window->BITMAPINFO.Masks[2] = b; // TODO ENUM?

            CreateAppWindowPalette2(r, g, b);

            return;
        }
        }

        LogError("Invalid bits per pixel value of %d.", *MonitorState._BitsPerPixel);
    }

    void CreateAppWindowPalette2(const u32 redMask, const u32 greenMask, const u32 blueMask)
    {
        auto actualRedMask = redMask;
        auto actualGreenMask = greenMask;
        auto actualBlueMask = blueMask;

        {
            *RendererMonitor.Color.Shift._R = 0;

            while ((actualRedMask & 1) == 0)
            {
                *RendererMonitor.Color.Shift._R = *RendererMonitor.Color.Shift._R + 1;
                actualRedMask = actualRedMask >> 1;
            }

            *RendererFont.Color.Shift._R = (0xff / actualRedMask);

            if (*RendererFont.Color.Shift._R == 0)
            {
                *RendererState.Colors16Bit.Unknown._Unknown01 = 32;
            }
            else
            {
                *RendererState.Colors16Bit.Unknown._Unknown01 = 0;

                for (u32 x = *RendererFont.Color.Shift._R; 1 < x; x = x >> 1)
                {
                    *RendererState.Colors16Bit.Unknown._Unknown01 = *RendererState.Colors16Bit.Unknown._Unknown01 + 1;
                }
            }
        }

        {
            *RendererMonitor.Color.Shift._G = 0;

            while ((actualGreenMask & 1) == 0)
            {
                *RendererMonitor.Color.Shift._G = *RendererMonitor.Color.Shift._G + 1;
                actualGreenMask = actualGreenMask >> 1;
            }

            *RendererFont.Color.Shift._G = (0xff / actualGreenMask);

            if (*RendererFont.Color.Shift._G == 0)
            {
                *RendererState.Colors16Bit.Unknown._Unknown03 = 32;
            }
            else
            {
                *RendererState.Colors16Bit.Unknown._Unknown03 = 0;

                for (u32 x = *RendererFont.Color.Shift._G; 1 < x; x = x >> 1)
                {
                    RendererState.Colors16Bit.Unknown._Unknown03 = RendererState.Colors16Bit.Unknown._Unknown03 + 1;
                }
            }
        }

        {
            *RendererMonitor.Color.Shift._B = 0;

            while ((actualBlueMask & 1) == 0)
            {
                *RendererMonitor.Color.Shift._B = *RendererMonitor.Color.Shift._B + 1;
                actualBlueMask = actualBlueMask >> 1;
            }

            *RendererFont.Color.Shift._B = (0xff / actualBlueMask);

            if (*RendererFont.Color.Shift._B == 0)
            {
                *RendererState.Colors16Bit.Unknown._Unknown05 = 32;
            }
            else
            {
                *RendererState.Colors16Bit.Unknown._Unknown05 = 0;

                for (u32 x = *RendererFont.Color.Shift._B; 1 < x; x = x >> 1)
                {
                    RendererState.Colors16Bit.Unknown._Unknown05 = RendererState.Colors16Bit.Unknown._Unknown05 + 1;
                }
            }
        }

        SelectRendererColors();
    }

    void VideoPaintAppWindow2(void)
    {
        VideoPaintAppWindow1(*AppWindowState._ApplicationWindowPointer);
    }

    void VideoPaintAppWindow1(ApplicationWindow* window)
    {
        if (*RendererWindow._WindowMode == RendererWindowMode::Off) { return; }

        if (*MonitorState._BitsPerPixel != GRAPHICS_BITS_PER_PIXEL_16 && *MonitorState._BitsPerPixel != GRAPHICS_BITS_PER_PIXEL_32)
        {
            LogError("Invalid bits per pixel value of %d.", *MonitorState._BitsPerPixel);
        }

        PAINTSTRUCT ps;

        const auto hdc = BeginPaint(window->HWND, &ps);

        BitBlt(hdc, 0, 0, *MonitorState._Width, *MonitorState._Height, window->HDC, 0, 0, SRCCOPY);

        EndPaint(window->HWND, &ps);
    }

    LRESULT AppWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        AppWindowCacheHandle1(*AppWindowCacheState._WindowCachePointer, hwnd, msg, wp, lp);

        if (hwnd != *AppWindowState._HWND) { return DefWindowProcA(hwnd, msg, wp, lp); }

        switch (msg)
        {
        case WM_DESTROY:
        {
            PostQuitMessage(0);

            *AppState._IsQuit = TRUE;

            break;
        }
        case WM_PAINT:
        {
            VideoPaintAppWindow2();

            break;
        }
        case WM_CLOSE:
        {
            *AppState._IsQuit = TRUE;

            return WINDOW_PROCEDURE_SUCCESS;
        }
        case WM_ACTIVATEAPP:
        {
            AppWindowState.PreviousActiveState = AppWindowState.ActiveState;

            // Indicates whether the window is being activated or deactivated.
            // This parameter is TRUE if the window is being activated;
            // it is FALSE if the window is being deactivated.
            AppWindowState.ActiveState = wp;

            if (AppWindowState.PreviousActiveState != AppWindowState.ActiveState)
            {
                const auto thread = GetCurrentProcess();

                if (AppWindowState.ActiveState)
                {
                    SetThreadPriority(thread, THREAD_PRIORITY_ABOVE_NORMAL);

                    RestoreRendererVideoMode();

                    if (AppWindowState.PreviousActiveState != -1) // TODO constant
                    {
                        ReleaseInput();

                        *AppWindowState._NextActiveState = 1;
                    }
                }
                else
                {
                    SaveRendererVideoMode();

                    SetThreadPriority(thread, THREAD_PRIORITY_IDLE);

                    *AppWindowState._NextActiveState = AppWindowState.ActiveState;
                }
            }

            break;
        }
        case WM_SETCURSOR:
        {
            if (*RendererWindow._WindowMode == RendererWindowMode::Off)
            {
                SetCursor(NULL);

                return TRUE;
            }

            break;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            auto indx = HIWORD(lp) & *InputState._KeyInputMask;

            if (indx == 0x5d) { indx = 0x154; }// TODO

            *InputState._KeyUnk1 = 0;

            if (indx == 0x1c && *InputState._KeyUnk2 && (lp & ALT_KEY_STATE_BIT)) // TODO
            {
                SelectAppWindowMode2((InternalWindowMode)(*RendererWindow._WindowMode == RendererWindowMode::Off));

                return WINDOW_PROCEDURE_SUCCESS;
            }

            InputState._InputPressed[indx] = TRUE;

            if (indx == 0x3e || indx == 0x44 || indx == 0x38 || indx == 0x138) { return WINDOW_PROCEDURE_SUCCESS; } // TODO

            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            auto indx = HIWORD(lp) & *InputState._KeyInputMask;

            if (indx == 0x5d) { indx = 0x154; }// TODO

            else if (indx == 0x2a || indx == 0x36)//TODO
            {
                InputState._InputReleased[42] = TRUE;
                InputState._InputReleased[54] = TRUE;
            }

            *InputState._KeyUnk1 = FALSE;
            InputState._InputReleased[indx] = TRUE;

            break;
        }
        case WM_CHAR:
        {
            const auto code = wp & 0xff; // TODO constant

            if (code != 0)
            {
                HandleInputText(code);
            }

            break;
        }
        case WM_COMMAND:
        {
            *AppWindowState.MCI._Result = wp;

            if (LOWORD(wp) == 0x9c41) // TODO constant
            {
                *AppState._IsQuit = TRUE;

                return WINDOW_PROCEDURE_SUCCESS;
            }

            break;
        }
        case WM_SYSCOMMAND:
        {
            if (wp == SC_KEYMENU)
            {
                if (*RendererWindow._WindowMode == RendererWindowMode::Off) { return WINDOW_PROCEDURE_SUCCESS; }
            }
            else if (wp == SC_SCREENSAVE || wp == SC_MONITORPOWER) { return WINDOW_PROCEDURE_SUCCESS; }

            break;
        }
        case WM_MOUSEMOVE:
        {
            *InputState.Mouse._X = LOWORD(lp);
            *InputState.Mouse._Y = HIWORD(lp);

            *InputState._Unk1 = 0;

            return WINDOW_PROCEDURE_SUCCESS;
        }
        case WM_LBUTTONDOWN:
        {
            *InputState.Mouse._X = LOWORD(lp);
            *InputState.Mouse._Y = HIWORD(lp);

            *InputState._Unk1 = 0;
            *InputState._Unk2 = *InputState._Unk2 | 1; // TODO

            InputState._InputPressed[INPUT_KEY_LEFT_MOUSE_BUTTON] = TRUE;

            return WINDOW_PROCEDURE_SUCCESS;
        }
        case WM_LBUTTONUP:
        {
            *InputState.Mouse._X = LOWORD(lp);
            *InputState.Mouse._Y = HIWORD(lp);

            *InputState._Unk1 = 0;
            *InputState._Unk2 = *InputState._Unk2 & 0xfe; // TODO

            InputState._InputReleased[INPUT_KEY_LEFT_MOUSE_BUTTON] = TRUE;

            return WINDOW_PROCEDURE_SUCCESS;
        }
        case WM_RBUTTONDOWN:
        {
            *InputState.Mouse._X = LOWORD(lp);
            *InputState.Mouse._Y = HIWORD(lp);

            *InputState._Unk1 = 0;
            *InputState._Unk2 = *InputState._Unk2 | 2; // TODO

            InputState._InputPressed[INPUT_KEY_RIGHT_MOUSE_BUTTON] = TRUE;

            return WINDOW_PROCEDURE_SUCCESS;
        }
        case WM_RBUTTONUP:
        {
            *InputState.Mouse._X = LOWORD(lp);
            *InputState.Mouse._Y = HIWORD(lp);

            *InputState._Unk1 = 0;
            *InputState._Unk2 = *InputState._Unk2 & 0xfd; // TODO

            InputState._InputReleased[INPUT_KEY_RIGHT_MOUSE_BUTTON] = TRUE;

            return WINDOW_PROCEDURE_SUCCESS;
        }
        case WM_MBUTTONDOWN:
        {
            *InputState.Mouse._X = LOWORD(lp);
            *InputState.Mouse._Y = HIWORD(lp);

            *InputState._Unk1 = 0;
            *InputState._Unk2 = *InputState._Unk2 | 4; // TODO

            InputState._InputPressed[INPUT_KEY_MIDDLE_MOUSE_BUTTON] = TRUE;

            return WINDOW_PROCEDURE_SUCCESS;
        }
        case WM_MBUTTONUP:
        {
            *InputState.Mouse._X = LOWORD(lp);
            *InputState.Mouse._Y = HIWORD(lp);

            *InputState._Unk1 = 0;
            *InputState._Unk2 = *InputState._Unk2 & 0xfb; // TODO

            InputState._InputReleased[INPUT_KEY_MIDDLE_MOUSE_BUTTON] = TRUE;

            return WINDOW_PROCEDURE_SUCCESS;
        }
        case WM_XBUTTONDOWN:
        {
            *InputState.Mouse._X = LOWORD(lp);
            *InputState.Mouse._Y = HIWORD(lp);

            switch (HIWORD(wp))
            {
            case XBUTTON1:
            {
                *InputState._Unk1 = 0;
                *InputState._Unk2 = *InputState._Unk2 | 8;//TODO

                InputState._InputPressed[INPUT_KEY_X1_MOUSE_BUTTON] = TRUE;

                return WINDOW_PROCEDURE_SUCCESS;
            }
            case XBUTTON2:
            {
                *InputState._Unk1 = 0;
                *InputState._Unk2 = *InputState._Unk2 | 0x10;//TODO

                InputState._InputPressed[INPUT_KEY_X2_MOUSE_BUTTON] = TRUE;

                return WINDOW_PROCEDURE_SUCCESS;
            }
            }

            *InputState._Unk1 = 0;

            return WINDOW_PROCEDURE_SUCCESS;
        }
        case WM_MOUSEWHEEL:
        {
            *InputState.Mouse._X = LOWORD(lp);
            *InputState.Mouse._Y = HIWORD(lp);

            // The high-order word indicates the distance the wheel is rotated,
            // expressed in multiples or divisions of WHEEL_DELTA, which is 120.
            // A positive value indicates that the wheel was rotated forward, away from the user;
            // a negative value indicates that the wheel was rotated backward, toward the user.
            const s32 delta = HIWORD(wp);

            *InputState.Mouse._WheelDelta = *InputState.Mouse._WheelDelta + delta;

            if (0 < delta)
            {
                InputState._InputPressed[INPUT_KEY_FORWARD_MOUSE_WHEEL] = TRUE;
                InputState._InputReleased[INPUT_KEY_FORWARD_MOUSE_WHEEL] = TRUE;

                return WINDOW_PROCEDURE_SUCCESS;
            }

            if (delta == 0) { return WINDOW_PROCEDURE_SUCCESS; }

            InputState._InputPressed[INPUT_KEY_BACKWARD_MOUSE_WHEEL] = TRUE;
            InputState._InputReleased[INPUT_KEY_BACKWARD_MOUSE_WHEEL] = TRUE;

            return WINDOW_PROCEDURE_SUCCESS;
        }
        case WM_XBUTTONUP:
        {
            *InputState.Mouse._X = LOWORD(lp);
            *InputState.Mouse._Y = HIWORD(lp);

            switch (HIWORD(wp))
            {
            case XBUTTON1:
            {
                *InputState._Unk1 = 0;
                *InputState._Unk2 = *InputState._Unk2 | 0xf7;//TODO

                InputState._InputReleased[INPUT_KEY_X1_MOUSE_BUTTON] = TRUE;

                return WINDOW_PROCEDURE_SUCCESS;
            }
            case XBUTTON2:
            {
                *InputState._Unk1 = 0;
                *InputState._Unk2 = *InputState._Unk2 | 0xef;//TODO

                InputState._InputReleased[INPUT_KEY_X2_MOUSE_BUTTON] = TRUE;

                return WINDOW_PROCEDURE_SUCCESS;
            }
            }

            return WINDOW_PROCEDURE_SUCCESS;
        }
        case MM_MCINOTIFY:
        {
            *AppWindowState.MCI._IsActive = TRUE;
            *AppWindowState.MCI._Result = wp;
            *AppWindowState.MCI._Device = lp;

            break;
        }
        }

        return DefWindowProcA(hwnd, msg, wp, lp);
    }

    BOOL SelectAppWindowResolution2(const u32 width, const u32 height, const u32 bpp)
    {
        SelectAppWindowResolution3(*AppWindowState._ApplicationWindowPointer, width, height, bpp);
        SelectAppWindowResolution1(*AppWindowState._ApplicationWindowPointer, width, height, bpp);

        return TRUE;
    }

    void SelectAppWindowResolution3(ApplicationWindow* window, const u32 width, const u32 height, const u32 bpp)
    {
        window->WindowWidth = width;
        window->WindowHeight = height;
        window->WindowBitsPerPixel = bpp;
    }

    void SelectAppWindowMode2(const InternalWindowMode mode)// TODO
    {
        SelectAppWindowMode1(*AppWindowState._ApplicationWindowPointer, (InternalWindowMode)(mode != InternalWindowMode::Screen));

        SHOWCURSOR(FALSE);
    }

    void SelectAppWindowMode1(ApplicationWindow* window, const InternalWindowMode mode)// TODO
    {
        if (*RendererWindow._WindowMode != (RendererWindowMode)mode)
        {
            *RendererWindow._WindowMode = (RendererWindowMode)(mode != InternalWindowMode::Screen);

            if (*RendererWindow._WindowMode == RendererWindowMode::On)
            {
                SelectAppWindowResolution1(window, window->WindowWidth, window->WindowHeight, window->WindowBitsPerPixel);
            }
            else
            {
                SelectAppWindowResolution1(window, window->ScreenWidth, window->ScreenHeight, window->ScreenBitsPerPixel);
            }
        }
    }

    void HandleAppWindowMessages()
    {
        for (u32 x = 0; x < MAX_INPUT_ELEMENT_COUNT; x++)
        {
            if (InputState._InputReleased[x])
            {
                InputState._InputPressed[x] = FALSE;
            }

            InputState._InputReleased[x] = FALSE;
        }

        while (TRUE)
        {
            while (TRUE)
            {
                MSG msg;

                if (!PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) { break; }

                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }

            AcquireControllerState();
            UnknownSound102();

            if (AppWindowState.ActiveState != 0 || *AppState._IsQuit) { break; } // TODO constant

            Sleep(20); // TODO constant

            if (!*AppWindowState._ActiveMode) { return; }
        }
    }

    // TODO should this be part of the renderer?
    // TODO move to graphics
    void SwapWindow1(void)
    {
        if (*RendererState._IsActive)
        {
            ToggleRendererScreen(ToggleMode::Unknown0);
        }
        else
        {
            InvalidateRect(*AppWindowState._HWND, NULL, FALSE);
        }

        HandleAppWindowMessages();
    }

    // TODO should this be part of the renderer?
    // TODO move to graphics
    void SwapWindow2(void)
    {
        if (!*RendererState._IsActive)
        {
            SwapWindow1();
        }
        else
        {
            ToggleRendererScreen(ToggleMode::Unknown1);
            HandleAppWindowMessages();
        }

        *RendererState.ViewPort._UnknownValue1 = *RendererState.ViewPort._UnknownValue2;
        *RendererState.Counters._FrameCounter = *RendererState.Counters._FrameCounter + 1;
    }
}