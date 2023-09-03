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

#include "Graphics.Basic.hxx"
#include "Mathematics.Basic.hxx"

#define MAX_DEVICE_COUNT 16

#define MAX_LIGHT_COUNT 3

#define MAX_COLOR_TABLE_BUFFER_SIZE 256

#define INVALID_TEXTURE_INDEX 0
#define MIN_VALID_TEXTURE_INDEX 1

#define RENDERER_TEN_THOUSAND_ELEMENT_COUNT 10000

#define DEFAULT_RENDERER_CLIPPING_NEAR (1.0f)
#define DEFAULT_RENDERER_CLIPPING_FAR (6400.0f)

namespace Renderers
{
    enum class RendererAcceleratedGraphicsPortMode : u32
    {
        Auto = 0,
        X1 = 1,
        X2 = 2,
        X4 = 3,
        X8 = 4
    };

    // TODO: Names.
    enum class RendererBlendOperation : u32
    {
        None = 0,
        Disable = 1,
        SelectArgument1 = 2,
        SelectArgument2 = 3,
        Modulate = 4,
        Modulate2X = 5,
        Modulate4X = 6,
        Add = 7,
        AddSigned = 8,
        AddSigned2X = 9,
        Subtract = 10,
        AddSmooth = 11,
        BlendDiffuseAlpha = 12,
        BlendTextureAlpha = 13,
        BlendFactorAlpha = 14,
        BlendTextureAlphaPreMultiplied = 15,
        BlendCurrentAlpha = 16,
        PreModulate = 17,
        ModulateAlphaAddColor = 18,
        ModulateColorAddAlpha = 19,
        ModulateInverseAlphaAddColor = 20,
        ModulateInverseColorAddAlpha = 21,
        BumpEnvMap = 22,
        BumpEnvMapLuminance = 23,
        DotProduct3 = 24,
        MultiplyAdd = 25,
        Lerp = 26,
        All = U32_MAX
    };

    OPTIONS(RendererClearMode, u32)
    {
        None = 0x0,
        Target = 0x1,
        Depth = 0x2,
        Stencil = 0x4,
        Unknown = 0x8,
        ClearColor = 0x10,
        FogColor = 0x20
    };

    OPTIONS(RendererClippingMode, u32)
    {
        Off = 0x0,
        On = 0x1,
        Unknown2 = 0x2,
        Unknown4 = 0x4,
        Unknown8 = 0x8,
        Unknown10 = 0x10,
        Unknown20 = 0x20
    };

    enum class RendererClampMode : u32
    {
        Wrap = 0,
        Clamp = 1
    };

    enum class RendererCubeMapSide : u32
    {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5
    };

    enum class RendererCullMode : u32
    {
        CCW = 0,
        CW = 1,
        None = 2
    };

    enum class RendererDestinationBlend : u32
    {
        InverseAlpha = 0,
        One = 1,
        Color = 2,
        InverseColor = 3,
        Zero = 4,
        NegativeOne = 5
    };

    OPTIONS(RendererMode, u32)
    {
        None = 0x0,
        U0x1 = 0x1,
        U0x2 = 0x2,
        U0x4 = 0x4,
        U0x8 = 0x8,
        U0x10 = 0x10,
        U0x20 = 0x20,
        U0x40 = 0x40,
        U0x80 = 0x80,
        U0x100 = 0x100,
        U0x200 = 0x200,
        U0x400 = 0x400
    };

    enum class RendererSourceBlendMode : u32
    {
        Alpha = 0,
        One = 1,
        Zero = 2
    };

    enum class RendererStencilFunction : u32
    {
        LessEqual = 0,
        Always = 1
    };

    enum class RendererStencilMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class RendererStencilPass : u32
    {
        LessEqualDecrement = 0,
        LessEqualIncrement = 1,
        GreaterEqualIncrement = 2,
        GreaterEqualDecrement = 3
    };

    enum class RendererLightingMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class RendererWindowMode : u32
    {
        Off = 0,
        On = 1
    };

    // TODO, it looks like "Light" is also a matrix...
    struct Transform // TODO: name
    {
        Mathematics::Matrix4x4 O2W;
        union
        {
            // TODO:
            // First row is XYZ coordinates
            // The rest of the rowsc ontaint rgba color data
            Mathematics::Matrix4x4 LightMatrix; // TODO: proper data type. This holds light colors
            struct
            {
                Graphics::f32xRGBA Color;
                f32x4 XYZ[MAX_LIGHT_COUNT];
            } Light; // TODO: proper data types
        };
        Mathematics::Matrix4x4 ModelView;
    };
}