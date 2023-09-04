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

#include "DX.hxx"
#include "Module.hxx"
#include "Quit.hxx"

#define MIN_SUPPORTED_SHADER_VERSION 0x100
#define SHADER_VERSION_MASK 0xff00

using namespace Renderers::Modules;
using namespace Renderers;
using namespace Settings;

namespace Renderer::Module
{
    extern "C" u32 AcquireGameWindowMode(void) { return TRUE; } // NOTE: Not being called by the application.

    extern "C" u32 __cdecl CreateGameWindow(const u32 width, const u32 height, const u32 bpp, u32 * *pixels, const HWND hwnd)
    {
        if (State.Window.Mode == RendererWindowMode::Off) { return INVALID_WINDOW_INDEX; }

        for (u32 x = MIN_WINDOW_INDEX; x < MAX_WINDOW_COUNT; x++)
        {
            if (State.DX.Windows.Windows[x] != NULL) { continue; }

            auto dxw = new DX::DXWindow();

            if (dxw == NULL) { return INVALID_WINDOW_INDEX; }

            DX::Initialize(dxw);

            dxw->HWND = hwnd;

            dxw->Width = width;
            dxw->Height = height;
            dxw->BitsPerPixel = bpp;

            dxw->Pixels.Pixels = pixels;

            State.DX.Windows.Windows[x] = dxw;

            DX::Initialize(dxw, &State.DX.Devices.Current.SwapChainParams);

            DX::DXC(State.DX.DirectX.Device->CreateAdditionalSwapChain(&State.DX.Devices.Current.SwapChainParams,
                &dxw->SwapChain), "Unable to create additional swap chain.");

            DX::DXC(State.DX.DirectX.Device->CreateDepthStencilSurface(width, height,
                State.DX.Devices.Current.SwapChainParams.AutoDepthStencilFormat, D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_NONE,
                &dxw->DepthSurface), "Unable to create additional depth buffer.");

            return x;
        }

        return INVALID_WINDOW_INDEX;
    }

    extern "C" u32 DestroyGameWindow(const u32) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SelectGameWindow(const u32, u32**) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SelectGameWindowMode(const RendererWindowMode mode)
    {
        State.Window.Mode = mode;

        return TRUE;
    }

    extern "C" u32 RestoreVideoMode(void)
    {
        DX::Release();

        if (State.DX.DirectX.Device != NULL)
        {
            State.DX.DirectX.Device->Release();
            State.DX.DirectX.Device = NULL;
        }

        if (State.DX.DirectX.Instance != NULL)
        {
            State.DX.DirectX.Instance->Release();
            State.DX.DirectX.Instance = NULL;
        }

        if (State.Window.Mode == RendererWindowMode::Off)
        {
            SHOWCURSOR(TRUE);
        }

        return TRUE;
    }

    extern "C" u32 SelectVideoMode(const u32 width, const u32 height, const u32 bpp, u32 * *pixels, const u32 indx)
    {
        auto dxw = State.DX.Windows.Windows[indx];

        if (dxw == NULL) { Quit("Invalid window index %d.", indx); }

        if (State.DX.DirectX.Instance != NULL)
        {
            if (indx == INVALID_WINDOW_INDEX)
            {
                return DX::ResetDevice(width, height, bpp, pixels, dxw);
            }

            return DX::Initialize(width, height, indx);
        }

        dxw->Width = width;
        dxw->Height = height;
        dxw->BitsPerPixel = bpp;

        State.DX.Mode.Mode = RendererMode::None;

        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_USE_FIXED_FUNCTION_PIPELINE_NAME, (u32*)&State.Settings.FixedPipelineMode);
        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_SYNC_RETRACE_NAME, (u32*)&State.Settings.SyncRetraceMode);
        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_RENDER_TEXTURE_COUNT_NAME, &State.Settings.TextureCount);
        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_RENDER_TEXTURE_SIZE_NAME, &State.Settings.TextureSize);
        Settings::ReadValue(RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME,
            RENDERER_MODULE_OPTIONS_GRAPHICS_USE_W_BUFFER_NAME, (u32*)&State.Settings.BufferMode);

        if (State.Settings.BufferMode == BufferOptionsMode::On) { State.DX.Mode.DepthBufferMode = D3DZBUFFERTYPE::D3DZB_USEW; }

        dxw->Pixels.Pixels = pixels;

        for (u32 x = 0; x < height; x++)
        {
            dxw->Pixels.Values[x] = pixels[x];
        }

        DX::Release();

        State.DX.DirectX.Instance = Direct3DCreate8(D3D_SDK_VERSION);

        if (State.DX.DirectX.Instance == NULL) { Quit("Unable to initialize DirectX."); }

        State.DX.DirectX.Instance->GetDeviceCaps(State.DX.Devices.Current.Index,
            D3DDEVTYPE::D3DDEVTYPE_HAL, &State.DX.Devices.Current.Capabilities);

        DX::Initialize(dxw, &State.DX.Devices.Current.PresentParams);

        const auto vsv = State.DX.Devices.Current.Capabilities.VertexShaderVersion & SHADER_VERSION_MASK;
        const auto psv = State.DX.Devices.Current.Capabilities.PixelShaderVersion & SHADER_VERSION_MASK;

        if (vsv < MIN_SUPPORTED_SHADER_VERSION || psv < MIN_SUPPORTED_SHADER_VERSION)
        {
            State.Settings.FixedPipelineMode = FixedPipelineOptionsMode::On;

            State.DX.Devices.Current.Capabilities.VertexShaderVersion = 0;
            State.DX.Devices.Current.Capabilities.PixelShaderVersion = 0;
        }

        if (State.DX.DirectX.Instance->CreateDevice(State.DX.Devices.Current.Index, D3DDEVTYPE::D3DDEVTYPE_HAL,
            State.Window.HWND, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
            &State.DX.Devices.Current.PresentParams, &State.DX.DirectX.Device) != D3D_OK)
        {
            State.DX.Devices.Current.PresentParams.BackBufferCount = 1;

            if (State.DX.DirectX.Instance->CreateDevice(State.DX.Devices.Current.Index, D3DDEVTYPE::D3DDEVTYPE_HAL,
                State.Window.HWND, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
                &State.DX.Devices.Current.PresentParams, &State.DX.DirectX.Device) != D3D_OK)
            {
                Quit("Unable to create DirectX device.");
            }
        }

        DX::Initialize();

        if (State.Window.Mode == RendererWindowMode::Off)
        {
            SetCursorPos(dxw->Width - 1, dxw->Height - 1);

            SHOWCURSOR(FALSE);
        }

        Clear(RendererClearMode::Target | RendererClearMode::Depth, GRAPHICS_COLOR_BLACK);

        Toggle();

        return TRUE;
    }

    extern "C" u32 VideoRestore(void)
    {
        DX::DXC(State.DX.DirectX.Device->Reset(&State.DX.Devices.Current.PresentParams), "Unable to reset device.");

        DX::Initialize();

        return TRUE;
    }

    extern "C" u32 VideoSave(void)
    {
        DX::Release();

        return TRUE;
    }
}