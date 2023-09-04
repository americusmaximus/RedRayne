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

#include "Assets.Basic.hxx"
#include "DX.hxx"
#include "Module.hxx"
#include "Quit.hxx"
#include "Settings.hxx"

#include <math.h>

#define MAX_TEXTURE_SURFACE_SIZE 1024

#define INVALID_DEVICE_MODE (-1)

using namespace Assets;
using namespace Mathematics;
using namespace Renderer::Module;
using namespace Renderers::Modules;
using namespace Renderers;
using namespace Settings;

namespace DX
{
    u32 Alpha(const f32 a)
    {
        return ((u32)roundf(255.0f * a) & 0xff) << 24;
    }

    u32 Color(const f32 r, const f32 g, const f32 b, const f32 a)
    {
        const auto ac = (u32)roundf(255.0f * a) & 0xff;
        const auto rc = (u32)roundf(255.0f * r) & 0xff;
        const auto gc = (u32)roundf(255.0f * g) & 0xff;
        const auto bc = (u32)roundf(255.0f * b) & 0xff;

        return (ac << 24) | (rc << 16) | (gc << 8) | (bc << 0);
    }

    void Release(void)
    {
        for (u32 x = 0; x < MAX_VERTEX_SHADER_COUNT; x++)
        {
            auto vs = &State.DX.Shaders.VertexShaders[x];

            if (vs->Handle != NULL)
            {
                State.DX.DirectX.Device->DeleteVertexShader(vs->Handle);
                vs->Handle = NULL;
            }
        }

        for (u32 x = 0; x < MAX_PIXEL_SHADER_COUNT; x++)
        {
            auto ps = &State.DX.Shaders.PixelShaders[x];

            if (ps->Handle != NULL)
            {
                State.DX.DirectX.Device->DeletePixelShader(ps->Handle);
                ps->Handle = NULL;
            }
        }

        if (State.DX.Surfaces.Opaque != NULL)
        {
            State.DX.Surfaces.Opaque->Release();
            State.DX.Surfaces.Opaque = NULL;
        }

        if (State.DX.Surfaces.Transparent != NULL)
        {
            State.DX.Surfaces.Transparent->Release();
            State.DX.Surfaces.Transparent = NULL;
        }

        for (u32 x = 0; x < State.Settings.TextureCount; x++)
        {
            if (State.DX.Textures.Surfaces.Textures[x] != NULL)
            {
                State.DX.Textures.Surfaces.Textures[x]->Release();
                State.DX.Textures.Surfaces.Textures[x] = NULL;
            }
        }

        if (State.DX.Surfaces.TextureDepthSurface != NULL)
        {
            State.DX.Surfaces.TextureDepthSurface->Release();
            State.DX.Surfaces.TextureDepthSurface = NULL;
        }

        for (u32 x = MIN_WINDOW_INDEX; x < MAX_WINDOW_COUNT; x++)
        {
            auto dxw = State.DX.Windows.Windows[x];

            if (dxw != NULL)
            {
                if (dxw->SwapChain != NULL)
                {
                    dxw->SwapChain->Release();
                    dxw->SwapChain = NULL;
                }

                if (dxw->DepthSurface != NULL)
                {
                    dxw->DepthSurface->Release();
                    dxw->DepthSurface = NULL;
                }
            }
        }

        ResetTextureCache();

        if (State.DX.Buffers.Main.Buffer != NULL)
        {
            State.DX.Buffers.Main.Buffer->Release();
            State.DX.Buffers.Main.Buffer = NULL;
        }

        if (State.DX.Buffers.RLVX.Buffer != NULL)
        {
            State.DX.Buffers.RLVX.Buffer->Release();
            State.DX.Buffers.RLVX.Buffer = NULL;
        }

        if (State.DX.Buffers.RTLVX.Buffer != NULL)
        {
            State.DX.Buffers.RTLVX.Buffer->Release();
            State.DX.Buffers.RTLVX.Buffer = NULL;
        }

        if (State.DX.Buffers.Basis.Buffer != NULL)
        {
            State.DX.Buffers.Basis.Buffer->Release();
            State.DX.Buffers.Basis.Buffer = NULL;
        }

        if (State.DX.Buffers.Index.Buffer != NULL)
        {
            State.DX.Buffers.Index.Buffer->Release();
            State.DX.Buffers.Index.Buffer = NULL;
        }

        ZeroMemory(State.DX.Shaders.Constants.Actual, MAX_SHADER_REGISTER_CONSTANT_COUNT * sizeof(f32x4));
    }

