/*
Copyright (c) 2023 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Module.hxx"
#include "Quit.hxx"

using namespace Renderers::Modules;
using namespace Renderers;

namespace Renderer::Module
{
    u32 ChangeWindowMode()
    {
        DEVMODEA dm =
        {
            .dmSize = sizeof(DEVMODEA),
            .dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL,
            .dmBitsPerPel = State.Window.BitsPerPixel,
            .dmPelsWidth = State.Window.Width,
            .dmPelsHeight = State.Window.Height,
        };

        if (State.Window.Mode == RendererWindowMode::Off)
        {
            if (ChangeDisplaySettingsA(&dm, CDS_FULLSCREEN)) { Quit("Unable to change display settings."); }
        }

        GL::Release();

        State.Window.DeviceContext = GetDC(State.Window.HWND);

        if (State.Window.DeviceContext == NULL) { Quit("Unable to acquire device context."); }

        PIXELFORMATDESCRIPTOR pfd =
        {
            .nSize = sizeof(PIXELFORMATDESCRIPTOR),
            .nVersion = 1,
            .dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
            .iPixelType = PFD_TYPE_RGBA,
            .cColorBits = GRAPHICS_BITS_PER_PIXEL_32,
            .cDepthBits = GRAPHICS_BITS_PER_PIXEL_32
        };

        State.Window.PixelFormat = ChoosePixelFormat(State.Window.DeviceContext, &pfd);

        DescribePixelFormat(State.Window.DeviceContext, State.Window.PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

        if (pfd.dwFlags & PFD_GENERIC_FORMAT) { Quit("OpenGL acceleration is not supported."); }

        if (!SetPixelFormat(State.Window.DeviceContext, State.Window.PixelFormat, &pfd)) { Quit("Unable to set pixel format."); }

        State.Window.RenderContext = wglCreateContext(State.Window.DeviceContext);

        if (State.Window.RenderContext == NULL) { Quit("Unable to create rendering context."); }

        wglMakeCurrent(State.Window.DeviceContext, State.Window.RenderContext);

        if (State.Window.Mode == RendererWindowMode::Off)
        {
            SHOWCURSOR(FALSE);
        }

        GL::Initialize();

        Clear(RendererClearMode::None, GRAPHICS_COLOR_BLACK);
        ClearZBuffer();
        Toggle();

        return TRUE;
    }

    extern "C" u32 __cdecl CreateGameWindow(const u32, const u32, const u32, u32**, const HWND) { return 0; }

    extern "C" u32 DestroyGameWindow(const u32) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SelectGameWindow(const u32, u32**) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SelectGameWindowMode(const RendererWindowMode mode)
    {
        State.Window.Mode = mode;

        return ChangeWindowMode();
    }

    extern "C" u32 AcquireGameWindowMode(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 SelectVideoMode(const u32 width, const u32 height, const u32 bpp, u32**, const u32)
    {
        State.Window.Width = width;
        State.Window.Height = height;
        State.Window.BitsPerPixel = bpp;

        return ChangeWindowMode();
    }

    extern "C" u32 RestoreVideoMode(void)
    {
        GL::Release();

        if (State.Window.Mode == RendererWindowMode::Off)
        {
            if (ChangeDisplaySettingsA(NULL, 0)) { Quit("Unable to change display settings."); }
        }

        SHOWCURSOR(TRUE);

        return TRUE;
    }

    extern "C" u32 VideoRestore(void)
    {
        if (State.Window.Mode == RendererWindowMode::On) { return TRUE; }

        DEVMODEA dm =
        {
            .dmSize = sizeof(DEVMODEA),
            .dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL,
            .dmBitsPerPel = State.Window.BitsPerPixel,
            .dmPelsWidth = State.Window.Width,
            .dmPelsHeight = State.Window.Height,
        };

        if (ChangeDisplaySettingsA(&dm, CDS_FULLSCREEN)) { Quit("Unable to change display settings."); }

        return TRUE;
    }

    extern "C" u32 VideoSave(void)
    {
        ResetTextureCache();

        if (State.Window.Mode == RendererWindowMode::On) { return TRUE; }

        ShowWindow(State.Window.HWND, SW_MINIMIZE);

        if (ChangeDisplaySettingsA(NULL, 0)) { Quit("Unable to change display settings."); }

        return TRUE;
    }
}