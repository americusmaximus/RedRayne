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
#include "Renderers.Basic.Vertexes.hxx"

#define DIRECT3D_VERSION 0x0900
#include <d3d9.h>

#define D3DUSAGE_NONE 0
#define D3DPRESENT_NONE 0

#define DEFAULT_TEXTURE_CACHE_CAPACITY 0

#define MAX_CUBE_TEXTURE_COUNT 100
#define MAX_PALETTE_BUFFER_SIZE 256
#define MAX_PIXEL_BUFFER_SIZE (1024 * 1024 + 512 * 512 + 256 * 256 + 128 * 128 + 64 * 64 + 32 * 32 + 16 * 16 + 8 * 8 + 4 * 4 + 2 * 2 + 1 * 1)
#define MAX_RENDER_TEXTURE_COUNT 100

#define MAX_SURFACE_STAGE_COUNT 8
#define MAX_SURFACE_STAGE_VALUES_COUNT 255

#define MAX_SURFACE_SAMPLER_COUNT 8
#define MAX_SURFACE_SAMPLER_VALUES_COUNT 255

#define MAX_TEXTURE_COUNT 4096

#define MAX_VERTEX_SHADER_COUNT 33
#define MAX_PIXEL_SHADER_COUNT 9

#define MAX_WINDOW_COUNT 6
#define INVALID_WINDOW_INDEX 0
#define MIN_WINDOW_INDEX 1

#define SHADER_REGISTER_0_INDEX 0
#define SHADER_REGISTER_1_INDEX 1
#define SHADER_REGISTER_2_INDEX 2
#define SHADER_REGISTER_3_INDEX 3
#define SHADER_REGISTER_4_INDEX 4
#define SHADER_REGISTER_5_INDEX 5
#define SHADER_REGISTER_6_INDEX 6
#define SHADER_REGISTER_7_INDEX 7
#define SHADER_REGISTER_8_INDEX 8
#define SHADER_REGISTER_9_INDEX 9
#define SHADER_REGISTER_10_INDEX 10
#define SHADER_REGISTER_11_INDEX 11
#define SHADER_REGISTER_12_INDEX 12
#define SHADER_REGISTER_13_INDEX 13
#define SHADER_REGISTER_14_INDEX 14
#define SHADER_REGISTER_15_INDEX 15
#define SHADER_REGISTER_16_INDEX 16
#define SHADER_REGISTER_17_INDEX 17
#define SHADER_REGISTER_18_INDEX 18
#define SHADER_REGISTER_19_INDEX 19
#define SHADER_REGISTER_20_INDEX 20
#define SHADER_REGISTER_21_INDEX 21
#define SHADER_REGISTER_22_INDEX 22
#define SHADER_REGISTER_23_INDEX 23
#define SHADER_REGISTER_24_INDEX 24
#define SHADER_REGISTER_25_INDEX 25
#define ACTUAL_SHADER_REGISTER_CONSTANT_COUNT 25

#define MAX_PIXEL_SHADER_REGISTER_COUNT 2

#define MAX_SHADER_REGISTER_CONSTANT_COUNT 100

#define MAX_STATE_VARIABLE_COUNT 256

#define DEFAULT_BUFFER_CAPACITY 65000
#define MINIMAL_BUFFER_CAPACITY 1000

#define MAX_BLEND_TEXTURE_COUNT 4

#define TEXTURE_STAGE_0_INDEX 0
#define TEXTURE_STAGE_1_INDEX 1
#define TEXTURE_STAGE_2_INDEX 2
#define TEXTURE_STAGE_3_INDEX 3
#define TEXTURE_STAGE_4_INDEX 4

#define STREAM_SOURCE_0_INDEX 0
#define STREAM_SOURCE_1_INDEX 1

#define D3DCLEAR_NONE 0
#define D3DLOCK_NONE 0

#define DEFAULT_REGULAR_TEXTURE_SIZE 128
#define DEFAULT_CUBE_TEXTURE_SIZE 256


#define DEFAULT_TEXTURE_RENDER_TARGET_INDEX 0

