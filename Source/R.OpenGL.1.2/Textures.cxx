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
#include "Basic.hxx"
#include "GL.hxx"
#include "Module.hxx"
#include "Quit.hxx"
#include "Textures.hxx"

using namespace Assets;
using namespace Renderer::Module;
using namespace Renderers;

namespace GL
{
    u32* AcquireTexturePixels(const Texture* texture, const u32 dimensions)
    {
        auto dims = texture->Definition.Width;

        if (dims == dimensions) { return &State.GL.Textures.Buffers.Pixels[0]; }

        u32 offset = 0;

        do
        {
            offset += dims * dims * sizeof(u32);

            dims /= 2;

            if (dims == 0) { Quit("Unable to get texture source size: %d x %d.", dimensions, dimensions); }
        } while (dims != dimensions);

        return &State.GL.Textures.Buffers.Pixels[offset];
    }

    void BindTexture(const u32 indx, const u32 texture)
    {
        switch (indx)
        {
        case TEXTURE_STAGE_0_INDEX: { GLF.glActiveTextureARB(GL_TEXTURE0_ARB); break; }
        case TEXTURE_STAGE_1_INDEX: { GLF.glActiveTextureARB(GL_TEXTURE1_ARB); break; }
        case TEXTURE_STAGE_2_INDEX: { GLF.glActiveTextureARB(GL_TEXTURE2_ARB); break; }
        case TEXTURE_STAGE_3_INDEX: { GLF.glActiveTextureARB(GL_TEXTURE3_ARB); break; }
        default: { return; }
        }

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        GLF.glActiveTextureARB(GL_TEXTURE0_ARB);
    }