    BOOL InitializeState(void)
    {
        for (u32 x = 0; x < MAX_STATE_VARIABLE_COUNT; x++)
        {
            State.DX.DirectX.State.Values[x] = U32_MAX;
        }

        if (State.DX.DirectX.Device->BeginScene() != D3D_OK) { return FALSE; }

        SetRenderState(D3DRENDERSTATETYPE::D3DRS_FILLMODE, D3DFILLMODE::D3DFILL_SOLID);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_SHADEMODE, D3DSHADEMODE::D3DSHADE_GOURAUD);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_ALPHATESTENABLE, FALSE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_LASTPIXEL, FALSE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_SRCBLEND, D3DBLEND::D3DBLEND_ONE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_DESTBLEND, D3DBLEND::D3DBLEND_INVSRCALPHA);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, D3DCULL::D3DCULL_NONE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_DITHERENABLE, TRUE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_ALPHABLENDENABLE, FALSE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGENABLE, FALSE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGCOLOR, GRAPHICS_COLOR_BLACK);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_SPECULARENABLE, TRUE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZVISIBLE, FALSE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, D3DZBUFFERTYPE::D3DZB_FALSE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, FALSE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_ALPHAREF, 0);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_ALPHAFUNC, D3DCMPFUNC::D3DCMP_GREATER);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_ALPHATESTENABLE, FALSE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_CLIPPING, TRUE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_LIGHTING, FALSE);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_STENCILZFAIL, D3DSTENCILOP::D3DSTENCILOP_KEEP);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_STENCILFAIL, D3DSTENCILOP::D3DSTENCILOP_KEEP);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_STENCILREF, 1);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_STENCILMASK, U32_MAX);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_STENCILWRITEMASK, U32_MAX);

        for (u32 x = 0; x < MAX_SURFACE_STAGE_COUNT; x++)
        {
            State.DX.Surfaces.Staging.Textures[x] = NULL;
            State.DX.Textures.Surfaces.Staging[x] = NULL;
        }

        State.DX.Mode.Mode = RendererMode::None;

        return State.DX.DirectX.Device->EndScene() == D3D_OK;
    }

    void Initialize(void)
    {
        for (u32 x = 0; x < MAX_STATE_VARIABLE_COUNT; x++)
        {
            State.DX.DirectX.State.Values[x] = 0;
        }

        for (u32 x = 0; x < MAX_SURFACE_STAGE_COUNT; x++)
        {
            for (u32 xx = 0; xx < MAX_SURFACE_STAGE_VALUES_COUNT; xx++)
            {
                State.DX.Surfaces.Staging.Stages[x][xx] = 0;
            }
        }

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off)
        {
            for (u32 x = 0; x < MAX_VERTEX_SHADER_COUNT; x++)
            {
                auto shader = &State.DX.Shaders.VertexShaders[x];

                if (shader->Function != NULL)
                {
                    DXC(State.DX.DirectX.Device->CreateVertexShader(shader->Declaration,
                        shader->Function, &shader->Handle, D3DUSAGE_NONE), "Unable to create vertex shader.");
                }
            }

            for (u32 x = 0; x < MAX_PIXEL_SHADER_COUNT; x++)
            {
                auto shader = &State.DX.Shaders.PixelShaders[x];

                DXC(State.DX.DirectX.Device->CreatePixelShader(shader->Function, &shader->Handle),
                    "Unable to create pixel shader.");
            }
        }

        const D3DVIEWPORT8 viewport =
        {
            .X = 0,
            .Y = 0,
            .Width = State.DX.Windows.Current->Width,
            .Height = State.DX.Windows.Current->Height,
            .MinZ = 0.0f,
            .MaxZ = 1.0f
        };

        State.DX.DirectX.Device->SetViewport(&viewport);

        if (State.DX.Windows.Current->BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_16)
        {
            State.DX.Surfaces.Formats.Transparent = D3DFORMAT::D3DFMT_A4R4G4B4;
            State.DX.Surfaces.Formats.Surface = TextureFormat::Color16Bit4444;
        }
        else
        {
            State.DX.Surfaces.Formats.Transparent = D3DFORMAT::D3DFMT_A8R8G8B8;
            State.DX.Surfaces.Formats.Surface = TextureFormat::Color32Bit;
        }

        DXC(State.DX.DirectX.Device->GetDepthStencilSurface(&State.DX.Windows.Windows[0]->DepthSurface), "Unable to get primary depth buffer.");

        if (State.DX.Windows.Windows[0]->DepthSurface != NULL)
        {
            State.DX.Windows.Windows[0]->DepthSurface->Release();
            State.DX.Windows.Windows[0]->DepthSurface = NULL;
        }

        for (u32 x = MIN_WINDOW_INDEX; x < MAX_WINDOW_COUNT; x++)
        {
            const auto dxw = State.DX.Windows.Windows[x];

            if (dxw != NULL)
            {
                DX::Initialize(dxw->Width, dxw->Height, x);
            }
        }

        DXC(State.DX.DirectX.Device->CreateImageSurface(MAX_TEXTURE_SURFACE_SIZE, MAX_TEXTURE_SURFACE_SIZE,
            State.DX.Surfaces.Formats.Transparent, &State.DX.Surfaces.Transparent),
            "Unable to create alpha source buffer.");

        DXC(State.DX.DirectX.Device->CreateImageSurface(MAX_TEXTURE_SURFACE_SIZE, MAX_TEXTURE_SURFACE_SIZE,
            D3DFORMAT::D3DFMT_R5G6B5, &State.DX.Surfaces.Opaque),
            "Unable to create 565 source buffer.");

        {
            const auto size = State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On
                ? (DEFAULT_BUFFER_CAPACITY * sizeof(REVX))
                : (DEFAULT_BUFFER_CAPACITY * sizeof(RVX));

            DXC(State.DX.DirectX.Device->CreateVertexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On ? (D3DFVF_TEX2 | D3DFVF_NORMAL | D3DFVF_XYZ | D3DFVF_TEX0) : D3DFVF_TEX0,
                D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Buffers.Main.Buffer),
                "Unable to create main vertex buffer.");
        }

        DXC(State.DX.DirectX.Device->CreateVertexBuffer(DEFAULT_BUFFER_CAPACITY * sizeof(RLVX),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            D3DFVF_TEX0, D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Buffers.RLVX.Buffer),
            "Unable to create untransformed prelit vertex buffer.");

        DXC(State.DX.DirectX.Device->CreateVertexBuffer(RENDERER_TEN_THOUSAND_ELEMENT_COUNT * sizeof(RTLVX),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            D3DFVF_TEX1 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW | D3DFVF_TEX0, D3DPOOL::D3DPOOL_DEFAULT,
            &State.DX.Buffers.RTLVX.Buffer),
            "Unable to create transformed prelit vertex buffer.");

        {
            const auto size = State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On
                ? (DEFAULT_BUFFER_CAPACITY * sizeof(REVX))
                : (DEFAULT_BUFFER_CAPACITY * sizeof(RBVX));

            DXC(State.DX.DirectX.Device->CreateVertexBuffer(size,
                D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                D3DFVF_TEX0, D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Buffers.Basis.Buffer),
                "Unable to create vertex basis buffer.");
        }

        DXC(State.DX.DirectX.Device->CreateIndexBuffer(DEFAULT_BUFFER_CAPACITY * sizeof(u16) * GRAPHICS_TRIANGLE_VERTEX_COUNT,
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFORMAT::D3DFMT_INDEX16, D3DPOOL::D3DPOOL_SYSTEMMEM,
            &State.DX.Buffers.Index.Buffer),
            "Unable to create index buffer.");

        for (u32 x = 0; x < State.Settings.TextureCount; x++)
        {
            if (State.DX.DirectX.Device->CreateTexture(State.Settings.TextureSize, State.Settings.TextureSize,
                1, D3DUSAGE_RENDERTARGET, State.DX.Devices.Current.PresentParams.BackBufferFormat,
                D3DPOOL::D3DPOOL_DEFAULT, &State.DX.Textures.Surfaces.Textures[x]) != D3D_OK)
            {
                State.Settings.TextureCount = 0; break;
            }
        }

        if (State.Settings.TextureCount != 0)
        {
            DXC(State.DX.DirectX.Device->CreateDepthStencilSurface(State.Settings.TextureSize, State.Settings.TextureSize,
                State.DX.Devices.Current.PresentParams.AutoDepthStencilFormat, D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_NONE,
                &State.DX.Surfaces.TextureDepthSurface), "Unable to create render texture depth buffer.");
        }

        if (!InitializeState()) { Quit("Unable to set DirectX into a known state."); }
    }

    BOOL ResetDevice(const u32 width, const u32 height, const u32 bpp, u32** pixels, DX::DXWindow* dxw)
    {
        if (dxw == NULL) { return FALSE; }

        Release();

        dxw->Width = width;
        dxw->Height = height;
        dxw->BitsPerPixel = bpp;

        dxw->Pixels.Pixels = pixels;

        for (u32 x = 0; x < dxw->Height; x++)
        {
            dxw->Pixels.Values[x] = pixels[x];
        }

        DX::Initialize(dxw, &State.DX.Devices.Current.PresentParams);

        DXC(State.DX.DirectX.Device->Reset(&State.DX.Devices.Current.PresentParams), "Unable to reset device.");

        const auto cdxw = State.DX.Windows.Current;
        State.DX.Windows.Current = dxw;

        Initialize();

        State.DX.Windows.Current = cdxw;

        if (State.Window.Mode == RendererWindowMode::On)
        {
            SHOWCURSOR(TRUE);
        }

        Clear(RendererClearMode::Target | RendererClearMode::Depth, GRAPHICS_COLOR_BLACK);
        Toggle();

        return TRUE;
    }

    void SetRenderState(const D3DRENDERSTATETYPE state, const DWORD value)
    {
        if (State.DX.DirectX.State.Values[state] != value)
        {
            State.DX.DirectX.State.Values[state] = value;

            State.DX.DirectX.Device->SetRenderState(state, value);
        }
    }

    void SelectRendererMode(const RendererMode mode)
    {
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_SRCBLEND, State.DX.Mode.Blend.SourceBlendMode);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_DESTBLEND, State.DX.Mode.Blend.DestinationBlendMode);
        SetRenderState(D3DRENDERSTATETYPE::D3DRS_BLENDOP, State.DX.Mode.Blend.DestinationBlendOperation);

        if (((State.DX.Mode.Mode ^ mode) & RendererMode::U0x1) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x1) != RendererMode::None)
            {
                State.DX.Surfaces.Staging.Textures[0] = NULL;
            }
            else
            {
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
            }
        }

        u32 indexes[MAX_BLEND_TEXTURE_COUNT] = { 0, U32_MAX, U32_MAX, U32_MAX };

        switch (State.DX.Mode.Blend.BlendOperation)
        {
        case RendererBlendOperation::Disable:
        case RendererBlendOperation::BlendTextureAlpha:
        {
            indexes[0] = 1;
            indexes[1] = 0;
            break;
        }
        case RendererBlendOperation::SelectArgument1:
        case RendererBlendOperation::BlendFactorAlpha:
        case RendererBlendOperation::BlendTextureAlphaPreMultiplied:
        case RendererBlendOperation::BlendCurrentAlpha:
        {
            indexes[0] = 1;
            indexes[1] = 4;
            break;
        }
        case RendererBlendOperation::SelectArgument2:
        {
            indexes[0] = 2;
            break;
        }
        case RendererBlendOperation::Modulate:
        {
            indexes[0] = 1;
            indexes[3] = 2;
            break;
        }
        case RendererBlendOperation::Add:
        {
            indexes[0] = 0;
            indexes[1] = 1;
            indexes[2] = 2;
            break;
        }
        case RendererBlendOperation::AddSigned2X:
        {
            indexes[0] = 0;
            indexes[1] = 3;
            break;
        }
        case RendererBlendOperation::BlendDiffuseAlpha:
        {
            indexes[0] = 1;
            indexes[1] = 4;
            indexes[2] = 3;
            break;
        }
        case RendererBlendOperation::PreModulate:
        {
            indexes[0] = 1;
            indexes[1] = 4;
            indexes[2] = 0;
            indexes[3] = 3;
            break;
        }
        }

        IDirect3DTexture8* tex[MAX_BLEND_TEXTURE_COUNT] = { NULL, NULL, NULL, NULL };

        for (u32 x = 0; x < MAX_BLEND_TEXTURE_COUNT; x++)
        {
            if (indexes[x] != U32_MAX)
            {
                tex[x] = State.DX.Textures.Surfaces.Staging[indexes[x]];
            }
        }

        auto clampOperation = State.DX.Mode.Clamp.ClampOperation;

        if (tex[0] != State.DX.Surfaces.Staging.Textures[0]
            || tex[1] != State.DX.Surfaces.Staging.Textures[1]
            || tex[2] != State.DX.Surfaces.Staging.Textures[2]
            || tex[3] != State.DX.Surfaces.Staging.Textures[3]
            || State.DX.Mode.Blend.BlendOperation != State.DX.Mode.Clamp.ClampOperation
            || (((State.DX.Mode.Mode ^ mode) & RendererMode::U0x400) != RendererMode::None))
        {
            for (u32 x = 0; x < MAX_BLEND_TEXTURE_COUNT; x++)
            {
                State.DX.Surfaces.Staging.Textures[x] = tex[x];
                State.DX.DirectX.Device->SetTexture(x, State.DX.Surfaces.Staging.Textures[x]);

                if (State.DX.Surfaces.Staging.Textures[x] != NULL)
                {
                    SelectTextureStageValue(x, D3DTEXTURESTAGESTATETYPE::D3DTSS_MAGFILTER, D3DTEXTUREFILTERTYPE::D3DTEXF_LINEAR);
                    SelectTextureStageValue(x, D3DTEXTURESTAGESTATETYPE::D3DTSS_MINFILTER, D3DTEXTUREFILTERTYPE::D3DTEXF_LINEAR);
                    SelectTextureStageValue(x, D3DTEXTURESTAGESTATETYPE::D3DTSS_MIPFILTER, D3DTEXTUREFILTERTYPE::D3DTEXF_LINEAR);

                    const auto indx = indexes[x];

                    SelectTextureStageValue(x, D3DTEXTURESTAGESTATETYPE::D3DTSS_ADDRESSU,
                        State.DX.Mode.Clamp.U[indx] == RendererClampMode::Wrap
                        ? D3DTEXTUREADDRESS::D3DTADDRESS_WRAP : D3DTEXTUREADDRESS::D3DTADDRESS_CLAMP);
                    SelectTextureStageValue(x, D3DTEXTURESTAGESTATETYPE::D3DTSS_ADDRESSV,
                        State.DX.Mode.Clamp.V[indx] == RendererClampMode::Wrap
                        ? D3DTEXTUREADDRESS::D3DTADDRESS_WRAP : D3DTEXTUREADDRESS::D3DTADDRESS_CLAMP);

                    if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off)
                    {
                        SelectTextureStageValue(x, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXTURETRANSFORMFLAGS,
                            ((mode & RendererMode::U0x400) != RendererMode::None)
                            ? D3DTEXTURETRANSFORMFLAGS::D3DTTFF_PROJECTED : D3DTEXTURETRANSFORMFLAGS::D3DTTFF_DISABLE);
                    }
                }
            }

            if (State.DX.Mode.Blend.BlendOperation == RendererBlendOperation::None)
            {
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_MODULATE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_MODULATE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);
            }
            else if (State.DX.Mode.Blend.BlendOperation == RendererBlendOperation::AddSigned2X)
            {
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_MODULATE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_MODULATE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_MODULATE);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_MODULATE);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectTextureStageValue(TEXTURE_STAGE_1_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_2_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);

                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_COLOROP, D3DTEXTUREOP::D3DTOP_DISABLE);
                SelectTextureStageValue(TEXTURE_STAGE_3_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_ALPHAOP, D3DTEXTUREOP::D3DTOP_DISABLE);
            }

            clampOperation = State.DX.Mode.Blend.BlendOperation;

            if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
            {
                if ((mode & RendererMode::U0x400) != RendererMode::None)
                {
                    SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXTURETRANSFORMFLAGS,
                        D3DTEXTURETRANSFORMFLAGS::D3DTTFF_PROJECTED | D3DTEXTURETRANSFORMFLAGS::D3DTTFF_COUNT3);
                    SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
                }
                else
                {
                    if (State.DX.Mode.Blend.BlendOperation == RendererBlendOperation::SelectArgument2)
                    {
                        SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXTURETRANSFORMFLAGS, D3DTEXTURETRANSFORMFLAGS::D3DTTFF_COUNT3);
                        SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
                    }
                    else
                    {
                        SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXTURETRANSFORMFLAGS, D3DTEXTURETRANSFORMFLAGS::D3DTTFF_DISABLE);
                        SelectTextureStageValue(TEXTURE_STAGE_0_INDEX, D3DTEXTURESTAGESTATETYPE::D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
                    }
                }
            }
        }

        State.DX.Mode.Clamp.ClampOperation = clampOperation;

        if (((State.DX.Mode.Mode ^ mode) & RendererMode::U0x20) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x20) != RendererMode::None)
            {
                SetRenderState(D3DRENDERSTATETYPE::D3DRS_ALPHABLENDENABLE, TRUE);
                SetRenderState(D3DRENDERSTATETYPE::D3DRS_ALPHATESTENABLE, TRUE);
            }
            else
            {
                SetRenderState(D3DRENDERSTATETYPE::D3DRS_ALPHABLENDENABLE, FALSE);
                SetRenderState(D3DRENDERSTATETYPE::D3DRS_ALPHATESTENABLE, FALSE);
            }
        }

        if (((State.DX.Mode.Mode ^ mode) & RendererMode::U0x8) != RendererMode::None)
        {
            if ((mode & RendererMode::U0x8) != RendererMode::None)
            {
                SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGENABLE, TRUE);

                if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
                {
                    SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGVERTEXMODE, D3DFOGMODE::D3DFOG_LINEAR);
                    SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGTABLEMODE, D3DFOGMODE::D3DFOG_NONE);
                }
            }
            else
            {
                SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGENABLE, FALSE);

                if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
                {
                    SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGVERTEXMODE, D3DFOGMODE::D3DFOG_NONE);
                    SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGTABLEMODE, D3DFOGMODE::D3DFOG_NONE);
                }
            }
        }

        if (((State.DX.Mode.Mode ^ mode) & (RendererMode::U0x80 | RendererMode::U0x40)) == RendererMode::None)
        {
            State.DX.Mode.Mode = mode;

            return;
        }

        if ((mode & RendererMode::U0x80) != RendererMode::None && (mode & RendererMode::U0x40) != RendererMode::None)
        {
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, State.DX.Mode.DepthBufferMode);
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, TRUE);
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_LESSEQUAL);
        }
        else if ((mode & RendererMode::U0x80) != RendererMode::None)
        {
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, State.DX.Mode.DepthBufferMode);
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, TRUE);
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
        }
        else if ((mode & RendererMode::U0x40) != RendererMode::None)
        {
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, State.DX.Mode.DepthBufferMode);
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, FALSE);
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_LESSEQUAL);
        }
        else
        {
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZENABLE, D3DZBUFFERTYPE::D3DZB_FALSE);
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZWRITEENABLE, FALSE);
            SetRenderState(D3DRENDERSTATETYPE::D3DRS_ZFUNC, D3DCMPFUNC::D3DCMP_ALWAYS);
        }

        State.DX.Mode.Mode = mode;
    }

    void SelectTextureStageValue(const u32 stage, const D3DTEXTURESTAGESTATETYPE type, const DWORD value)
    {
        if (State.DX.Surfaces.Staging.Stages[stage][type] != value)
        {
            State.DX.DirectX.Device->SetTextureStageState(stage, type, value);
            State.DX.Surfaces.Staging.Stages[stage][type] = value;
        }
    }

    void SetShaders(const DXVSN vsn, const DXPSN psn)
    {
        const auto vs = &State.DX.Shaders.VertexShaders[(u32)vsn];

        DXC(State.DX.DirectX.Device->SetVertexShader(vs->Handle == NULL ? vs->FVF : vs->Handle), "Unable to set vertex shader.");

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off)
        {
            DXC(State.DX.DirectX.Device->SetPixelShader(State.DX.Shaders.PixelShaders[(u32)psn].Handle), "Unable to set pixel shader.");
        }
    }

    void SetIndexes(const u16* indexes, const u32 count)
    {
        if (DEFAULT_BUFFER_CAPACITY < count) { Quit("Too many indexes."); }

        if (DEFAULT_BUFFER_CAPACITY < State.DX.Buffers.Index.Count + count)
        {
            State.DX.Buffers.Index.Count = 0;
        }

        const auto options = State.DX.Buffers.Index.Count == 0
            ? D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK
            : D3DLOCK_NOOVERWRITE | D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK;

        u16* buffer;
        DXC(State.DX.Buffers.Index.Buffer->Lock(State.DX.Buffers.Index.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT * sizeof(u16),
            count * GRAPHICS_TRIANGLE_VERTEX_COUNT * sizeof(u16), (BYTE**)&buffer, options), "Unable to lock index buffer.");

        CopyMemory(buffer, indexes, count * GRAPHICS_TRIANGLE_VERTEX_COUNT * sizeof(u16));

        DXC(State.DX.Buffers.Index.Buffer->Unlock(), "Unable to unlock index buffer.");
    }

    void SetIndexSource(const u32 index, IDirect3DIndexBuffer8* buffer)
    {
        DXC(State.DX.DirectX.Device->SetIndices(buffer, index), "Unable to set index source.");
    }

    void SetLightShaders(const RendererMode mode)
    {
        switch (State.DX.Mode.Blend.BlendOperation)
        {
        case RendererBlendOperation::Disable: { SetShaders(DXVSN::Prelit1, DXPSN::Bump); break; }
        case RendererBlendOperation::SelectArgument1: { SetShaders(DXVSN::Prelit1, DXPSN::BumpSpec); break; }
        case RendererBlendOperation::AddSigned: { SetShaders(DXVSN::Shdwbkfc1, DXPSN::DecalTexKill); break; }
        case RendererBlendOperation::Subtract: { SetShaders(DXVSN::Vomni1, DXPSN::Decal); break; }
        default:
        {
            if ((mode & RendererMode::U0x400) != RendererMode::None)
            {
                SetShaders(DXVSN::Pretex, DXPSN::DecalTexKill);
            }
            else
            {
                SetShaders(DXVSN::Prelit1, DXPSN::Decal);
            }

            break;
        }
        }

        SetShaderConstants();
    }

    void SetSkeletonShaders(const RendererMode mode)
    {
        switch (State.DX.Mode.Blend.BlendOperation)
        {
        case RendererBlendOperation::Disable: { SetShaders(DXVSN::Vskelbmp, DXPSN::Bump); break; }
        case RendererBlendOperation::SelectArgument1: { SetShaders(DXVSN::Vskelspc, DXPSN::BumpSpec); break; }
        case RendererBlendOperation::BlendTextureAlphaPreMultiplied: { SetShaders(DXVSN::Vskelsp2, DXPSN::BumpSpec); break; }
        case RendererBlendOperation::BlendCurrentAlpha: { SetShaders(DXVSN::Vskelsp3, DXPSN::BumpSpec); break; }
        default:
        {
            if ((mode & RendererMode::U0x400) != RendererMode::None)
            {
                SetShaders(DXVSN::Vskeltex, DXPSN::DecalTexKill);
            }
            else
            {
                SetShaders(DXVSN::Vskel, DXPSN::Decal);
            }

            break;
        }
        }

        SetShaderConstants();
    }

    void SetBlendShaders(const RendererMode mode)
    {
        switch (State.DX.Mode.Blend.BlendOperation)
        {
        case RendererBlendOperation::None:
        {
            if ((mode & RendererMode::U0x400) != RendererMode::None)
            {
                SetShaders(DXVSN::Texgen, DXPSN::DecalTexKill);
            }
            else
            {
                SetShaders(DXVSN::Dirlight1, DXPSN::Decal);
            }

            break;
        }
        case RendererBlendOperation::SelectArgument2: { SetShaders(DXVSN::Cubic, DXPSN::Decal); break; }
        case RendererBlendOperation::Disable:
        case RendererBlendOperation::SelectArgument1:
        case RendererBlendOperation::Modulate:
        case RendererBlendOperation::BlendDiffuseAlpha:
        case RendererBlendOperation::BlendTextureAlpha:
        case RendererBlendOperation::BlendFactorAlpha:
        case RendererBlendOperation::BlendTextureAlphaPreMultiplied:
        case RendererBlendOperation::BlendCurrentAlpha: { SetShaders(DXVSN::Dirlight1, DXPSN::Decal); break; }
        case RendererBlendOperation::Add: { SetShaders(DXVSN::Vgloss, DXPSN::Gloss); break; }
        case RendererBlendOperation::AddSigned: { SetShaders(DXVSN::Shdwbkfc1, DXPSN::DecalTexKill); break; }
        case RendererBlendOperation::Subtract: { SetShaders(DXVSN::Vomni1, DXPSN::Decal); break; }
        case RendererBlendOperation::AddSmooth: { SetShaders(DXVSN::Vspecmap, DXPSN::Decal); break; }
        default: { SetShaders(DXVSN::Vlmap2, DXPSN::LightMap); break; }
        }

        SetShaderConstants();
    }

    void SetShadersMode(const RendererMode mode)
    {
        switch (State.DX.Mode.Blend.BlendOperation)
        {
        case RendererBlendOperation::None: { SetShaders(DXVSN::Dirlight2, DXPSN::Decal); break; }
        case RendererBlendOperation::Disable:
        {
            SetShaders((mode & RendererMode::U0x400) != RendererMode::None
                ? DXVSN::Vbumpgen : DXVSN::Vbump, DXPSN::Bump);

            break;
        }
        case RendererBlendOperation::SelectArgument1:
        {
            SetShaders((mode & RendererMode::U0x400) != RendererMode::None
                ? DXVSN::Vbmpspgn : DXVSN::Vbmpspec, DXPSN::BumpSpec);

            break;
        }
        case RendererBlendOperation::Modulate: { SetShaders(DXVSN::Vbmpcube, DXPSN::BumpCubic); break; }
        case RendererBlendOperation::AddSigned: { SetShaders(DXVSN::Shdwbkfc2, DXPSN::DecalTexKill); break; }
        case RendererBlendOperation::AddSigned2X: { SetShaders(DXVSN::Vlmap1, DXPSN::LightMap); break; }
        case RendererBlendOperation::Subtract: { SetShaders(DXVSN::Vomni3, DXPSN::Decal);  break; }
        case RendererBlendOperation::BlendDiffuseAlpha: { SetShaders(DXVSN::Vbmpspom, DXPSN::BumpSpecLightMap);  break; }
        case RendererBlendOperation::BlendTextureAlpha: { SetShaders(DXVSN::Vomnibmp, DXPSN::Bump); break; }
        case RendererBlendOperation::BlendFactorAlpha: { SetShaders(DXVSN::Vbmpspom, DXPSN::BumpSpec); break; }
        case RendererBlendOperation::PreModulate: { SetShaders(DXVSN::Vlmbs, DXPSN::Plmbs); break; }
        default: { SetShaders((DXVSN)mode, (DXPSN)mode); break; }
        }

        SetShaderConstants();
    }

    void SetShaderConstants(void)
    {
        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On) { return; }

        if (State.DX.Mode.Blend.BlendOperation == RendererBlendOperation::Disable)
        {
            const f32x4 constants[] =
            {
                {
                    .X = State.DX.Light.Lights[0].Ambient.r,
                    .Y = State.DX.Light.Lights[0].Ambient.g,
                    .Z = State.DX.Light.Lights[0].Ambient.b,
                    .W = 0.0f
                },
                {
                    .X = State.DX.Light.Colors.R[0],
                    .Y = State.DX.Light.Colors.G[0],
                    .Z = State.DX.Light.Colors.B[0],
                    .W = 0.0f
                }
            };

            DXC(State.DX.DirectX.Device->SetPixelShaderConstant(SHADER_REGISTER_0_INDEX, constants,
                MAX_PIXEL_SHADER_REGISTER_COUNT), "Unable to set pixel shader constants.");
        }

        if (!State.DX.Mode.IsChanged) { return; }

        State.DX.Mode.IsChanged = FALSE;

        Matrix4x4 matrix;

        MultiplyMatrix(&matrix, &State.DX.Transform.ModelView, &State.DX.Transform.Projection);

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_0_INDEX].X = matrix.M11;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_0_INDEX].Y = matrix.M21;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_0_INDEX].Z = matrix.M31;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_0_INDEX].W = matrix.M41;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_1_INDEX].X = matrix.M12;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_1_INDEX].Y = matrix.M22;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_1_INDEX].Z = matrix.M32;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_1_INDEX].W = matrix.M42;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_2_INDEX].X = matrix.M13;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_2_INDEX].Y = matrix.M23;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_2_INDEX].Z = matrix.M33;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_2_INDEX].W = matrix.M43;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_3_INDEX].X = matrix.M14;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_3_INDEX].Y = matrix.M24;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_3_INDEX].Z = matrix.M34;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_3_INDEX].W = matrix.M44;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].X = 1.0f;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].Y = 1.0f;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].Z = 1.0f;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_7_INDEX].W = 1.0f;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_24_INDEX].X = 0.0f;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_24_INDEX].Y = 0.0f;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_24_INDEX].Z = 0.0f;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_24_INDEX].W = 0.0f;

        if (State.DX.Light.Mode == RendererLightingMode::On)
        {
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].X = -State.DX.Light.XYZ[0].X;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].Y = -State.DX.Light.XYZ[0].Y;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].Z = -State.DX.Light.XYZ[0].Z;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].X = State.DX.Light.Colors.R[0];
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].Y = State.DX.Light.Colors.R[1];
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].Z = State.DX.Light.Colors.R[2];
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].W = State.DX.Light.Lights[0].Ambient.r;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_20_INDEX].X = -State.DX.Light.XYZ[1].X;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_20_INDEX].Y = -State.DX.Light.XYZ[1].Y;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_20_INDEX].Z = -State.DX.Light.XYZ[1].Z;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_21_INDEX].X = State.DX.Light.Colors.G[0];
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_21_INDEX].Y = State.DX.Light.Colors.G[1];
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_21_INDEX].Z = State.DX.Light.Colors.G[2];
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_21_INDEX].W = State.DX.Light.Lights[0].Ambient.g;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_22_INDEX].X = -State.DX.Light.XYZ[2].X;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_22_INDEX].Y = -State.DX.Light.XYZ[2].Y;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_22_INDEX].Z = -State.DX.Light.XYZ[2].Z;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_23_INDEX].X = State.DX.Light.Colors.B[0];
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_23_INDEX].Y = State.DX.Light.Colors.B[1];
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_23_INDEX].Z = State.DX.Light.Colors.B[2];
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_23_INDEX].W = State.DX.Light.Lights[0].Ambient.b;
        }
        else
        {
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].X = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].Y = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].Z = 0.0f;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].X = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].Y = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].Z = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_8_INDEX].W = 1.0f;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_20_INDEX].X = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_20_INDEX].Y = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_20_INDEX].Z = 0.0f;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_21_INDEX].X = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_21_INDEX].Y = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_21_INDEX].Z = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_21_INDEX].W = 1.0f;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_22_INDEX].X = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_22_INDEX].Y = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_22_INDEX].Z = 0.0f;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_23_INDEX].X = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_23_INDEX].Y = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_23_INDEX].Z = 0.0f;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_23_INDEX].W = 1.0f;
        }

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_4_INDEX].W = 0.0f;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_5_INDEX].X = 0.0f;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_5_INDEX].Y = 1.0f;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_5_INDEX].Z = 0.5f;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_5_INDEX].W = State.DX.Light.Alpha;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_6_INDEX].X = State.DX.Fog.Start;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_6_INDEX].Y = 1.0f / (State.DX.Fog.End - State.DX.Fog.Start);

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_9_INDEX].X = State.DX.Light.Color.R;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_9_INDEX].Y = State.DX.Light.Color.G;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_9_INDEX].Z = State.DX.Light.Color.B;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_9_INDEX].W = 0.0f;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_10_INDEX].X = State.DX.Fog.Color.R;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_10_INDEX].Y = State.DX.Fog.Color.G;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_10_INDEX].Z = State.DX.Fog.Color.B;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_10_INDEX].W = 0.0f;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_12_INDEX].X = State.DX.Transform.O2W.M11;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_12_INDEX].Y = State.DX.Transform.O2W.M21;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_12_INDEX].Z = State.DX.Transform.O2W.M31;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_12_INDEX].W = State.DX.Transform.O2W.M41;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_13_INDEX].X = State.DX.Transform.O2W.M12;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_13_INDEX].Y = State.DX.Transform.O2W.M22;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_13_INDEX].Z = State.DX.Transform.O2W.M32;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_13_INDEX].W = State.DX.Transform.O2W.M42;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_14_INDEX].X = State.DX.Transform.O2W.M13;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_14_INDEX].Y = State.DX.Transform.O2W.M23;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_14_INDEX].Z = State.DX.Transform.O2W.M33;
        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_14_INDEX].W = State.DX.Transform.O2W.M43;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_20_INDEX].W = 0.0f;

        State.DX.Shaders.Constants.Staging[SHADER_REGISTER_22_INDEX].W = 0.0f;

        if ((State.DX.Mode.Mode & RendererMode::U0x400) != RendererMode::None)
        {
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_15_INDEX].X = State.DX.Transform.W2L.M11;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_15_INDEX].Y = State.DX.Transform.W2L.M21;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_15_INDEX].Z = State.DX.Transform.W2L.M31;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_15_INDEX].W = State.DX.Transform.W2L.M41;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_16_INDEX].X = State.DX.Transform.W2L.M12;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_16_INDEX].Y = State.DX.Transform.W2L.M22;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_16_INDEX].Z = State.DX.Transform.W2L.M32;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_16_INDEX].W = State.DX.Transform.W2L.M42;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_17_INDEX].X = State.DX.Transform.W2L.M13;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_17_INDEX].Y = State.DX.Transform.W2L.M23;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_17_INDEX].Z = State.DX.Transform.W2L.M33;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_17_INDEX].W = State.DX.Transform.W2L.M43;

            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_18_INDEX].X = State.DX.Transform.W2L.M14;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_18_INDEX].Y = State.DX.Transform.W2L.M24;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_18_INDEX].Z = State.DX.Transform.W2L.M34;
            State.DX.Shaders.Constants.Staging[SHADER_REGISTER_18_INDEX].W = State.DX.Transform.W2L.M44;
        }

        for (u32 x = 0; x < ACTUAL_SHADER_REGISTER_CONSTANT_COUNT; x++)
        {
            auto actual = &State.DX.Shaders.Constants.Actual[x];
            const auto staging = &State.DX.Shaders.Constants.Staging[x];

            if (actual->X != staging->X || actual->Y != staging->Y || actual->Z != staging->Z || actual->W != staging->W)
            {
                DXC(State.DX.DirectX.Device->SetVertexShaderConstant(x, staging, 1), "Unable to set vertex shader constants.");

                actual->X = staging->X;
                actual->Y = staging->Y;
                actual->Z = staging->Z;
                actual->W = staging->W;
            }
        }
    }

    void SetRTLVXShaders(const RendererMode mode)
    {
        switch (State.DX.Mode.Blend.BlendOperation)
        {
        case RendererBlendOperation::AddSigned: { SetShaders(DXVSN::Shdwbkfc1, DXPSN::DecalTexKill); break; }
        case RendererBlendOperation::Subtract: { SetShaders(DXVSN::Vomni2, DXPSN::Decal); break; }
        default:
        {
            if ((mode & RendererMode::U0x400) != RendererMode::None)
            {
                SetShaders(DXVSN::Pretex, DXPSN::DecalTexKill);
            }
            else
            {
                SetShaders(DXVSN::Prelit2, DXPSN::Decal);
            }

            break;
        }
        }

        SetShaderConstants();
    }

    BOOL SelectLight(void)
    {
        State.DX.Mode.IsChanged = TRUE;

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off) { return TRUE; }

        State.DX.Light.Lights[0].Type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
        State.DX.Light.Lights[0].Diffuse.a = 1.0f;
        State.DX.Light.Lights[0].Specular.r = 0.0f;
        State.DX.Light.Lights[0].Specular.g = 0.0f;
        State.DX.Light.Lights[0].Specular.b = 0.0f;
        State.DX.Light.Lights[0].Specular.a = 0.0f;
        State.DX.Light.Lights[0].Position.x = 0.0f;
        State.DX.Light.Lights[0].Position.y = 0.0f;
        State.DX.Light.Lights[0].Position.z = 0.0f;
        State.DX.Light.Lights[0].Range = 0.0f;
        State.DX.Light.Lights[0].Falloff = 0.0f;
        State.DX.Light.Lights[0].Attenuation0 = 0.0f;
        State.DX.Light.Lights[0].Attenuation1 = 0.0f;
        State.DX.Light.Lights[0].Attenuation2 = 0.0f;
        State.DX.Light.Lights[0].Theta = 0.0f;
        State.DX.Light.Lights[0].Phi = 0.0f;

        State.DX.Light.Lights[1].Type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
        State.DX.Light.Lights[1].Diffuse.a = 0.0f;
        State.DX.Light.Lights[1].Specular.r = 0.0f;
        State.DX.Light.Lights[1].Specular.g = 0.0f;
        State.DX.Light.Lights[1].Specular.b = 0.0f;
        State.DX.Light.Lights[1].Specular.a = 0.0f;
        State.DX.Light.Lights[1].Ambient.r = 0.0f;
        State.DX.Light.Lights[1].Ambient.g = 0.0f;
        State.DX.Light.Lights[1].Ambient.b = 0.0f;
        State.DX.Light.Lights[1].Ambient.a = 0.0f;
        State.DX.Light.Lights[1].Position.x = 0.0f;
        State.DX.Light.Lights[1].Position.y = 0.0f;
        State.DX.Light.Lights[1].Position.z = 0.0f;
        State.DX.Light.Lights[1].Range = 0.0f;
        State.DX.Light.Lights[1].Falloff = 0.0f;
        State.DX.Light.Lights[1].Attenuation0 = 0.0f;
        State.DX.Light.Lights[1].Attenuation1 = 0.0f;
        State.DX.Light.Lights[1].Attenuation2 = 0.0f;
        State.DX.Light.Lights[1].Theta = 0.0f;
        State.DX.Light.Lights[1].Phi = 0.0f;

        State.DX.Light.Lights[2].Type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
        State.DX.Light.Lights[2].Diffuse.a = 0.0f;
        State.DX.Light.Lights[2].Specular.r = 0.0f;
        State.DX.Light.Lights[2].Specular.g = 0.0f;
        State.DX.Light.Lights[2].Specular.b = 0.0f;
        State.DX.Light.Lights[2].Specular.a = 0.0f;
        State.DX.Light.Lights[2].Ambient.r = 0.0f;
        State.DX.Light.Lights[2].Ambient.g = 0.0f;
        State.DX.Light.Lights[2].Ambient.b = 0.0f;
        State.DX.Light.Lights[2].Ambient.a = 0.0f;
        State.DX.Light.Lights[2].Position.x = 0.0f;
        State.DX.Light.Lights[2].Position.y = 0.0f;
        State.DX.Light.Lights[2].Position.z = 0.0f;
        State.DX.Light.Lights[2].Range = 0.0f;
        State.DX.Light.Lights[2].Falloff = 0.0f;
        State.DX.Light.Lights[2].Attenuation0 = 0.0f;
        State.DX.Light.Lights[2].Attenuation1 = 0.0f;
        State.DX.Light.Lights[2].Attenuation2 = 0.0f;
        State.DX.Light.Lights[2].Theta = 0.0f;
        State.DX.Light.Lights[2].Phi = 0.0f;

        for (u32 x = 0; x < MAX_LIGHT_COUNT; x++)
        {
            auto light = &State.DX.Light.Lights[x];

            auto isColorMissing = light->Diffuse.r == 0.0f && light->Diffuse.g == 0.0f && light->Diffuse.b == 0.0f;
            auto isDirectionMissing = light->Direction.x == 0.0f && light->Direction.y == 0.0f && light->Direction.z == 0.0f;

            if (isColorMissing || isDirectionMissing)
            {
                State.DX.DirectX.Device->LightEnable(x, FALSE);
            }
            else
            {
                DXC(State.DX.DirectX.Device->SetLight(x, light), "Unable to set light.");

                State.DX.DirectX.Device->LightEnable(x, TRUE);
            }
        }

        return TRUE;
    }

    void Initialize(DXWindow* window)
    {
        window->Width = MIN_GRAPHICS_RESOLUTION_WIDTH;
        window->Height = MIN_GRAPHICS_RESOLUTION_HEIGHT;
        window->BitsPerPixel = GRAPHICS_BITS_PER_PIXEL_32;

        window->Pixels.Pixels = NULL;

        ZeroMemory(&window->Pixels.Values, sizeof(u32) * MAX_GRAPHICS_RESOLUTION_HEIGHT);

        window->SwapChain = NULL;
        window->DepthSurface = NULL;
    }

    void Initialize(DXWindow* window, D3DPRESENT_PARAMETERS* pp)
    {
        D3DDISPLAYMODE dm = { 0 };

        if (State.Window.Mode == RendererWindowMode::Off)
        {
            BOOL found = FALSE;
            const auto count = State.DX.DirectX.Instance->GetAdapterModeCount(State.DX.Devices.Current.Index);

            for (u32 x = 0; x < count; x++)
            {
                DXC(State.DX.DirectX.Instance->EnumAdapterModes(State.DX.Devices.Current.Index, x, &dm),
                    "Unable to enumerate device modes.");

                if (dm.Width == window->Width && dm.Height == window->Height)
                {
                    if (window->BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_16)
                    {
                        if (dm.Format == D3DFORMAT::D3DFMT_R5G6B5)
                        {
                            found = TRUE; break;
                        }
                    }
                    else if (dm.Format == D3DFORMAT::D3DFMT_R8G8B8
                                || dm.Format == D3DFORMAT::D3DFMT_A8R8G8B8
                                || dm.Format == D3DFORMAT::D3DFMT_X8R8G8B8)
                    {
                        found = TRUE; break;
                    }
                }
            }

            if (!found) { Quit("Unable to find a valid video mode for %d x %d.", window->Width, window->Height); }
        }
        else
        {
            DXC(State.DX.DirectX.Instance->GetAdapterDisplayMode(State.DX.Devices.Current.Index, &dm),
                "Unable to acquire display mode for window mode.");
        }

        auto stencilFormat = window->BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_16
                                ? D3DFORMAT::D3DFMT_D16 : D3DFORMAT::D3DFMT_D32;

        if (window->BitsPerPixel != GRAPHICS_BITS_PER_PIXEL_16)
        {
            auto result = State.DX.DirectX.Instance->CheckDeviceFormat(State.DX.Devices.Current.Index, D3DDEVTYPE::D3DDEVTYPE_HAL,
                dm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRESOURCETYPE::D3DRTYPE_SURFACE, D3DFORMAT::D3DFMT_D32);

            if (result != D3D_OK)
            {
                stencilFormat = D3DFORMAT::D3DFMT_D24S8;

                result = State.DX.DirectX.Instance->CheckDeviceFormat(State.DX.Devices.Current.Index, D3DDEVTYPE::D3DDEVTYPE_HAL,
                    dm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRESOURCETYPE::D3DRTYPE_SURFACE, D3DFORMAT::D3DFMT_D24S8);

                if (result != D3D_OK)
                {
                    stencilFormat = D3DFORMAT::D3DFMT_D16;

                    DXC(State.DX.DirectX.Instance->CheckDeviceFormat(State.DX.Devices.Current.Index, D3DDEVTYPE::D3DDEVTYPE_HAL,
                        dm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRESOURCETYPE::D3DRTYPE_SURFACE, D3DFORMAT::D3DFMT_D16),
                        "Unable to find depth buffer format.");
                }
            }
        }

        pp->AutoDepthStencilFormat = stencilFormat;
        pp->BackBufferFormat = dm.Format;
        pp->BackBufferHeight = window->Height;
        pp->BackBufferWidth = window->Width;
        pp->EnableAutoDepthStencil = TRUE;
        pp->Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        pp->hDeviceWindow = window->HWND;
        pp->MultiSampleType = D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_NONE;
        pp->Windowed = State.Window.Mode == RendererWindowMode::On;

        if (State.Window.Mode == RendererWindowMode::Off)
        {
            pp->BackBufferCount = 2;
            pp->SwapEffect = D3DSWAPEFFECT::D3DSWAPEFFECT_FLIP;
            pp->FullScreen_RefreshRateInHz = dm.RefreshRate;
            pp->FullScreen_PresentationInterval = State.Settings.SyncRetraceMode == SyncRetraceOptionsMode::Off
                ? D3DPRESENT_INTERVAL_IMMEDIATE : D3DPRESENT_INTERVAL_TWO;

            return;
        }

        pp->BackBufferCount = 1;
        pp->SwapEffect = D3DSWAPEFFECT::D3DSWAPEFFECT_COPY;
        pp->FullScreen_RefreshRateInHz = 0;
        pp->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    }

    BOOL Initialize(const u32 width, const u32 height, const u32 indx)
    {
        auto dxw = State.DX.Windows.Windows[indx];

        if (dxw == NULL) { return FALSE; }

        if (dxw->SwapChain != NULL)
        {
            dxw->SwapChain->Release();
            dxw->SwapChain = NULL;
        }

        if (dxw->DepthSurface != NULL)
        {
            dxw->DepthSurface->Release();
            dxw->DepthSurface = NULL;
        }

        dxw->Width = width;
        dxw->Height = height;

        Initialize(dxw, &State.DX.Devices.Current.SwapChainParams);

        DXC(State.DX.DirectX.Device->CreateAdditionalSwapChain(
            &State.DX.Devices.Current.SwapChainParams,
            &dxw->SwapChain), "Unable to create additional swap chain.");

        DXC(State.DX.DirectX.Device->CreateDepthStencilSurface(dxw->Width, dxw->Height,
            State.DX.Devices.Current.SwapChainParams.AutoDepthStencilFormat,
            D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_NONE, &dxw->DepthSurface),
            "Unable to create additional depth buffer.");

        return TRUE;
    }
}