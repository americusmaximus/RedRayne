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

#include "Assets.Basic.hxx"
#include "Graphics.Basic.hxx"
#include "Mathematics.Basic.hxx"
#include "Native.Basic.hxx"
#include "Renderers.Basic.hxx"

#include <gl/GL.h>

#define DEFAULT_TEXTURE_CACHE_CAPACITY 0

#define GL_CLAMP_TO_EDGE                    0x812F
#define GL_MAX_TEXTURE_UNITS                0x84E2
#define GL_TEXTURE_MAX_LEVEL                0x813D
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define WGL_BIND_TO_TEXTURE_RGB_ARB         0x2070
#define WGL_DEPTH_BITS_ARB                  0x2022
#define WGL_DOUBLE_BUFFER_ARB               0x2011
#define WGL_DRAW_TO_PBUFFER_EXT             0x202D
#define WGL_FRONT_LEFT_ARB                  0x2083
#define WGL_MIPMAP_TEXTURE_ARB              0x2074
#define WGL_PBUFFER_HEIGHT_EXT              0x2035
#define WGL_PBUFFER_LARGEST_EXT             0x2033
#define WGL_PBUFFER_WIDTH_EXT               0x2034
#define WGL_TEXTURE_2D_ARB                  0x207A
#define WGL_TEXTURE_FORMAT_ARB              0x2072
#define WGL_TEXTURE_RGB_ARB                 0x2075
#define WGL_TEXTURE_TARGET_ARB              0x2073

#define MAX_BIND_TEXTURE_COUNT 2
#define MAX_PALETTE_BUFFER_SIZE 256
#define MAX_PIXEL_BUFFER_SIZE (1024 * 1024 + 512 * 512 + 256 * 256 + 128 * 128 + 64 * 64 + 32 * 32 + 16 * 16 + 8 * 8 + 4 * 4 + 2 * 2 + 1 * 1)
#define MAX_RENDER_TEXTURE_COUNT 8
#define MAX_SELECTED_TEXTURE_COUNT 8
#define MAX_TEXTURE_COUNT 5000
#define MAX_VERTEX_COUNT 5000

#define MAX_LIGHT_COLOR_COUNT 8

namespace GL
{
    DECLARE_HANDLE(HPBUFFERARB);

    typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGARBPROC) (const HDC);
    typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
    typedef HDC(WINAPI* PFNWGLGETPBUFFERDCARBPROC) (const HPBUFFERARB);
    typedef HPBUFFERARB(WINAPI* PFNWGLCREATEPBUFFERARBPROC) (const HDC, const s32, const s32, const s32, const s32*);
    typedef s32(WINAPI* PFNWGLBINDTEXIMAGEARBPROC) (const HPBUFFERARB, const s32);
    typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC) (const HDC, const s32*, const f32*, const u32, s32*, u32*);
    typedef s32(WINAPI* PFNWGLDESTROYPBUFFERARBPROC) (const HPBUFFERARB);
    typedef s32(WINAPI* PFNWGLQUERYPBUFFERARBPROC) (const HPBUFFERARB, const s32, u32*);
    typedef s32(WINAPI* PFNWGLRELEASEPBUFFERDCARBPROC) (const HPBUFFERARB, const HDC);
    typedef s32(WINAPI* PFNWGLRELEASETEXIMAGEARBPROC) (const HPBUFFERARB, const s32);
    typedef s32(WINAPI* PFNWGLSETPBUFFERATTRIBARBPROC) (const HPBUFFERARB, const s32*);
    typedef void (WINAPI* PFNGLACTIVETEXTUREARBPROC) (const u32);
    typedef void (WINAPI* PFNGLCLIENTACTIVETEXTUREARBPROC) (const u32);
    typedef void (WINAPI* PFNGLMULTITEXCOORD2FARBPROC)(const u32, const f32, const f32);

    struct GLTexture
    {
        u32 ID;
        HPBUFFERARB Buffer;
        HDC DeviceContext;
        HGLRC RenderContext;
        u32 Width;
        u32 Height;
    };

    struct GLLight
    {
        f32x4 Position;
        Graphics::f32xRGBA Color;
    };

    struct GLState
    {
        struct
        {
            Renderers::RendererCullMode Cull;

            struct
            {
                f32 Near, Far;
            } Clipping;

            u32 SourceBlendMode = GL_SRC_ALPHA;
            u32 DestinationBlendMode = GL_ONE_MINUS_SRC_ALPHA;
        } Mode;

        struct
        {
            Renderers::RendererBlendOperation MultiBlendOperation;

            struct
            {
                u32 Textures[MAX_BIND_TEXTURE_COUNT];
            } Bind;

            struct
            {
                Renderers::RendererClampMode S = Renderers::RendererClampMode::Wrap;
                Renderers::RendererClampMode T = Renderers::RendererClampMode::Wrap;
            } Clamp;

            struct
            {
                u32 Textures[MAX_SELECTED_TEXTURE_COUNT];
            } Selected;

            struct
            {
                BOOL IsActive = FALSE;

                u32 Index;
                f32 Direction;

                GLTexture Textures[MAX_RENDER_TEXTURE_COUNT];
            } Render;

            struct
            {
                /// <summary>
                /// Texture 32-bit ARGB pixel colors palette.
                /// Used during the texture uploading, and is being overwritten each time new texture is uploaded.
                /// </summary>
                u32 Colors[MAX_PALETTE_BUFFER_SIZE];

                /// <summary>
                /// Texture 32-bit BGRA pixel colors for all mip levels.
                /// Used during the texture uploading, and is being overwritten each time new texture is uploaded.
                /// </summary>
                u32 Pixels[MAX_PIXEL_BUFFER_SIZE];
            } Buffers;

            u32 TextureCount;
            u32 Textures[MAX_TEXTURE_COUNT];
        } Textures;

        struct
        {
            Renderers::RendererLightingMode Mode = Renderers::RendererLightingMode::Off;

            struct
            {
                f32 Alpha = 1.0f;

                f32 R[MAX_LIGHT_COLOR_COUNT];
                f32 G[MAX_LIGHT_COLOR_COUNT];
                f32 B[MAX_LIGHT_COLOR_COUNT];
            } Colors;

            GLLight Lights[MAX_LIGHT_COUNT];
        } Light;

        struct
        {
            Mathematics::Matrix4x4 ModelView;
            Mathematics::Matrix4x4 Projection;
        } Transform;

        struct
        {
            Graphics::f32xRGBA Colors[MAX_VERTEX_COUNT];
        } Vertexes;
    };

    void Initialize(void);
    void Release(void);
    void SelectRendererMode(const Renderers::RendererMode);

    struct GLFState
    {
        PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT;
        PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
        PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
        PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
        PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
        PFNWGLBINDTEXIMAGEARBPROC wglBindTexImageARB;
        PFNWGLRELEASETEXIMAGEARBPROC wglReleaseTexImageARB;
        PFNWGLSETPBUFFERATTRIBARBPROC wglSetPbufferAttribARB;
        PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
        PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB;
        PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB;
        PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB;
        PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
        PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB;
    };

    extern struct GLFState GLF;
}