namespace DX::Shaders::Pixel
{
    extern const char NAME_BUMP[];
    extern const char NAME_BUMPSPEC[];
    extern const char NAME_DECAL[];
    extern const char NAME_DECALKIL[];
    extern const char NAME_PBMPCUBE[];
    extern const char NAME_PBMPSPLM[];
    extern const char NAME_PGLOSS[];
    extern const char NAME_PLMAP[];
    extern const char NAME_PLMBS[];

    extern const DWORD FUNC_BUMP[];
    extern const DWORD FUNC_BUMPSPEC[];
    extern const DWORD FUNC_DECAL[];
    extern const DWORD FUNC_DECALKIL[];
    extern const DWORD FUNC_PBMPCUBE[];
    extern const DWORD FUNC_PBMPSPLM[];
    extern const DWORD FUNC_PGLOSS[];
    extern const DWORD FUNC_PLMAP[];
    extern const DWORD FUNC_PLMBS[];
}

namespace DX::Shaders::Vertex
{
    extern const D3DVERTEXELEMENT9 DECLARATION_XYZ_NORMAL_UV[];
    extern const D3DVERTEXELEMENT9 DECLARATION_XYZ_DIFFUSE_UV[];
    extern const D3DVERTEXELEMENT9 DECLARATION_XYZ_UV3_DIFFUSE_UV4_UV[];
    extern const D3DVERTEXELEMENT9 DECLARATION_XYZRHW_DIFFUSE_SPECULAR_UV[];
    extern const D3DVERTEXELEMENT9 DECLARATION_SKELETON[];
    extern const D3DVERTEXELEMENT9 DECLARATION_BASIS[];

    extern const char NAME_CUBIC[];
    extern const char NAME_DIRLIGHT[];
    extern const char NAME_PRELIT[];
    extern const char NAME_PRETEX[];
    extern const char NAME_SHDWBKFC[];
    extern const char NAME_TEXGEN[];
    extern const char NAME_TLVERTEX[];
    extern const char NAME_VBMPCUBE[];
    extern const char NAME_VBMPSPEC[];
    extern const char NAME_VBMPSPGN[];
    extern const char NAME_VBMPSPOM[];
    extern const char NAME_VBMPSPPL[];
    extern const char NAME_VBUMP[];
    extern const char NAME_VBUMPGEN[];
    extern const char NAME_VGLOSS[];
    extern const char NAME_VLMAP[];
    extern const char NAME_VLMAP2[];
    extern const char NAME_VLMBS[];
    extern const char NAME_VOMNI[];
    extern const char NAME_VOMNIBMP[];
    extern const char NAME_VOMNIN[];
    extern const char NAME_VSKEL[];
    extern const char NAME_VSKELBMP[];
    extern const char NAME_VSKELSPC[];
    extern const char NAME_VSKELSPC2[];
    extern const char NAME_VSKELSPC3[];
    extern const char NAME_VSKELTEX[];
    extern const char NAME_VSPECMAP[];

    extern const DWORD FUNC_CUBIC[];
    extern const DWORD FUNC_DIRLIGHT_XYZ_NORMAL_UV[];
    extern const DWORD FUNC_DIRLIGHT_BASIS[];
    extern const DWORD FUNC_PRELIT_XYZ_UV3_DIFFUSE_UV4_UV[];
    extern const DWORD FUNC_PRELIT_XYZ_DIFFUSE_UV[];
    extern const DWORD FUNC_PRETEX[];
    extern const DWORD FUNC_SHDWBKFC_XYZ_NORMAL_UV[];
    extern const DWORD FUNC_SHDWBKFC_BASIS[];
    extern const DWORD FUNC_TEXGEN[];
    extern const DWORD FUNC_VBMPCUBE[];
    extern const DWORD FUNC_VBMPSPEC[];
    extern const DWORD FUNC_VBMPSPGN[];
    extern const DWORD FUNC_VBMPSPOM[];
    extern const DWORD FUNC_VBMPSPPL[];
    extern const DWORD FUNC_VBUMP[];
    extern const DWORD FUNC_VBUMPGEN[];
    extern const DWORD FUNC_VGLOSS[];
    extern const DWORD FUNC_VLMAP[];
    extern const DWORD FUNC_VLMAP2[];
    extern const DWORD FUNC_VLMBS[];
    extern const DWORD FUNC_VOMNI_XYZ_UV3_DIFFUSE_UV4_UV[];
    extern const DWORD FUNC_VOMNI_XYZ_DIFFUSE_UV[];
    extern const DWORD FUNC_VOMNI_BASIS[];
    extern const DWORD FUNC_VOMNIBMP[];
    extern const DWORD FUNC_VOMNIN[];
    extern const DWORD FUNC_VSKEL[];
    extern const DWORD FUNC_VSKELBMP[];
    extern const DWORD FUNC_VSKELSPC[];
    extern const DWORD FUNC_VSKELSPC2[];
    extern const DWORD FUNC_VSKELSPC3[];
    extern const DWORD FUNC_VSKELTEX[];
    extern const DWORD FUNC_VSPECMAP[];
}

