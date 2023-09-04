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
#include "Renderers.Basic.hxx"

using namespace Assets;
using namespace Mathematics;
using namespace Renderer::Module;
using namespace Renderers::Modules;
using namespace Renderers;

namespace DX
{
    u32* AcquireTexturePixels(const Texture* texture, const u32 width)
    {
        auto txw = texture->Definition.Width;

        if (txw == width) { return &State.DX.Pixels.Pixels[0]; }

        u32 offset = 0;

        do
        {
            offset += txw * txw * sizeof(u32);

            txw /= 2;

            if (txw == 0) { Quit("Unable to get texture source size: %d x %d.", width, width); }
        } while (txw != width);

        return &State.DX.Pixels.Pixels[offset];
    }

    void ConvertTexture(const DX::DXTexture* dxt, const Texture* texture)
    {
        if (texture->Definition.Format == TextureFormat::Palette24Bit
            || texture->Definition.Format == TextureFormat::Palette32Bit
            || texture->Definition.Format == TextureFormat::OpacityMap8Bit)
        {
            for (u32 x = 0; x < MAX_PALETTE_BUFFER_SIZE; x++)
            {
                const auto r = (u32)texture->Data.Colors[x * 3 + 0];
                const auto g = (u32)texture->Data.Colors[x * 3 + 1];
                const auto b = (u32)texture->Data.Colors[x * 3 + 2];

                State.DX.Pixels.Colors[x] = (r << 16) | (g << 8) | b;
            }

            if (texture->Definition.Format == TextureFormat::Palette32Bit)
            {
                for (u32 x = 0; x < MAX_PALETTE_BUFFER_SIZE; x++)
                {
                    const auto alpha = (u32)texture->Data.Colors[MAX_PALETTE_BUFFER_SIZE * 3 + x];

                    State.DX.Pixels.Colors[x] = (alpha << 24) | State.DX.Pixels.Colors[x];
                }
            }
            else if (texture->Data.Alphas == NULL)
            {
                for (u32 x = 0; x < MAX_PALETTE_BUFFER_SIZE; x++)
                {
                    State.DX.Pixels.Colors[x] = 0xff000000 | State.DX.Pixels.Colors[x];
                }
            }
        }

        const u32 size = dxt->ActualWidth * dxt->ActualHeight;

        switch (texture->Definition.Format)
        {
        case TextureFormat::Palette24Bit:
        case TextureFormat::Palette32Bit:
        {
            for (u32 xx = 0; xx < size; xx++)
            {
                const auto indx = texture->Data.Indexes[xx];

                State.DX.Pixels.Pixels[xx] = State.DX.Pixels.Colors[indx];
            }

            break;
        }
        case TextureFormat::OpacityMap8Bit:
        {
            for (u32 xx = 0; xx < size; xx++)
            {
                const auto alpha = texture->Data.Alphas[xx];

                const auto indx = texture->Data.Indexes[xx];

                State.DX.Pixels.Pixels[xx] = (((u32)alpha) << 24) | State.DX.Pixels.Colors[indx];
            }

            break;
        }
        case TextureFormat::Color32Bit:
        {
            for (u32 xx = 0; xx < size; xx++)
            {
                State.DX.Pixels.Pixels[xx] = texture->Data.ARGB[xx];
            }

            break;
        }
        }
    }

    void CopyTexture16BitTransparent(const u32* colors, void* pixels, const s32 pitch, const u32 width, const u32 height)
    {
        auto buffer = (u16*)pixels;
        const auto stride = pitch / sizeof(u16);

        for (u32 x = 0; x < height; x++)
        {
            for (u32 xx = 0; xx < width; xx++)
            {
                auto color = colors[height * x + xx];

                u16 r = (color >> 28) << 12;
                u16 g = ((color >> 20) & 0xf) << 8;
                u16 b = ((color >> 12) & 0xf) << 4;
                u16 a = (color >> 4) & 0xf;

                buffer[stride * x + xx] = r | g | b | a;
            }
        }
    }

