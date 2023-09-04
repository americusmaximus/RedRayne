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

#pragma once

#include "DX.hxx"
#include "Settings.hxx"
#include "Renderers.Basic.hxx"
#include "Renderers.Modules.Export.hxx"

#ifdef _WIN64
#define RENDERER_MODULE_TITLE_NAME "DirectX 9.0 T&L (x64)"
#else
#define RENDERER_MODULE_TITLE_NAME "DirectX 9.0 T&L (x32)"
#endif

namespace Renderer::Module
{
    struct RendererModuleState
    {
        Renderers::Modules::RendererModuleInitializeArguments InitializeArguments;

        DX::DXState DX;

        struct
        {
            Settings::FixedPipelineOptionsMode FixedPipelineMode;
            Settings::SyncRetraceOptionsMode SyncRetraceMode = Settings::SyncRetraceOptionsMode::On;

            u32 TextureCount;
            u32 TextureSize = DEFAULT_REGULAR_TEXTURE_SIZE;

            Settings::BufferOptionsMode BufferMode;
        } Settings;

        struct
        {
            HWND HWND;

            Renderers::RendererWindowMode Mode;
        } Window;
    };

    extern RendererModuleState State;

    u32 DotLineListD3DTL(const Renderers::RTLVX*, const u32, const Renderers::RendererMode, const D3DPRIMITIVETYPE, const s32);
}