namespace DX
{
    enum class DXPSN : u32
    {
        Decal = 0,
        DecalTexKill = 1,
        Bump = 2,
        BumpSpec = 3,
        BumpSpecLightMap = 4,
        BumpCubic = 5,
        Gloss = 6,
        LightMap = 7,
        Plmbs = 8
    };

    enum class DXVSN : u32
    {
        Dirlight1 = 0,
        Texgen = 1,
        Cubic = 2,
        Prelit1 = 3,
        Prelit2 = 4,
        Pretex = 5,
        Vbump = 6,
        Vbmpspec = 7,
        Vbmpcube = 8,
        Vbumpgen = 9,
        Vbmpspgn = 10,
        Vbmpsppl = 11,
        Vbmpspom = 12,
        Vgloss = 13,
        Vlmap1 = 14,
        Vlmap2 = 15,
        Vskel = 16,
        Vskeltex = 17,
        Vomni1 = 18,
        Vomni2 = 19,
        Vomnin = 20,
        Vomnibmp = 21,
        Tlvertex = 22,
        Shdwbkfc1 = 23,
        Vspecmap = 24,
        Vskelspc = 25,
        Vskelsp2 = 26,
        Vskelsp3 = 27,
        Vskelbmp = 28,
        Vomni3 = 29,
        Dirlight2 = 30,
        Shdwbkfc2 = 31,
        Vlmbs = 32
    };

    struct DXDevice
    {
        u32 Index;

        u32 DeviceID;
        u32 VendorID;

        char Driver[MAX_DEVICE_IDENTIFIER_STRING];
        char Description[MAX_DEVICE_IDENTIFIER_STRING];
    };

    struct DXTexture
    {
        Assets::TextureFormat Format;

        u32 Width;
        u32 Height;
        u32 Options;
        u32 MipLevelCount;

        u16 ActualWidth;
        u16 ActualHeight;
        u32 ActualMipLevelCount;

        IDirect3DTexture9* Texture;
    };

    struct DXWindow
    {
        u32 Width;
        u32 Height;
        u32 BitsPerPixel;

        struct
        {
            u32** Pixels;
            u32* Values[MAX_GRAPHICS_RESOLUTION_HEIGHT];
        }
        Pixels;

        HWND HWND;
        IDirect3DSurface9* DepthSurface;
        IDirect3DSwapChain9* SwapChain;
    };

    struct DXVS
    {
        struct
        {
            const D3DVERTEXELEMENT9* Declaration;
            const DWORD* Function;
        } Definition;

        const char* Name;
        const DWORD FVF;

        struct
        {
            IDirect3DVertexDeclaration9* Declaration;
            IDirect3DVertexShader9* Shader;
        } Shader;
    };

    struct DXPS
    {
        const DWORD* Function;
        const char* Name;
        IDirect3DPixelShader9* Shader;
    };

    struct DXState
    {
        struct
        {
            IDirect3D9* Instance;
            IDirect3DDevice9* Device;

            struct
            {
                DWORD Values[MAX_STATE_VARIABLE_COUNT];
            } State;
        } DirectX;

        struct
        {
            struct
            {
                u32 Count;
                DXDevice Details[MAX_DEVICE_COUNT];
            } Available;

            struct
            {
                u32 Index;