    void CopyTexture16BitOpaque(const u32* colors, void* pixels, const s32 pitch, const u32 width, const u32 height)
    {
        auto buffer = (u16*)pixels;
        const auto stride = pitch / sizeof(u16);

        for (u32 x = 0; x < height; x++)
        {
            for (u32 xx = 0; xx < width; xx++)
            {
                const auto color = colors[height * x + xx];

                u16 r = (color >> 19) << 11;
                u16 g = ((color >> 10) & 0x3f) << 5;
                u16 b = (color >> 3) & 0x1f;

                buffer[stride * x + xx] = r | g | b;
            }
        }
    }

    void CopyTexture32Bit(const u32* colors, void* pixels, const s32 pitch, const u32 width, const u32 height)
    {
        auto buffer = (u32*)pixels;
        const auto stride = pitch / sizeof(u32);

        for (u32 x = 0; x < height; x++)
        {
            for (u32 xx = 0; xx < width; xx++)
            {
                buffer[stride * x + xx] = colors[height * x + xx];
            }
        }
    }

    void ProcessTextureMipLevel(const Texture* texture, const u32 currentWidth, const u32 nextWidth)
    {
        const auto current = AcquireTexturePixels(texture, currentWidth);
        const auto next = AcquireTexturePixels(texture, nextWidth);

        for (u32 x = 0; x < currentWidth; x = x + 2)
        {
            for (u32 xx = 0; xx < currentWidth; xx = xx + 2)
            {
                const auto c00 = current[x * currentWidth + xx];
                const auto c01 = current[x * currentWidth + (xx + 1)];
                const auto c10 = current[(x + 1) * currentWidth + xx];
                const auto c11 = current[(x + 1) * currentWidth + (xx + 1)];

                const auto ca = ((c00 >> 24) + (c01 >> 24) + (c10 >> 24) + (c11 >> 24)) / 4;
                const auto cr = ((c00 >> 16 & 0xff) + (c01 >> 16 & 0xff) + (c10 >> 16 & 0xff) + (c11 >> 16 & 0xff)) / 4;
                const auto cg = ((c00 >> 8 & 0xff) + (c01 >> 8 & 0xff) + (c10 >> 8 & 0xff) + (c11 >> 8 & 0xff)) / 4;
                const auto cb = ((c00 & 0xff) + (c01 & 0xff) + (c10 & 0xff) + (c11 & 0xff)) / 4;

                next[((x * nextWidth) + xx) / 2] = (ca << 24) | (cr << 16) | (cg << 8) | cb;
            }
        }
    }

    void ProcessTextureMipLevels(const Texture* texture)
    {
        const auto levels = texture->Definition.MipLevelCount + 1;

        auto dims = texture->Definition.Width;

        for (u32 x = 1; x < levels; x++)
        {
            const auto next = dims / 2;

            ProcessTextureMipLevel(texture, dims, next);

            dims = next;
        }
    }
}

namespace Renderer::Module
{
    extern "C" u32 AcquireTextureInformation(const u32) { return DEFAULT_TEXTURE_CACHE_CAPACITY; } // NOTE: Not being called by the application.

