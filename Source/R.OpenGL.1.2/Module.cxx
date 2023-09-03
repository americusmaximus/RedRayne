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

using namespace Renderers::Modules;

namespace Renderer::Module
{
    RendererModuleState State;

    extern "C" u32 AcquireGraphicsCardList(u32 * count, char** apis, char** names, u32*, u32*)
    {
        *count = 1;
        *apis = const_cast<char*>(RENDERER_MODULE_TITLE_NAME);
        *names = const_cast<char*>("OpenGL Compatible Graphics Card");

        return TRUE;
    }

    extern "C" u32 AcquireModuleName(char* name)
    {
        if (name == NULL) { return FALSE; }

        strcpy(name, RENDERER_MODULE_TITLE_NAME);

        return TRUE;
    }

    extern "C" u32 Init(const HWND hwnd, const RendererModuleInitializeArguments * args)
    {
        State.Window.HWND = hwnd;

        const auto hdc = GetDC(NULL);

        if (hdc == NULL) { return FALSE; }

        State.Window.Width = GetDeviceCaps(hdc, HORZRES);
        State.Window.Height = GetDeviceCaps(hdc, VERTRES);
        State.Window.BitsPerPixel = GetDeviceCaps(hdc, BITSPIXEL);

        return TRUE;
    }

    extern "C" u32 HasShaderSupport(void) { return FALSE; }

    extern "C" u32 Kill(void)
    {
        GL::Release();

        return TRUE;
    }

    extern "C" u32 SelectGraphicsCard(const u32) { return TRUE; }
}