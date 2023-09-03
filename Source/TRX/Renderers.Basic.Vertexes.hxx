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

#include "Basic.hxx"
#include "Graphics.Basic.hxx"

namespace Renderers
{
    // Generic 32-byte vertex
    // Used as a proxy for D3DTLVertex and SimpleVertex
    // TODO Refactor out!
    struct AbstractVertex
    {
        f32x3 XYZ;
        u32 Unk[5];
    };

    // a.k.a. SVertex
    struct RVX
    {
        f32x3 XYZ;
        f32x3 Normal;
        f32x2 UV;
    };

    // D3DLVERTEX a.k.a. SDX8LVertex
    struct RLVX
    {
        f32x3 XYZ;
        u32 Color;
        f32x2 UV;
    };

    // D3DTLVERTEX a.k.a. SD3DTLVertex
    struct RTLVX
    {
        f32x3 XYZ;
        f32 RHW;
        u32 Color;
        u32 Specular;
        f32x2 UV;
    };

    struct REVX
    {
        f32x3 XYZ;
        f32x3 Normal;
        f32x2 UV1;
        f32x2 UV2;
    };

    struct RBVX
    {
        f32x3 XYZ;
        f32x3 Normal;
        f32x2 UV;
        f32x3 BlendWeight;
        f32x3 BlendIndexes;
        f32x2 UV1;
        f32x3 PointSize;
    };

    struct RCVX
    {
        f32x4 XYZW;
        f32x3 XYZ;
        f32x2 UV;
        Graphics::f32xRGBA Color;
        f32 Alpha;
        f32 Unknown3; // TODO
        f32 Unknown4; // TODO
    };

    struct RBNVX
    {
        u32 BoneCount;//TODO type
        f32x3 XYZ;
        f32x3 BlendWeight; // TODO: name
        f32x3 BlendIndexes; // TODO: name
        f32x3 Normal;
        f32x4 PointSize; // TODO: name
        f32x3 Diffuse; // TODO: name
        union
        {
            u32 Indexes[4];//TODO
            f32 Values[4];//TODO
        } Bones;
        f32x2 UV;
        f32x3 UV1;
        f32x3 UV2;
    };
}