    extern "C" u32 AllocateTexture(const Texture * texture)
    {
        u32 indx = INVALID_TEXTURE_INDEX;

        for (u32 x = MIN_VALID_TEXTURE_INDEX; x < MAX_TEXTURE_COUNT; x++)
        {
            if (State.DX.Textures.Textures[x].Texture == NULL)
            {
                indx = x;

                break;
            }
        }

        if (indx == INVALID_TEXTURE_INDEX || MAX_TEXTURE_COUNT <= indx) { return INVALID_TEXTURE_INDEX; }

        FreeTexture(indx);

        auto dxt = &State.DX.Textures.Textures[indx];

        dxt->Width = texture->Definition.Width;
        dxt->Height = texture->Definition.Height;
        dxt->Options = texture->Definition.Options;
        dxt->MipLevelCount = texture->Definition.MipLevelCount;

        switch (texture->Definition.Format)
        {
        case TextureFormat::Palette24Bit:
        case TextureFormat::Palette32Bit: { dxt->Format = TextureFormat::Color16Bit565; break; }
        case TextureFormat::OpacityMap8Bit: { dxt->Format = State.DX.Surfaces.Formats.Surface; break; }
        case TextureFormat::Color32Bit:
        {
            dxt->Format = (dxt->Options & TEXTURE_OPTION_IGNORE_ALPHA)
                ? TextureFormat::Color16Bit565
                : State.DX.Surfaces.Formats.Surface;

            break;
        }
        default: { return INVALID_TEXTURE_INDEX; }
        }

        dxt->ActualWidth = (s16)texture->Definition.Width;
        dxt->ActualHeight = (s16)texture->Definition.Height;
        dxt->ActualMipLevelCount = texture->Definition.MipLevelCount + 1;

        u32 mlc = TEXTURE_MIP_MAP_COUNT_0;
        const auto size = Max(dxt->ActualHeight, dxt->ActualWidth);

        switch (size)
        {
        case TEXTURE_DIMENSIONS_SIZE_1: { mlc = TEXTURE_MIP_MAP_COUNT_1; break; }
        case TEXTURE_DIMENSIONS_SIZE_2: { mlc = TEXTURE_MIP_MAP_COUNT_2; break; }
        case TEXTURE_DIMENSIONS_SIZE_4: { mlc = TEXTURE_MIP_MAP_COUNT_3; break; }
        case TEXTURE_DIMENSIONS_SIZE_8: { mlc = TEXTURE_MIP_MAP_COUNT_4; break; }
        case TEXTURE_DIMENSIONS_SIZE_16: { mlc = TEXTURE_MIP_MAP_COUNT_5; break; }
        case TEXTURE_DIMENSIONS_SIZE_32: { mlc = TEXTURE_MIP_MAP_COUNT_6; break; }
        case TEXTURE_DIMENSIONS_SIZE_64: { mlc = TEXTURE_MIP_MAP_COUNT_7; break; }
        case TEXTURE_DIMENSIONS_SIZE_128: { mlc = TEXTURE_MIP_MAP_COUNT_8; break; }
        case TEXTURE_DIMENSIONS_SIZE_256: { mlc = TEXTURE_MIP_MAP_COUNT_9; break; }
        case TEXTURE_DIMENSIONS_SIZE_512: { mlc = TEXTURE_MIP_MAP_COUNT_10; break; }
        case TEXTURE_DIMENSIONS_SIZE_1024: { mlc = TEXTURE_MIP_MAP_COUNT_11; break; }
        case TEXTURE_DIMENSIONS_SIZE_2048: { mlc = TEXTURE_MIP_MAP_COUNT_12; break; }
        case TEXTURE_DIMENSIONS_SIZE_4096: { mlc = TEXTURE_MIP_MAP_COUNT_13; break; }
        default: { Quit("Unknown texture size: %d x %d.", dxt->ActualWidth, dxt->ActualHeight); }
        }

        if (dxt->ActualMipLevelCount < TEXTURE_DIMENSIONS_SIZE_1 || mlc < dxt->ActualMipLevelCount)
        {
            Quit("Invalid mip level count: %d.", dxt->ActualMipLevelCount);
        }

        auto format = D3DFORMAT::D3DFMT_UNKNOWN;

        switch (dxt->Format)
        {
        case TextureFormat::Color32Bit: { format = D3DFORMAT::D3DFMT_A8R8G8B8; break; }
        case TextureFormat::Color16Bit565: { format = D3DFORMAT::D3DFMT_R5G6B5; break; }
        case TextureFormat::Color16Bit4444: { format = D3DFORMAT::D3DFMT_A4R4G4B4; break; }
        default: { return INVALID_TEXTURE_INDEX; }
        }

        if (State.DX.DirectX.Device->CreateTexture(dxt->ActualWidth, dxt->ActualHeight,
            dxt->ActualMipLevelCount, D3DUSAGE_NONE, format, D3DPOOL::D3DPOOL_MANAGED, &dxt->Texture) != D3D_OK)
        {
            FreeTexture(indx);

            return INVALID_TEXTURE_INDEX;
        }

        return indx;
    }