                D3DCAPS9 Capabilities;
                D3DPRESENT_PARAMETERS PresentParams;
                D3DPRESENT_PARAMETERS SwapChainParams;
            } Current;
        } Devices;

        struct
        {
            Graphics::f32xRGB Color;

            f32 Start, End;
        } Fog;

        struct
        {
            BOOL IsChanged = TRUE;
            Renderers::RendererMode Mode;

            D3DCULL Cull = D3DCULL::D3DCULL_CCW;
            D3DZBUFFERTYPE DepthBufferMode = D3DZBUFFERTYPE::D3DZB_TRUE;

            struct
            {
                D3DBLEND SourceBlendMode = D3DBLEND::D3DBLEND_SRCALPHA;
                D3DBLEND DestinationBlendMode = D3DBLEND::D3DBLEND_INVSRCALPHA;

                Renderers::RendererBlendOperation BlendOperation;
                D3DBLENDOP DestinationBlendOperation = D3DBLENDOP::D3DBLENDOP_ADD;
            } Blend;

            struct
            {
                f32 Near = DEFAULT_RENDERER_CLIPPING_NEAR;
                f32 Far = DEFAULT_RENDERER_CLIPPING_FAR;;
            } Clipping;

            struct
            {
                Renderers::RendererBlendOperation ClampOperation = Renderers::RendererBlendOperation::All;
                Renderers::RendererClampMode U[MAX_SURFACE_STAGE_COUNT];
                Renderers::RendererClampMode V[MAX_SURFACE_STAGE_COUNT];
            } Clamp;
        } Mode;

        struct
        {
            Renderers::RendererLightingMode Mode = Renderers::RendererLightingMode::On;

            f32 Alpha;

            struct
            {
                f32 R[MAX_LIGHT_COUNT] = { 1.0f, 0.0f, 0.0f };
                f32 G[MAX_LIGHT_COUNT] = { 1.0f, 0.0f, 0.0f };
                f32 B[MAX_LIGHT_COUNT] = { 1.0f, 0.0f, 0.0f };
            } Colors;

            D3DLIGHT9 Lights[MAX_LIGHT_COUNT];

            Graphics::f32xRGB Color;

            f32x3 XYZ[MAX_LIGHT_COUNT];
        } Light;

        struct
        {
            BOOL IsActive;
        } Lock;

        struct
        {
            BOOL IsActive;
        } Scene;

        struct
        {
            struct
            {
                u32 Count;
                IDirect3DIndexBuffer9* Buffer;
            } Index;

            struct
            {
                u32 Count;
                IDirect3DVertexBuffer9* Buffer;
            } Main;

            struct
            {
                u32 Count;
                IDirect3DVertexBuffer9* Buffer;
            } RLVX;

            struct
            {
                u32 Count;
                IDirect3DVertexBuffer9* Buffer;
            } RTLVX;

            struct
            {
                u32 Count;
                IDirect3DVertexBuffer9* Buffer;
            } Basis;
        } Buffers;

        struct
        {
            struct
            {
                D3DFORMAT Transparent;
                Assets::TextureFormat Surface;
            } Formats;

            struct
            {
                IDirect3DSurface9* Surface;
                IDirect3DSurface9* DepthSurface;
            } Target;

            IDirect3DSurface9* Transparent;
            IDirect3DSurface9* Opaque;

            struct
            {
                IDirect3DTexture9* Textures[MAX_SURFACE_STAGE_COUNT];
                u32 Stages[MAX_SURFACE_STAGE_COUNT][MAX_SURFACE_STAGE_VALUES_COUNT];
                DWORD Sampling[MAX_SURFACE_SAMPLER_COUNT][MAX_SURFACE_SAMPLER_VALUES_COUNT];
            } Staging;
        } Surfaces;

        struct
        {
            u16 Indexes[MINIMAL_BUFFER_CAPACITY];

            struct
            {
                Renderers::RTLVX Min[MINIMAL_BUFFER_CAPACITY];
                Renderers::RTLVX RTLVX[RENDERER_TEN_THOUSAND_ELEMENT_COUNT];
            } RTLVX;
        } Data;

