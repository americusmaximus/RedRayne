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
#include "Mathematics.Basic.hxx"
#include "Module.hxx"
#include "Quit.hxx"

#include <math.h>

using namespace Assets;
using namespace DX;
using namespace Graphics;
using namespace Mathematics;;
using namespace Renderers::Modules;
using namespace Renderers;
using namespace Settings;

namespace Renderer::Module
{
    extern "C" u32 AllocateOptimizedMesh(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 AllocateVertexBuffer(const void* vertexes, const u32 vertexCount, const u32 vertexSize, void** buffer)
    {
        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On) { return FALSE; }

        const auto bufferSize = vertexCount * vertexSize;

        IDirect3DVertexBuffer9* vertexBuffer;
        DX::DXC(State.DX.DirectX.Device->CreateVertexBuffer(bufferSize, D3DUSAGE_WRITEONLY,
            D3DFVF_TEX0, D3DPOOL::D3DPOOL_MANAGED, &vertexBuffer, NULL), "Unable to create vertex buffer.");

        void* data;

        DX::DXC(vertexBuffer->Lock(0, bufferSize, (void**)&data, D3DLOCK_NONE), "Unable to lock vertex buffer.");

        CopyMemory(data, vertexes, bufferSize);

        DX::DXC(vertexBuffer->Unlock(), "Unable to unlock vertex buffer.");

        *buffer = vertexBuffer;

        return TRUE;
    }

    extern "C" u32 BeginScene(void)
    {
        if (State.DX.Scene.IsActive) { return FALSE; }

        if (State.DX.DirectX.Device->BeginScene() != D3D_OK) { return FALSE; }

        State.DX.Scene.IsActive = TRUE;

        return TRUE;
    }

    extern "C" u32 Clear(const RendererClearMode mode, const u32 color)
    {
        DWORD options = D3DCLEAR_NONE;

        if ((mode & RendererClearMode::Target) != RendererClearMode::None)
        {
            options |= D3DCLEAR_TARGET;
        }

        if ((mode & RendererClearMode::Depth) != RendererClearMode::None)
        {
            options |= D3DCLEAR_ZBUFFER;
        }

        DX::DXC(State.DX.DirectX.Device->Clear(0, NULL, options, color, 1.0f, 0), "Unable to clear.");

        return TRUE;
    }

    extern "C" u32 ClearZBuffer(void) { return TRUE; }