    extern "C" u32 BeginCubeMapRender(const u32, const RendererCubeMapSide) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 BeginRenderToTexture(u32 indx)
    {
        if (State.Settings.TextureCount <= indx) { return FALSE; }

        State.DX.DirectX.Device->GetRenderTarget(&State.DX.Surfaces.Target.Surface);
        State.DX.DirectX.Device->GetDepthStencilSurface(&State.DX.Surfaces.Target.DepthSurface);

        IDirect3DSurface8* surface;

        State.DX.Textures.Surfaces.Textures[indx]->GetSurfaceLevel(0, &surface);

        State.DX.DirectX.Device->SetRenderTarget(surface, State.DX.Surfaces.TextureDepthSurface);

        surface->Release();

        return TRUE;
    }

    extern "C" u32 CopyBackBufferToRenderTexture(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 EndCubeMapRender(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 EndRenderToTexture(void)
    {
        State.DX.DirectX.Device->SetRenderTarget(State.DX.Surfaces.Target.Surface, State.DX.Surfaces.Target.DepthSurface);

        State.DX.Surfaces.Target.Surface->Release();
        State.DX.Surfaces.Target.DepthSurface->Release();

        return TRUE;
    }

    extern "C" u32 FreeTexture(const u32 indx)
    {
        if (MAX_TEXTURE_COUNT <= indx) { return FALSE; }

        auto texture = &State.DX.Textures.Textures[indx];

        if (texture->Texture != NULL)
        {
            texture->Texture->Release();
            texture->Texture = NULL;
        }

        ZeroMemory(texture, sizeof(DX::DXTexture));

        return TRUE;
    }

    extern "C" u32 ResetTextureCache(void)
    {
        for (u32 x = INVALID_TEXTURE_INDEX; x < MAX_TEXTURE_COUNT; x++)
        {
            FreeTexture(x);
        }

        return TRUE;
    }

    extern "C" u32 SelectCubeTexture(const u32, const u32) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SelectRenderTexture(const u32 indx)
    {
        State.DX.Textures.Surfaces.Staging[0] = State.DX.Textures.Surfaces.Textures[indx];

        return TRUE;
    }

    extern "C" u32 SelectTexture(const u32 tindx, const u32 sindx)
    {
        if (MAX_TEXTURE_COUNT <= tindx) { return FALSE; }

        State.DX.Textures.Surfaces.Staging[sindx] = State.DX.Textures.Textures[tindx].Texture;

        return TRUE;
    }

    extern "C" u32 UploadTexture(const u32 indx, const Texture * texture)
    {
        if (MAX_TEXTURE_COUNT <= indx) { return FALSE; }

        auto dxt = &State.DX.Textures.Textures[indx];

        auto dims = dxt->ActualWidth;

        DX::ConvertTexture(dxt, texture);
        DX::ProcessTextureMipLevels(texture);

        auto surface = dxt->Format == TextureFormat::Color16Bit565
            ? State.DX.Surfaces.Opaque
            : State.DX.Surfaces.Transparent;

        for (u32 x = 0; x < dxt->ActualMipLevelCount; x++)
        {
            D3DLOCKED_RECT lr;

            const RECT rect =
            {
                .left = 0,
                .top = 0,
                .right = dims,
                .bottom = dims
            };

            DX::DXC(surface->LockRect(&lr, &rect, D3DLOCK_NOSYSLOCK), "Unable to lock source texture.");

            auto pixels = DX::AcquireTexturePixels(texture, dims);

            switch (dxt->Format)
            {
            case TextureFormat::Color32Bit: { DX::CopyTexture32Bit(pixels, lr.pBits, lr.Pitch, dims, dims); break; }
            case TextureFormat::Color16Bit565: { DX::CopyTexture16BitOpaque(pixels, lr.pBits, lr.Pitch, dims, dims); break; }
            case TextureFormat::Color16Bit4444: { DX::CopyTexture16BitTransparent(pixels, lr.pBits, lr.Pitch, dims, dims); break; }
            }

            DX::DXC(surface->UnlockRect(), "Unable to unlock texture level.");

            IDirect3DSurface8* sl;
            DX::DXC(dxt->Texture->GetSurfaceLevel(x, &sl), "Unable to acquire surface level on texture.");

            DX::DXC(State.DX.DirectX.Device->CopyRects(surface, &rect, 1, sl, NULL), "Unable to copy texture surface rectangle.");

            sl->Release();

            dims /= 2;
        }

        return TRUE;
    }
}