        struct
        {
            /// <summary>
            /// Texture 32-bit ARGB pixel colors palette.
            /// Note: used during the texture uploading, and is being overwritten each time new texture is uploaded.
            /// </summary>
            u32 Colors[MAX_PALETTE_BUFFER_SIZE];

            /// <summary>
            /// Texture 32-bit ARGB pixel colors for all mip levels.
            /// Note: used during the texture uploading, and is being overwritten each time new texture is uploaded.
            /// </summary>
            u32 Pixels[MAX_PIXEL_BUFFER_SIZE];
        } Pixels;

        struct
        {
            BOOL IsCahnged = TRUE;

            f32 Near, Far;

            Mathematics::Matrix4x4 ModelView;

            Mathematics::Matrix4x4 Projection;

            Mathematics::Matrix4x4 O2W;

            Mathematics::Matrix4x4 W2L;

            const Mathematics::Matrix4x4 Identity
            {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };
        } Transform;

        struct
        {
            DXTexture Textures[MAX_TEXTURE_COUNT];

            struct
            {
                IDirect3DTexture9* Staging[MAX_SURFACE_STAGE_COUNT];
                IDirect3DTexture9* Textures[MAX_RENDER_TEXTURE_COUNT];
            } Surfaces;
        } Textures;

        struct
        {
            DX::DXWindow* Current;
            DX::DXWindow* Windows[MAX_WINDOW_COUNT];
        } Windows;

        struct
        {
            struct
            {
                // NOTE: Only ACTUAL_SHADER_REGISTER_CONSTANT_COUNT are used in the renderer directly.
                // NOTE: The bone data is passed in the registers over ACTUAL_SHADER_REGISTER_CONSTANT_COUNT.
                f32x4 Actual[MAX_SHADER_REGISTER_CONSTANT_COUNT];
                f32x4 Staging[MAX_SHADER_REGISTER_CONSTANT_COUNT];
            } Constants;