    u32 DotLineListD3DTL(const RTLVX* vertexes, const u32 vertexCount, const RendererMode mode, const D3DPRIMITIVETYPE primitive, const s32 primitiveCount)
    {
        DX::SelectRendererMode(mode);

        if (MINIMAL_BUFFER_CAPACITY < vertexCount) { return FALSE; }

        CopyMemory(&State.DX.Data.RTLVX.Min, vertexes, vertexCount * sizeof(RTLVX));

        const auto clippingNear = State.DX.Mode.Clipping.Near;
        const auto clippingFar = State.DX.Mode.Clipping.Far;
        const auto aspect = clippingFar / (clippingFar - clippingNear);

        for (u32 x = 0; x < vertexCount; x++)
        {
            auto rhw = State.DX.Data.RTLVX.Min[x].RHW;

            if (rhw <= 0.0f)
            {
                State.DX.Data.RTLVX.Min[x].XYZ.Z = 1.0f;
            }
            else
            {
                State.DX.Data.RTLVX.Min[x].XYZ.Z = ((1.0f / rhw) * aspect - clippingNear * aspect) * rhw;
            }

            State.DX.Data.Indexes[x] = x;
        }

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX, NULL, 0, sizeof(RTLVX)), "Unable to set stream source.");

        DX::SetShaders(DXVSN::Tlvertex, DXPSN::Decal);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitiveUP(primitive, 0, vertexCount, primitiveCount,
            &State.DX.Data.Indexes, D3DFORMAT::D3DFMT_INDEX16, &State.DX.Data.RTLVX.Min, sizeof(RTLVX)), "Unable to draw indexed primitive.");

        return TRUE;
    }

    extern "C" u32 DotListD3DL(const RLVX*, const u32, const RendererMode) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 DotListD3DTL(const RTLVX * vertexes, const u32 vertexCount, const RendererMode mode)
    {
        return DotLineListD3DTL(vertexes, vertexCount, mode, D3DPRIMITIVETYPE::D3DPT_POINTLIST, vertexCount);
    }

    extern "C" u32 DrawPixels(void) { return FALSE; }

    extern "C" u32 EnableClipping(const RendererClippingMode) { return TRUE; }

    extern "C" u32 EnableCulling(const RendererCullMode mode)
    {
        switch (mode)
        {
        case RendererCullMode::CCW: { State.DX.Mode.Cull = D3DCULL::D3DCULL_CCW; break; }
        case RendererCullMode::CW: { State.DX.Mode.Cull = D3DCULL::D3DCULL_CW; break; }
        default: { State.DX.Mode.Cull = D3DCULL::D3DCULL_NONE; break; }
        }

        return TRUE;
    }

    extern "C" u32 EnableLighting(const RendererLightingMode mode)
    {
        State.DX.Light.Mode = mode;

        return TRUE;
    }

    extern "C" u32 EndScene(void)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        State.DX.Scene.IsActive = FALSE;

        return State.DX.DirectX.Device->EndScene() == D3D_OK;
    }

    extern "C" u32 FreeOptimizedMesh(const u32) { return FALSE; }

    extern "C" u32 FreeVertexBuffer(const void* buffer)
    {
        auto vertexBuffer = (IDirect3DVertexBuffer9*)buffer;

        if (vertexBuffer != NULL)
        {
            vertexBuffer->Release();
        }

        return TRUE;
    }

    extern "C" u32 LineListD3DL(const RLVX*, const u32, const RendererMode) { return FALSE; }

    extern "C" u32 LineListD3DTL(const RTLVX * vertexes, const u32 vertexCount, const RendererMode mode)
    {
        return DotLineListD3DTL(vertexes, vertexCount, mode, D3DPRIMITIVETYPE::D3DPT_LINELIST, vertexCount - 1);
    }

    extern "C" u32 LockFrame(void)
    {
        if (State.DX.Scene.IsActive) { EndScene(); }

        if (!State.DX.Lock.IsActive)
        {
            State.DX.DirectX.Device->GetRenderTarget(DEFAULT_TEXTURE_RENDER_TARGET_INDEX, &State.DX.Surfaces.Target.Surface);

            D3DLOCKED_RECT rect;

            if (State.DX.Surfaces.Target.Surface->LockRect(&rect, NULL, D3DLOCK_NOSYSLOCK) == D3D_OK)
            {
                u32 offset = 0;

                for (u32 x = 0; x < State.DX.Windows.Current->Height; x++)
                {
                    State.DX.Windows.Current->Pixels.Pixels[x] = (u32*)((addr)rect.pBits + offset);

                    offset = offset + rect.Pitch;
                }

                State.DX.Lock.IsActive = TRUE;

                return TRUE;
            }

            return FALSE;
        }

        return TRUE;
    }

    extern "C" u32 LockFrameForScreenshot(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 PolyList(const RVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        DX::SelectRendererMode(mode);

        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
        {
            DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_LIGHTING, (DWORD)State.DX.Light.Mode);
        }

        if (DEFAULT_BUFFER_CAPACITY < vertexCount) { Quit("Too many vertexes."); }
        if (DEFAULT_BUFFER_CAPACITY < indexCount) { Quit("Too many indexes."); }

        if (DEFAULT_BUFFER_CAPACITY < State.DX.Buffers.Main.Count + vertexCount)
        {
            State.DX.Buffers.Main.Count = 0;
        }

        const auto options = State.DX.Buffers.Main.Count == 0
            ? D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK
            : D3DLOCK_NOOVERWRITE | D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK;

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
        {
            REVX* buffer;
            DX::DXC(State.DX.Buffers.Main.Buffer->Lock(State.DX.Buffers.Main.Count * sizeof(REVX),
                vertexCount * sizeof(REVX), (void**)&buffer, options), "Unable to lock vertex buffer.");

            for (u32 x = 0; x < vertexCount; x++)
            {
                const auto input = &vertexes[x];
                auto vertex = &buffer[x];

                vertex->XYZ.X = input->XYZ.X;
                vertex->XYZ.Y = input->XYZ.Y;
                vertex->XYZ.Z = input->XYZ.Z;

                vertex->Normal.X = input->Normal.X;
                vertex->Normal.Y = input->Normal.Y;
                vertex->Normal.Z = input->Normal.Z;

                vertex->UV1.X = input->UV.X;
                vertex->UV1.Y = input->UV.Y;

                vertex->UV2.X = input->Normal.X;
                vertex->UV2.Y = input->Normal.Y;
            }
        }
        else
        {
            RVX* buffer;
            DX::DXC(State.DX.Buffers.Main.Buffer->Lock(State.DX.Buffers.Main.Count * sizeof(RVX),
                vertexCount * sizeof(RVX), (void**)&buffer, options), "Unable to lock vertex buffer.");

            CopyMemory(buffer, vertexes, vertexCount * sizeof(RVX));
        }

        DX::DXC(State.DX.Buffers.Main.Buffer->Unlock(), "Unable to unlock vertex buffer.");

        DX::SetIndexes(indexes, indexCount);

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX,
            State.DX.Buffers.Main.Buffer, 0,
            State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On ? sizeof(REVX) : sizeof(RVX)),
            "Unable to set stream source.");

        DXC(State.DX.DirectX.Device->SetIndices(State.DX.Buffers.Index.Buffer), "Unable to set index source.");

        DX::SetBlendShaders(mode);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            State.DX.Buffers.Main.Count, 0, vertexCount,
            State.DX.Buffers.Index.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount), "Unable to draw indexed primitive.");

        State.DX.Buffers.Main.Count += vertexCount;
        State.DX.Buffers.Index.Count += indexCount;

        return TRUE;
    }

    extern "C" u32 PolyListBasis(const RBVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        DX::SelectRendererMode(mode);

        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);

        if (DEFAULT_BUFFER_CAPACITY < vertexCount) { Quit("Too many vertexes."); }
        if (DEFAULT_BUFFER_CAPACITY < indexCount) { Quit("Too many indexes."); }

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
        {
            if (DEFAULT_BUFFER_CAPACITY < State.DX.Buffers.Main.Count + vertexCount)
            {
                State.DX.Buffers.Main.Count = 0;
            }

            DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_LIGHTING, (DWORD)State.DX.Light.Mode);

            const auto options = State.DX.Buffers.Main.Count == 0
                ? D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK
                : D3DLOCK_NOOVERWRITE | D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK;

            REVX* buffer;
            DX::DXC(State.DX.Buffers.Main.Buffer->Lock(State.DX.Buffers.Main.Count * sizeof(REVX),
                vertexCount * sizeof(REVX), (void**)&buffer, options), "Unable to lock vertex buffer.");

            for (u32 x = 0; x < vertexCount; x++)
            {
                const auto input = &vertexes[x];
                auto vertex = &buffer[x];

                vertex->XYZ.X = input->XYZ.X;
                vertex->XYZ.Y = input->XYZ.Y;
                vertex->XYZ.Z = input->XYZ.Z;

                vertex->Normal.X = input->Normal.X;
                vertex->Normal.Y = input->Normal.Y;
                vertex->Normal.Z = input->Normal.Z;

                vertex->UV1.X = input->UV.X;
                vertex->UV1.Y = input->UV.Y;

                vertex->UV2.X = input->UV1.X;
                vertex->UV2.Y = input->UV1.Y;
            }

            DX::DXC(State.DX.Buffers.Main.Buffer->Unlock(), "Unable to unlock vertex buffer.");

            DX::SetIndexes(indexes, indexCount);

            DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX,
                State.DX.Buffers.Main.Buffer, 0, sizeof(REVX)), "Unable to set stream source.");

            DXC(State.DX.DirectX.Device->SetIndices(State.DX.Buffers.Index.Buffer), "Unable to set index source.");

            DX::SetBlendShaders(mode);

            DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
                State.DX.Buffers.Main.Count, 0, vertexCount,
                State.DX.Buffers.Index.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount), "Unable to draw indexed primitive.");

            State.DX.Buffers.Main.Count += vertexCount;
            State.DX.Buffers.Index.Count += indexCount;
        }
        else
        {
            if (DEFAULT_BUFFER_CAPACITY < State.DX.Buffers.Basis.Count + vertexCount)
            {
                State.DX.Buffers.Basis.Count = 0;
            }

            const auto options = State.DX.Buffers.Basis.Count == 0
                ? D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK
                : D3DLOCK_NOOVERWRITE | D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK;

            RBVX* buffer;
            DX::DXC(State.DX.Buffers.Basis.Buffer->Lock(State.DX.Buffers.Basis.Count * sizeof(RBVX),
                vertexCount * sizeof(RBVX), (void**)&buffer, options), "Unable to lock vertex buffer.");

            CopyMemory(buffer, vertexes, vertexCount * sizeof(RBVX));

            DX::DXC(State.DX.Buffers.Basis.Buffer->Unlock(), "Unable to unlock vertex buffer.");

            DX::SetIndexes(indexes, indexCount);

            DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX,
                State.DX.Buffers.Basis.Buffer, 0, sizeof(RBVX)), "Unable to set stream source.");

            DXC(State.DX.DirectX.Device->SetIndices(State.DX.Buffers.Index.Buffer), "Unable to set index source.");

            DX::SetShadersMode(mode);

            DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
                State.DX.Buffers.Basis.Count, 0, vertexCount,
                State.DX.Buffers.Index.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount), "Unable to draw indexed primitive.");

            State.DX.Buffers.Basis.Count += vertexCount;
            State.DX.Buffers.Index.Count += indexCount;
        }

        return TRUE;
    }

    extern "C" u32 PolyListBone(const RBNVX*, const u32, const u16*, const u32, const f32x4*, const RendererMode) { return FALSE; }

    extern "C" u32 PolyListD3DL(const RTLVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        DX::SelectRendererMode(mode);

        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);
        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_LIGHTING, FALSE);

        if (DEFAULT_BUFFER_CAPACITY < vertexCount) { Quit("Too many vertexes."); }
        if (DEFAULT_BUFFER_CAPACITY < indexCount) { Quit("Too many indexes."); }

        if (DEFAULT_BUFFER_CAPACITY < State.DX.Buffers.RLVX.Count + vertexCount)
        {
            State.DX.Buffers.RLVX.Count = 0;
        }

        const auto options = State.DX.Buffers.RLVX.Count == 0
            ? D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK
            : D3DLOCK_NOOVERWRITE | D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK;

        RLVX* buffer;
        DX::DXC(State.DX.Buffers.RLVX.Buffer->Lock(State.DX.Buffers.RLVX.Count * sizeof(RLVX),
            vertexCount * sizeof(RLVX), (void**)&buffer, options), "Unable to lock vertex buffer.");

        for (u32 x = 0; x < vertexCount; x++)
        {
            const auto input = &vertexes[x];
            auto vertex = &buffer[x];

            vertex->XYZ.X = input->XYZ.X;
            vertex->XYZ.Y = input->XYZ.Y;
            vertex->XYZ.Z = input->XYZ.Z;

            vertex->Color = input->Color;

            vertex->UV.X = input->UV.X;
            vertex->UV.Y = input->UV.Y;
        }

        DX::DXC(State.DX.Buffers.RLVX.Buffer->Unlock(), "Unable to unlock vertex buffer.");

        DX::SetIndexes(indexes, indexCount);

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX,
            State.DX.Buffers.RLVX.Buffer, 0, sizeof(RLVX)), "Unable to set stream source.");

        DXC(State.DX.DirectX.Device->SetIndices(State.DX.Buffers.Index.Buffer), "Unable to set index source.");

        DX::SetRTLVXShaders(mode);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            State.DX.Buffers.RLVX.Count, 0, vertexCount,
            State.DX.Buffers.Index.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount), "Unable to draw indexed primitive.");

        State.DX.Buffers.RLVX.Count += vertexCount;
        State.DX.Buffers.Index.Count += indexCount;

        return TRUE;
    }

    extern "C" u32 PolyListD3DTL(const RTLVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        DX::SelectRendererMode(mode & (~(RendererMode::U0x80 | RendererMode::U0x40 | RendererMode::U0x8)));

        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);

        if (RENDERER_TEN_THOUSAND_ELEMENT_COUNT < vertexCount) { Quit("Too many vertexes."); }
        if (DEFAULT_BUFFER_CAPACITY < indexCount) { Quit("Too many indexes."); }

        if (RENDERER_TEN_THOUSAND_ELEMENT_COUNT < State.DX.Buffers.RTLVX.Count + vertexCount)
        {
            State.DX.Buffers.RTLVX.Count = 0;
        }

        const auto options = State.DX.Buffers.RTLVX.Count == 0
            ? D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK
            : D3DLOCK_NOOVERWRITE | D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK;

        RTLVX* buffer;
        DX::DXC(State.DX.Buffers.RTLVX.Buffer->Lock(State.DX.Buffers.RTLVX.Count * sizeof(RTLVX),
            vertexCount * sizeof(RTLVX), (void**)&buffer, options), "Unable to lock vertex buffer.");

        CopyMemory(buffer, vertexes, vertexCount * sizeof(RTLVX));

        DX::DXC(State.DX.Buffers.RTLVX.Buffer->Unlock(), "Unable to unlock vertex buffer.");

        DX::SetIndexes(indexes, indexCount);

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX,
            State.DX.Buffers.RTLVX.Buffer, 0, sizeof(RTLVX)), "Unable to set stream source.");

        DXC(State.DX.DirectX.Device->SetIndices(State.DX.Buffers.Index.Buffer), "Unable to set index source.");

        DX::SetShaders(DXVSN::Tlvertex, DXPSN::Decal);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            State.DX.Buffers.RTLVX.Count, 0, vertexCount,
            State.DX.Buffers.Index.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount), "Unable to draw indexed primitive.");

        State.DX.Buffers.RTLVX.Count += vertexCount;
        State.DX.Buffers.Index.Count += indexCount;

        return TRUE;
    }

    extern "C" u32 PolyListTL(const RCVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (!State.DX.Scene.IsActive) { return FALSE; }

        if (State.DX.Mode.Blend.BlendOperation != RendererBlendOperation::None) { return FALSE; }

        DX::SelectRendererMode(mode);

        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);
        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_LIGHTING, FALSE);

        for (u32 x = 0; x < vertexCount; x++)
        {
            const auto input = &vertexes[x];
            auto vertex = &State.DX.Data.RTLVX.RTLVX[x];

            vertex->XYZ.X = input->XYZ.X;
            vertex->XYZ.Y = input->XYZ.Y;
            vertex->XYZ.Z = Max(0.0f, 1.0f - input->XYZW.W);

            vertex->Color = DX::Color(input->Color.R, input->Color.G, input->Color.B, input->Color.A);
            vertex->Specular = DX::Alpha(1.0f - input->Color.A);

            if ((mode & RendererMode::U0x400) != RendererMode::None)
            {
                vertex->RHW = input->Unknown3 * input->XYZW.W;

                vertex->UV.X = ((1.0f / input->Unknown3) * input->XYZW.W + 1.0f) * 0.5f;
                vertex->UV.Y = 0.5f - (1.0f / input->Unknown3) * input->XYZW.W * 0.5f;
            }
            else
            {
                vertex->RHW = input->XYZW.W;

                vertex->UV.X = input->UV.X;
                vertex->UV.Y = input->UV.Y;
            }
        }

        DX::SetShaders(DXVSN::Tlvertex, DXPSN::Decal);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            0, vertexCount, indexCount, indexes, D3DFORMAT::D3DFMT_INDEX16,
            State.DX.Data.RTLVX.RTLVX, sizeof(RTLVX)), "Unable to draw indexed primitive.");

        return TRUE;
    }

    extern "C" u32 RenderOptimizedMesh(void) { return FALSE; }

    extern "C" u32 RenderVertexBuffer(const void* buffer, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        DX::SelectRendererMode(mode);

        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);

        DX::SetIndexes(indexes, indexCount);

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX,
            (IDirect3DVertexBuffer9*)buffer, 0, sizeof(RVX)), "Unable to set stream source.");

        DXC(State.DX.DirectX.Device->SetIndices(State.DX.Buffers.Index.Buffer), "Unable to set index source.");

        DX::SetBlendShaders(mode);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            0, 0, vertexCount,
            State.DX.Buffers.Index.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount), "Unable to draw indexed primitive.");

        State.DX.Buffers.Index.Count += indexCount;

        return TRUE;
    }

    extern "C" u32 RenderVertexBufferBasis(const void* buffer, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        DX::SelectRendererMode(mode);

        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);

        DX::SetIndexes(indexes, indexCount);

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX,
            (IDirect3DVertexBuffer9*)buffer, 0, sizeof(RBVX)), "Unable to set stream source.");

        DXC(State.DX.DirectX.Device->SetIndices(State.DX.Buffers.Index.Buffer), "Unable to set index source.");

        DX::SetShadersMode(mode);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            0, 0, vertexCount,
            State.DX.Buffers.Index.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount), "Unable to draw indexed primitive.");

        State.DX.Buffers.Index.Count += indexCount;

        return TRUE;
    }

    extern "C" u32 RenderVertexBufferBone(const void* buffer, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const f32x4 * constants, const u32 constantCount, const RendererMode mode)
    {
        if (SHADER_REGISTER_24_INDEX <= constantCount) { return FALSE; }

        DX::SelectRendererMode(mode);

        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);

        DX::SetIndexes(indexes, indexCount);

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX,
            (IDirect3DVertexBuffer9*)buffer, 0, sizeof(RBNVX)), "Unable to set stream source.");

        DXC(State.DX.DirectX.Device->SetIndices(State.DX.Buffers.Index.Buffer), "Unable to set index source.");

        DX::SetSkeletonShaders(mode);

        for (u32 x = 0; x < constantCount; x++)
        {
            const auto indx = SHADER_REGISTER_25_INDEX + 3 * x;
            const auto shaderConstants = &constants[x * 3];

            DX::DXC(State.DX.DirectX.Device->SetVertexShaderConstantF(indx, (f32*)shaderConstants, 3),
                "Unable to set vertex shader constants.");
        }

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            0, 0, vertexCount,
            State.DX.Buffers.Index.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount), "Unable to draw indexed primitive.");

        State.DX.Buffers.Index.Count += indexCount;

        return TRUE;
    }

    extern "C" u32 RenderVertexBufferPrelit(const void* buffer, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        DX::SelectRendererMode(mode);

        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_CULLMODE, State.DX.Mode.Cull);

        DX::SetIndexes(indexes, indexCount);

        DX::DXC(State.DX.DirectX.Device->SetStreamSource(STREAM_SOURCE_0_INDEX,
            (IDirect3DVertexBuffer9*)buffer, 0, sizeof(RVX)), "Unable to set stream source.");

        DXC(State.DX.DirectX.Device->SetIndices(State.DX.Buffers.Index.Buffer), "Unable to set index source.");

        DX::SetLightShaders(mode);

        DX::DXC(State.DX.DirectX.Device->DrawIndexedPrimitive(D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,
            0, 0, vertexCount,
            State.DX.Buffers.Index.Count * GRAPHICS_TRIANGLE_VERTEX_COUNT, indexCount), "Unable to draw indexed primitive.");

        State.DX.Buffers.Index.Count += indexCount;

        return TRUE;
    }

    extern "C" u32 SetAlpha(const u32 value)
    {
        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_ALPHAREF, value);

        return TRUE;
    }

    extern "C" u32 SetAmbientLight(const f32 color)
    {
        State.DX.Mode.IsChanged = TRUE;

        State.DX.Light.Lights[0].Ambient.r = color;
        State.DX.Light.Lights[0].Ambient.g = color;
        State.DX.Light.Lights[0].Ambient.b = color;
        State.DX.Light.Lights[0].Ambient.a = color;

        return TRUE;
    }

    extern "C" u32 SetAmbientLightColor(const f32 r, const f32 g, const f32 b)
    {
        State.DX.Mode.IsChanged = TRUE;

        State.DX.Light.Lights[0].Ambient.r = r;
        State.DX.Light.Lights[0].Ambient.g = g;
        State.DX.Light.Lights[0].Ambient.b = b;
        State.DX.Light.Lights[0].Ambient.a = 1.0f;

        return TRUE;
    }

    extern "C" u32 SetColorTable(const u8 * rgb, u16 * rgba)
    {
        if (State.DX.Windows.Current->BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_16)
        {
            *State.InitializeArguments.R.Display = 11;
            *State.InitializeArguments.R.Font = 8;
            *State.InitializeArguments.R.BitsPerPixel = 3;

            *State.InitializeArguments.G.Display = 5;
            *State.InitializeArguments.G.Font = 4;
            *State.InitializeArguments.G.BitsPerPixel = 2;

            *State.InitializeArguments.B.Display = 0;
            *State.InitializeArguments.B.Font = 8;
            *State.InitializeArguments.B.BitsPerPixel = 3;
        }
        else
        {
            *State.InitializeArguments.R.Display = 16;
            *State.InitializeArguments.R.Font = 1;
            *State.InitializeArguments.R.BitsPerPixel = 0;

            *State.InitializeArguments.G.Display = 8;
            *State.InitializeArguments.G.Font = 1;
            *State.InitializeArguments.G.BitsPerPixel = 0;

            *State.InitializeArguments.B.Display = 0;
            *State.InitializeArguments.B.Font = 1;
            *State.InitializeArguments.B.BitsPerPixel = 0;
        }

        {
            const auto rf = *State.InitializeArguments.R.Font;
            const auto rd = *State.InitializeArguments.R.Display;

            const auto gf = *State.InitializeArguments.G.Font;
            const auto gd = *State.InitializeArguments.G.Display;

            const auto bf = *State.InitializeArguments.B.Font;
            const auto bd = *State.InitializeArguments.B.Display;

            for (u32 x = 0; x < MAX_COLOR_TABLE_BUFFER_SIZE; x++)
            {
                auto r = rgb[x * 3 + 0] / rf << (rd & 0x1f);
                auto g = rgb[x * 3 + 1] / gf << (gd & 0x1f);
                auto b = rgb[x * 3 + 2] / bf << (bd & 0x1f);

                rgba[x] = (u16)(r | g | b);
            }
        }

        return TRUE;
    }

    extern "C" u32 SetDestinationBlend(const RendererDestinationBlend mode)
    {
        switch (mode)
        {
        case RendererDestinationBlend::InverseAlpha:
        {
            State.DX.Mode.Blend.DestinationBlendMode = D3DBLEND::D3DBLEND_INVSRCALPHA;
            State.DX.Mode.Blend.DestinationBlendOperation = D3DBLENDOP::D3DBLENDOP_ADD;
            break;
        }
        case RendererDestinationBlend::One:
        {
            State.DX.Mode.Blend.DestinationBlendMode = D3DBLEND::D3DBLEND_ONE;
            State.DX.Mode.Blend.DestinationBlendOperation = D3DBLENDOP::D3DBLENDOP_ADD;
            break;
        }
        case RendererDestinationBlend::Color:
        {
            State.DX.Mode.Blend.DestinationBlendMode = D3DBLEND::D3DBLEND_SRCCOLOR;
            State.DX.Mode.Blend.DestinationBlendOperation = D3DBLENDOP::D3DBLENDOP_ADD;
            break;
        }
        case RendererDestinationBlend::InverseColor:
        {
            State.DX.Mode.Blend.DestinationBlendMode = D3DBLEND::D3DBLEND_INVSRCCOLOR;
            State.DX.Mode.Blend.DestinationBlendOperation = D3DBLENDOP::D3DBLENDOP_ADD;
            break;
        }
        case RendererDestinationBlend::Zero:
        {
            State.DX.Mode.Blend.DestinationBlendMode = D3DBLEND::D3DBLEND_ZERO;
            State.DX.Mode.Blend.DestinationBlendOperation = D3DBLENDOP::D3DBLENDOP_ADD;
            break;
        }
        case RendererDestinationBlend::NegativeOne:
        {
            State.DX.Mode.Blend.DestinationBlendMode = D3DBLEND::D3DBLEND_ONE;
            State.DX.Mode.Blend.DestinationBlendOperation = D3DBLENDOP::D3DBLENDOP_REVSUBTRACT;
            break;
        }
        }

        return TRUE;
    }

    extern "C" u32 SetDirtyRect(const u32, const u32, const u32, const u32) { return FALSE; }

    extern "C" u32 SetFog(const f32 r, const f32 g, const f32 b, const f32 start, const f32 end)
    {
        const u32 rv = Clamp<u32>((u32)Max<f32>(0.0f, (r * 255.0f)), 0, 255);
        const u32 gv = Clamp<u32>((u32)Max<f32>(0.0f, (g * 255.0f)), 0, 255);
        const u32 bv = Clamp<u32>((u32)Max<f32>(0.0f, (b * 255.0f)), 0, 255);

        const u32 color = (rv << 16) | (gv << 8) | bv;

        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGCOLOR, color);

        // Depth at which pixel or vertex fog effects begin for linear fog mode.The default value is 0.0f.
        // Depth is specified in world space for vertex fog and either device space[0.0, 1.0] or world space for pixel fog.
        // For pixel fog, these values are in device space when the system uses z for fog calculations and world - world space when the system is using eye - relative fog(w - fog).
        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGSTART, *((DWORD*)(&start)));

        // Depth at which pixel or vertex fog effects end for linear fog mode. The default value is 1.0f.
        // Depth is specified in world space for vertex fog and either device space [0.0, 1.0] or world space for pixel fog.
        // For pixel fog, these values are in device space when the system uses z for fog calculations and in world space when the system is using eye-relative fog (w-fog).
        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGEND, *((DWORD*)(&end)));

        // Fog density for pixel or vertex fog used in the exponential fog modes (D3DFOG_EXP and D3DFOG_EXP2). Valid density values range from 0.0 through 1.0.
        DX::SetRenderState(D3DRENDERSTATETYPE::D3DRS_FOGDENSITY, 0);

        State.DX.Mode.IsChanged = TRUE;

        State.DX.Fog.Start = start;
        State.DX.Fog.End = end;

        State.DX.Fog.Color.R = r;
        State.DX.Fog.Color.G = g;
        State.DX.Fog.Color.B = b;

        return TRUE;
    }

    extern "C" u32 SetGloss(const f32) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SetLightColor(const f32 r, const f32 g, const f32 b, const u32 indx)
    {
        State.DX.Light.Colors.R[indx] = r;
        State.DX.Light.Colors.G[indx] = g;
        State.DX.Light.Colors.B[indx] = b;

        return TRUE;
    }

    extern "C" u32 SetLightConstants(const f32 diffuse, const f32 specular, const f32 power, const f32 alpha)
    {
        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
        {
            const D3DMATERIAL9 material =
            {
                .Diffuse =
                {
                    .r = diffuse,
                    .g = diffuse,
                    .b = diffuse,
                    .a = alpha
                },
                .Ambient =
                {
                    .r = 1.0f,
                    .g = 1.0f,
                    .b = 1.0f,
                    .a = alpha
                },
                .Specular =
                {
                    .r = specular,
                    .g = specular,
                    .b = specular,
                    .a = alpha
                },
                .Emissive =
                {
                    .r = 0.0f,
                    .g = 0.0f,
                    .b = 0.0f,
                    .a = 0.0f
                },
                .Power = power
            };

            DX::DXC(State.DX.DirectX.Device->SetMaterial(&material), "Unable to set material.");
        }

        if (State.DX.Light.Alpha != alpha)
        {
            State.DX.Mode.IsChanged = TRUE;
        }

        State.DX.Light.Alpha = alpha;

        return TRUE;
    }

    extern "C" u32 SetLightVector(const f32x3*, const f32, const u32) { return TRUE; }

    extern "C" u32 SetMultiTextureBlend(const RendererBlendOperation operation)
    {
        State.DX.Mode.Blend.BlendOperation = operation;

        if (operation == RendererBlendOperation::None || operation == RendererBlendOperation::SelectArgument2)
        {
            return TRUE;
        }

        return State.DX.Devices.Current.Capabilities.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3;
    }

    extern "C" u32 SetOmniLight(const f32xRGB * rgb, const f32 alpha, const u32 color)
    {
        const D3DCOLORVALUE constants[] =
        {
            {
                .r = rgb->R,
                .g = rgb->G,
                .b = rgb->B,
                .a = alpha
            },
            {
                .r = ((color >> 16) & 0xff) * ONE_TWO_HUNDRED_FIFTY_FIFTH,
                .g = ((color >> 8) & 0xff) * ONE_TWO_HUNDRED_FIFTY_FIFTH,
                .b = (color & 0xff) * ONE_TWO_HUNDRED_FIFTY_FIFTH,
                .a = 1.0f
            }
        };

        DX::DXC(State.DX.DirectX.Device->SetVertexShaderConstantF(ACTUAL_SHADER_REGISTER_CONSTANT_COUNT,
            (f32*)&constants, ARRAYSIZE(constants)), "Unable to set vertex shader constants.");

        return TRUE;
    }

    extern "C" u32 SetSourceBlend(const RendererSourceBlendMode mode)
    {
        switch (mode)
        {
        case RendererSourceBlendMode::Alpha: { State.DX.Mode.Blend.SourceBlendMode = D3DBLEND::D3DBLEND_SRCALPHA; break; }
        case RendererSourceBlendMode::One: { State.DX.Mode.Blend.SourceBlendMode = D3DBLEND::D3DBLEND_ONE; break; }
        case RendererSourceBlendMode::Zero: { State.DX.Mode.Blend.SourceBlendMode = D3DBLEND::D3DBLEND_ZERO; break; }
        }

        return TRUE;
    }

    extern "C" u32 SetStencilEnable(const RendererStencilMode) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SetStencilFunc(const RendererStencilFunction) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SetStencilPass(const RendererStencilPass) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SetTextureClamp(const RendererClampMode u, const RendererClampMode v, const u32 indx)
    {
        if (State.DX.Mode.Clamp.U[indx] != u || State.DX.Mode.Clamp.V[indx] != v)
        {
            State.DX.Mode.Clamp.U[indx] = u;
            State.DX.Mode.Clamp.V[indx] = v;

            State.DX.Mode.Clamp.ClampOperation = RendererBlendOperation::All;
        }

        return TRUE;
    }

    extern "C" u32 SetTransform(const Transform * transform, const f32 zNear, const f32 zFar)
    {
        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
        {
            State.DX.DirectX.Device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&transform->ModelView);

            State.DX.DirectX.Device->SetTransform(D3DTRANSFORMSTATETYPE::D3DTS_VIEW,
                (D3DMATRIX*)&State.DX.Transform.Identity);
        }

        BOOL changed = FALSE;

        if (State.DX.Transform.Near != zNear || State.DX.Transform.Far != zFar)
        {
            changed = TRUE;

            State.DX.Transform.Near = zNear;
            State.DX.Transform.Far = zFar;
        }

        if (State.DX.Transform.IsCahnged || changed)
        {
            State.DX.Transform.IsCahnged = FALSE;

            const auto clippingNear = State.DX.Mode.Clipping.Near;
            const auto clippingFar = State.DX.Mode.Clipping.Far;

            State.DX.Transform.Projection.M11 = zNear;
            State.DX.Transform.Projection.M12 = 0.0f;
            State.DX.Transform.Projection.M13 = 0.0f;
            State.DX.Transform.Projection.M14 = 0.0f;

            State.DX.Transform.Projection.M21 = 0.0f;
            State.DX.Transform.Projection.M22 = zFar;
            State.DX.Transform.Projection.M23 = 0.0f;
            State.DX.Transform.Projection.M24 = 0.0f;

            State.DX.Transform.Projection.M31 = 0.0f;
            State.DX.Transform.Projection.M32 = 0.0f;
            State.DX.Transform.Projection.M33 = clippingFar / (clippingFar - clippingNear);
            State.DX.Transform.Projection.M34 = 1.0f;

            State.DX.Transform.Projection.M41 = 0.0f;
            State.DX.Transform.Projection.M42 = 0.0f;
            State.DX.Transform.Projection.M43 = -(clippingNear * clippingFar / (clippingFar - clippingNear));
            State.DX.Transform.Projection.M44 = 0.0f;

            State.DX.DirectX.Device->SetTransform(D3DTRANSFORMSTATETYPE::D3DTS_PROJECTION, (D3DMATRIX*)&State.DX.Transform.Projection);
        }

        CopyMemory(&State.DX.Transform.ModelView, &transform->ModelView, sizeof(Matrix4x4));

        for (u32 x = 0; x < MAX_LIGHT_COUNT; x++)
        {
            State.DX.Light.XYZ[x].X = transform->Light.XYZ[x].X;
            State.DX.Light.XYZ[x].Y = transform->Light.XYZ[x].Y;
            State.DX.Light.XYZ[x].Z = transform->Light.XYZ[x].Z;
        }

        State.DX.Light.Color.R = transform->Light.Color.R;
        State.DX.Light.Color.G = transform->Light.Color.G;
        State.DX.Light.Color.B = transform->Light.Color.B;

        State.DX.Mode.IsChanged = TRUE;

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
        {
            for (u32 x = 0; x < MAX_LIGHT_COUNT; x++)
            {
                auto item = &State.DX.Light.XYZ[x];
                auto light = &State.DX.Light.Lights[x];

                auto length = sqrtf(item->X * item->X + item->Y * item->Y + item->Z * item->Z);

                if (length == 0.0f) { length = 1.0f; }

                auto px = item->X / length;
                auto py = item->Y / length;
                auto pz = item->Z / length;

                light->Direction.x = px * transform->ModelView.M11 + py * transform->ModelView.M21 + pz * transform->ModelView.M31;
                light->Direction.y = px * transform->ModelView.M12 + py * transform->ModelView.M22 + pz * transform->ModelView.M32;
                light->Direction.z = px * transform->ModelView.M13 + py * transform->ModelView.M23 + pz * transform->ModelView.M33;

                light->Diffuse.r = length * State.DX.Light.Colors.R[x];
                light->Diffuse.g = length * State.DX.Light.Colors.G[x];
                light->Diffuse.b = length * State.DX.Light.Colors.B[x];
            }

            DX::SelectLight();
        }

        return TRUE;
    }

    extern "C" u32 SetTransformO2W(const Matrix4x4 * transform)
    {
        CopyMemory(&State.DX.Transform.O2W, transform, sizeof(Matrix4x4));

        State.DX.Mode.IsChanged = TRUE;

        return TRUE;
    }

    extern "C" u32 SetTransformW2L(const Matrix4x4 * transform, const f32 zNear, const f32 zFar)
    {
        Matrix4x4 matrix =
        {
            0.5f, 0.0f,  0.0f, 0.0f,
            0.0f, -0.5f, 0.0f, 0.0f,
            0.5f, 0.5f,  1.0f, 1.0f,
            0.0f, 0.0f,  0.0f, 0.0f
        };

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::Off)
        {
            matrix.M33 = zFar / (zFar - zNear);
            matrix.M43 = -zNear * zFar / (zFar - zNear);
        }

        Matrix4x4 result;

        MultiplyMatrix(&result, transform, &matrix);

        if (State.Settings.FixedPipelineMode == FixedPipelineOptionsMode::On)
        {
            Matrix4x4 inverted;

            InvertMatrix(&inverted, &State.DX.Transform.ModelView);

            Matrix4x4 mt;

            MultiplyMatrix(&mt, &inverted, &result);

            State.DX.DirectX.Device->SetTransform(D3DTRANSFORMSTATETYPE::D3DTS_TEXTURE0, (D3DMATRIX*)&mt);

            return TRUE;
        }

        CopyMemory(&State.DX.Transform.W2L, &result, sizeof(Matrix4x4));

        State.DX.Mode.IsChanged = TRUE;

        return TRUE;
    }

    extern "C" u32 SetViewPort(const f32 x0, const f32 y0, const f32 x1, const f32 y1, const f32 zNear, const f32 zFar)
    {
        if (x1 <= 0 || y1 <= 0) { return FALSE; }

        const D3DVIEWPORT9 viewport =
        {
            .X = (u32)(x0),
            .Y = (u32)(y0),
            .Width = (u32)(x1 - x0),
            .Height = (u32)(y1 - y0),
            .MinZ = 0.0f,
            .MaxZ = 1.0f
        };

        State.DX.DirectX.Device->SetViewport(&viewport);

        State.DX.Mode.Clipping.Near = zNear;
        State.DX.Mode.Clipping.Far = zFar;

        State.DX.Transform.IsCahnged = TRUE;

        return TRUE;
    }

    extern "C" u32 Toggle(void)
    {
        if (State.DX.DirectX.Device != NULL)
        {
            if (State.Window.Mode == RendererWindowMode::On)
            {
                State.DX.DirectX.Device->Present(NULL, NULL, NULL, NULL);

                for (u32 x = MIN_WINDOW_INDEX; x < MAX_WINDOW_COUNT; x++)
                {
                    auto window = State.DX.Windows.Windows[x];

                    if (window == NULL) { continue; }

                    auto chain = window->SwapChain;

                    if (chain != NULL)
                    {
                        chain->Present(NULL, NULL, NULL, NULL, D3DPRESENT_NONE);
                    }
                }

                return TRUE;
            }

            DX::DXC(State.DX.DirectX.Device->Present(NULL, NULL, NULL, NULL), "Unable to present back buffer.");
        }

        return TRUE;
    }

    extern "C" u32 UnlockFrame(void)
    {
        if (!State.DX.Lock.IsActive) { return FALSE; }

        State.DX.Lock.IsActive = FALSE;

        for (u32 x = 0; x < State.DX.Windows.Current->Height; x++)
        {
            State.DX.Windows.Current->Pixels.Pixels[x] = State.DX.Windows.Current->Pixels.Values[x];
        }

        DX::DXC(State.DX.Surfaces.Target.Surface->UnlockRect(), "Unable to unlock back buffer.");

        State.DX.Surfaces.Target.Surface->Release();

        return TRUE;
    }
}