    void ConvertTexture(const Texture* texture)
    {
        if (texture->Definition.Format == TextureFormat::Palette24Bit
            || texture->Definition.Format == TextureFormat::Palette32Bit
            || texture->Definition.Format == TextureFormat::OpacityMap8Bit)
        {
            for (u32 x = 0; x < MAX_PALETTE_BUFFER_SIZE; x++)
            {
                auto r = (u32)texture->Data.Colors[x * 3 + 0];
                auto g = (u32)texture->Data.Colors[x * 3 + 1];
                auto b = (u32)texture->Data.Colors[x * 3 + 2];

                State.GL.Textures.Buffers.Colors[x] = (r << 16) | (g << 8) | (b << 0);
            }

            if (texture->Definition.Format == TextureFormat::Palette32Bit)
            {
                for (u32 x = 0; x < MAX_PALETTE_BUFFER_SIZE; x++)
                {
                    auto alpha = (u32)texture->Data.Colors[MAX_PALETTE_BUFFER_SIZE * 3 + x];

                    State.GL.Textures.Buffers.Colors[x] = (alpha << 24) | State.GL.Textures.Buffers.Colors[x];
                }
            }
            else if (texture->Data.Alphas == NULL)
            {
                for (u32 x = 0; x < MAX_PALETTE_BUFFER_SIZE; x++)
                {
                    State.GL.Textures.Buffers.Colors[x] = 0xff000000 | State.GL.Textures.Buffers.Colors[x];
                }
            }
        }

        auto pixels = &State.GL.Textures.Buffers.Pixels[0];
        const u32 size = texture->Definition.Width * texture->Definition.Width;

        switch (texture->Definition.Format)
        {
        case TextureFormat::Palette24Bit:
        case TextureFormat::Palette32Bit:
        {
            for (u32 xx = 0; xx < size; xx++)
            {
                auto indx = texture->Data.Indexes[xx];

                pixels[xx] = State.GL.Textures.Buffers.Colors[indx];
            }

            break;
        }
        case TextureFormat::OpacityMap8Bit:
        {
            for (u32 xx = 0; xx < size; xx++)
            {
                auto alpha = texture->Data.Alphas[xx];

                auto indx = texture->Data.Indexes[xx];

                pixels[xx] = (((u32)alpha) << 24) | State.GL.Textures.Buffers.Colors[indx];
            }

            break;
        }
        case TextureFormat::Color32Bit:
        {
            for (u32 xx = 0; xx < size; xx++)
            {
                pixels[xx] = texture->Data.ARGB[xx];
            }

            break;
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
    extern "C" u32 __cdecl AllocateTexture(const Texture * texture)
    {
        if (MAX_TEXTURE_COUNT <= State.GL.Textures.TextureCount) { return INVALID_TEXTURE_INDEX; }

        u32 id = INVALID_TEXTURE_ID;

        glGenTextures(1, &id);

        return id;
    }

    extern "C" u32 BeginCubeMapRender(const u32, const RendererCubeMapSide) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 BeginRenderToTexture(const u32 indx)
    {
        glBindTexture(GL_TEXTURE_2D, State.GL.Textures.Render.Textures[indx].ID);
        GL::GLF.wglReleaseTexImageARB(State.GL.Textures.Render.Textures[indx].Buffer, WGL_FRONT_LEFT_ARB);
        wglMakeCurrent(State.GL.Textures.Render.Textures[indx].DeviceContext,
            State.GL.Textures.Render.Textures[indx].RenderContext);

        State.GL.Textures.Render.Index = indx;
        State.GL.Textures.Render.IsActive = TRUE;
        State.GL.Textures.Render.Direction = -1.0f;

        glDrawBuffer(GL_FRONT);
        glReadBuffer(GL_FRONT);

        return TRUE;
    }

    extern "C" u32 CopyBackBufferToRenderTexture(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 EndCubeMapRender(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 EndRenderToTexture(void)
    {
        glBindTexture(GL_TEXTURE_2D, State.GL.Textures.Render.Textures[State.GL.Textures.Render.Index].ID);
        wglMakeCurrent(State.Window.DeviceContext, State.Window.RenderContext);
        GL::GLF.wglBindTexImageARB(State.GL.Textures.Render.Textures[State.GL.Textures.Render.Index].Buffer, WGL_FRONT_LEFT_ARB);

        State.GL.Textures.Render.IsActive = FALSE;
        State.GL.Textures.Render.Direction = 1.0f;

        return TRUE;
    }

    extern "C" u32 FreeTexture(const u32 indx)
    {
        if (indx != INVALID_TEXTURE_ID)
        {
            glDeleteTextures(1, &indx);
        }

        return TRUE;
    }

    extern "C" u32 AcquireTextureInformation(const u32) { return DEFAULT_TEXTURE_CACHE_CAPACITY; } // NOTE: Not being called by the application.

    extern "C" u32 ResetTextureCache(void)
    {
        State.GL.Textures.TextureCount = 0;

        for (u32 x = 0; x < MAX_TEXTURE_COUNT; x++)
        {
            auto id = State.GL.Textures.Textures[x];

            if (id != INVALID_TEXTURE_ID)
            {
                glDeleteTextures(1, &id);
            }
        }

        return TRUE;
    }

    extern "C" u32 SelectCubeTexture(const u32, const u32) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SelectRenderTexture(const u32 indx)
    {
        State.GL.Textures.Selected.Textures[0] = State.GL.Textures.Render.Textures[indx].ID;

        return TRUE;
    }

    extern "C" u32 SelectTexture(const u32 id, const u32 indx)
    {
        if (MAX_SELECTED_TEXTURE_COUNT <= indx) { return FALSE; }

        State.GL.Textures.Selected.Textures[indx] = id;

        return TRUE;
    }

    extern "C" u32 UploadTexture(const u32 indx, const Texture * texture)
    {
        GL::ConvertTexture(texture);

        GL::ProcessTextureMipLevels(texture);

        glBindTexture(GL_TEXTURE_2D, indx);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        const auto levels = texture->Definition.MipLevelCount + 1;

        auto dims = texture->Definition.Width;

        for (u32 x = 0; x < levels; x++)
        {
            auto pixels = GL::AcquireTexturePixels(texture, dims);

            glTexImage2D(GL_TEXTURE_2D, x, GL_RGBA8, dims, dims, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

            dims /= 2;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels);

        return TRUE;
    }
}