            DXVS VertexShaders[MAX_VERTEX_SHADER_COUNT]
            {
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                        .Function = DX::Shaders::Vertex::FUNC_DIRLIGHT_XYZ_NORMAL_UV
                    },
                    .Name = DX::Shaders::Vertex::NAME_DIRLIGHT,
                    .FVF = D3DFVF_TEX2 | D3DFVF_NORMAL | D3DFVF_XYZ,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                        .Function = DX::Shaders::Vertex::FUNC_TEXGEN
                    },
                    .Name = DX::Shaders::Vertex::NAME_TEXGEN,
                    .FVF = D3DFVF_TEX2 | D3DFVF_NORMAL | D3DFVF_XYZ,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                        .Function = DX::Shaders::Vertex::FUNC_CUBIC
                    },
                    .Name = DX::Shaders::Vertex::NAME_CUBIC,
                    .FVF = D3DFVF_TEX2 | D3DFVF_NORMAL | D3DFVF_XYZ,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_UV3_DIFFUSE_UV4_UV,
                        .Function = DX::Shaders::Vertex::FUNC_PRELIT_XYZ_UV3_DIFFUSE_UV4_UV
                    },
                    .Name = DX::Shaders::Vertex::NAME_PRELIT,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_DIFFUSE_UV,
                        .Function = DX::Shaders::Vertex::FUNC_PRELIT_XYZ_DIFFUSE_UV
                    },
                    .Name = DX::Shaders::Vertex::NAME_PRELIT,
                    .FVF = D3DFVF_TEX1 | D3DFVF_DIFFUSE | D3DFVF_XYZ,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_UV3_DIFFUSE_UV4_UV,
                        .Function = DX::Shaders::Vertex::FUNC_PRETEX
                    },
                    .Name = DX::Shaders::Vertex::NAME_PRETEX,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_VBUMP
                    },
                    .Name = DX::Shaders::Vertex::NAME_VBUMP,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_VBMPSPEC
                    },
                    .Name = DX::Shaders::Vertex::NAME_VBMPSPEC,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_VBMPCUBE,
                    },
                    .Name = DX::Shaders::Vertex::NAME_VBMPCUBE,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_VBUMPGEN
                    },
                    .Name = DX::Shaders::Vertex::NAME_VBUMPGEN,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_VBMPSPGN
                    },
                    .Name = DX::Shaders::Vertex::NAME_VBMPSPGN,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_VBMPSPPL
                    },
                    .Name = DX::Shaders::Vertex::NAME_VBMPSPPL,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_VBMPSPOM
                    },
                    .Name = DX::Shaders::Vertex::NAME_VBMPSPOM,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                        .Function = DX::Shaders::Vertex::FUNC_VGLOSS
                    },
                    .Name = DX::Shaders::Vertex::NAME_VGLOSS,
                    .FVF = D3DFVF_TEX2 | D3DFVF_NORMAL | D3DFVF_XYZ,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_VLMAP
                    },
                    .Name = DX::Shaders::Vertex::NAME_VLMAP,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                        .Function = DX::Shaders::Vertex::FUNC_VLMAP2
                    },
                    .Name = DX::Shaders::Vertex::NAME_VLMAP2,
                    .FVF = D3DFVF_TEX2 | D3DFVF_NORMAL | D3DFVF_XYZ,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_SKELETON,
                        .Function = DX::Shaders::Vertex::FUNC_VSKEL
                    },
                    .Name = DX::Shaders::Vertex::NAME_VSKEL,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_SKELETON,
                        .Function = DX::Shaders::Vertex::FUNC_VSKELTEX
                    },
                    .Name = DX::Shaders::Vertex::NAME_VSKELTEX,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_UV3_DIFFUSE_UV4_UV,
                        .Function = DX::Shaders::Vertex::FUNC_VOMNI_XYZ_UV3_DIFFUSE_UV4_UV
                    },
                    .Name = DX::Shaders::Vertex::NAME_VOMNI,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_DIFFUSE_UV,
                        .Function = DX::Shaders::Vertex::FUNC_VOMNI_XYZ_DIFFUSE_UV
                    },
                    .Name = DX::Shaders::Vertex::NAME_VOMNI,
                    .FVF = D3DFVF_TEX1 | D3DFVF_DIFFUSE | D3DFVF_XYZ,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                        .Function = DX::Shaders::Vertex::FUNC_VOMNIN
                    },
                    .Name = DX::Shaders::Vertex::NAME_VOMNIN,
                    .FVF = D3DFVF_TEX2 | D3DFVF_NORMAL | D3DFVF_XYZ,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_VOMNIBMP
                    },
                    .Name = DX::Shaders::Vertex::NAME_VOMNIBMP,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZRHW_DIFFUSE_SPECULAR_UV,
                        .Function = NULL
                    },
                    .Name = DX::Shaders::Vertex::NAME_TLVERTEX,
                    .FVF = D3DFVF_TEX1 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                        .Function = DX::Shaders::Vertex::FUNC_SHDWBKFC_XYZ_NORMAL_UV
                    },
                    .Name = DX::Shaders::Vertex::NAME_SHDWBKFC,
                    .FVF = D3DFVF_TEX2 | D3DFVF_NORMAL | D3DFVF_XYZ,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_XYZ_NORMAL_UV,
                        .Function = DX::Shaders::Vertex::FUNC_VSPECMAP
                    },
                    .Name = DX::Shaders::Vertex::NAME_VSPECMAP,
                    .FVF = D3DFVF_TEX2 | D3DFVF_NORMAL | D3DFVF_XYZ,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_SKELETON,
                        .Function = DX::Shaders::Vertex::FUNC_VSKELSPC
                    },
                    .Name = DX::Shaders::Vertex::NAME_VSKELSPC,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_SKELETON,
                        .Function = DX::Shaders::Vertex::FUNC_VSKELSPC2
                    },
                    .Name = DX::Shaders::Vertex::NAME_VSKELSPC2,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_SKELETON,
                        .Function = DX::Shaders::Vertex::FUNC_VSKELSPC3
                    },
                    .Name = DX::Shaders::Vertex::NAME_VSKELSPC3,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_SKELETON,
                        .Function = DX::Shaders::Vertex::FUNC_VSKELBMP
                    },
                    .Name = DX::Shaders::Vertex::NAME_VSKELBMP,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_VOMNI_BASIS
                    },
                    .Name = DX::Shaders::Vertex::NAME_VOMNI,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_DIRLIGHT_BASIS
                    },
                    .Name = DX::Shaders::Vertex::NAME_DIRLIGHT,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_SHDWBKFC_BASIS
                    },
                    .Name = DX::Shaders::Vertex::NAME_SHDWBKFC,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                },
                {
                    .Definition =
                    {
                        .Declaration = DX::Shaders::Vertex::DECLARATION_BASIS,
                        .Function = DX::Shaders::Vertex::FUNC_VLMBS
                    },
                    .Name = DX::Shaders::Vertex::NAME_VLMBS,
                    .FVF = D3DFVF_TEX0,
                    .Shader = {.Declaration = NULL, .Shader = NULL}
                }
            };

            DXPS PixelShaders[MAX_PIXEL_SHADER_COUNT]
            {
                {
                    .Function = DX::Shaders::Pixel::FUNC_DECAL,
                    .Name = DX::Shaders::Pixel::NAME_DECAL,
                    .Shader = NULL
                },
                {
                    .Function = DX::Shaders::Pixel::FUNC_DECALKIL,
                    .Name = DX::Shaders::Pixel::NAME_DECALKIL,
                    .Shader = NULL
                },
                {
                    .Function = DX::Shaders::Pixel::FUNC_BUMP,
                    .Name = DX::Shaders::Pixel::NAME_BUMP,
                    .Shader = NULL
                },
                {
                    .Function = DX::Shaders::Pixel::FUNC_BUMPSPEC,
                    .Name = DX::Shaders::Pixel::NAME_BUMPSPEC,
                    .Shader = NULL
                },
                {
                    .Function = DX::Shaders::Pixel::FUNC_PBMPSPLM,
                    .Name = DX::Shaders::Pixel::NAME_PBMPSPLM,
                    .Shader = NULL
                },
                {
                    .Function = DX::Shaders::Pixel::FUNC_PBMPCUBE,
                    .Name = DX::Shaders::Pixel::NAME_PBMPCUBE,
                    .Shader = NULL
                },
                {
                    .Function = DX::Shaders::Pixel::FUNC_PGLOSS,
                    .Name = DX::Shaders::Pixel::NAME_PGLOSS,
                    .Shader = NULL
                },
                {
                    .Function = DX::Shaders::Pixel::FUNC_PLMAP,
                    .Name = DX::Shaders::Pixel::NAME_PLMAP,
                    .Shader = NULL
                },
                {
                    .Function = DX::Shaders::Pixel::FUNC_PLMBS,
                    .Name = DX::Shaders::Pixel::NAME_PLMBS,
                    .Shader = NULL
                }
            };
        } Shaders;
    };

    void DXC(const HRESULT, const char*);

    BOOL InitializeState(void);
    BOOL ResetDevice(const u32, const u32, const u32, u32**, DX::DXWindow*);
    BOOL SelectLight(void);
    u32 Alpha(const f32);
    u32 Color(const f32, const f32, const f32, const f32);
    void Initialize(void);
    void Release(void);
    void SelectRendererMode(const Renderers::RendererMode);
    void SelectTextureStageValue(const u32, const D3DTEXTURESTAGESTATETYPE, const DWORD);
    void SelectSamplerState(const u32 sampler, const D3DSAMPLERSTATETYPE type, const DWORD value);
    void SetBlendShaders(const Renderers::RendererMode);
    void SetIndexes(const u16*, const u32);
    void SetLightShaders(const Renderers::RendererMode);
    void SetRenderState(const D3DRENDERSTATETYPE, const DWORD);
    void SetShaderConstants(void);
    void SetShaders(const DXVSN, const DXPSN);
    void SetShadersMode(const Renderers::RendererMode);
    void SetSkeletonShaders(const Renderers::RendererMode);
    void SetRTLVXShaders(const Renderers::RendererMode);

    void Initialize(DXWindow*);
    void Initialize(DXWindow*, D3DPRESENT_PARAMETERS*);
    BOOL Initialize(const u32, const u32, const u32);
}