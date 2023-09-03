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

#include "Module.hxx"
#include "Quit.hxx"
#include "Textures.hxx"

#include <math.h>

using namespace Assets;
using namespace Graphics;
using namespace Mathematics;
using namespace Renderers::Modules;
using namespace Renderers;

namespace Renderer::Module
{
    extern "C" u32 AllocateOptimizedMesh(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 AllocateVertexBuffer(const void*, const u32, const u32, void**) { return FALSE; }

    extern "C" u32 BeginScene(void) { return TRUE; }

    extern "C" u32 Clear(const RendererClearMode, const u32 color)
    {
        GL::SelectRendererMode(RendererMode::U0x80 | RendererMode::U0x40);

        const auto r = ONE_TWO_HUNDRED_FIFTY_FIFTH * (color >> 16 & 0xff);
        const auto g = ONE_TWO_HUNDRED_FIFTY_FIFTH * (color >> 8 & 0xff);
        const auto b = ONE_TWO_HUNDRED_FIFTY_FIFTH * (color >> 0 & 0xff);

        glClearColor(r, g, b, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        return TRUE;
    }

    extern "C" u32 ClearZBuffer(void)
    {
        GL::SelectRendererMode(RendererMode::U0x80 | RendererMode::U0x40);

        glClear(GL_DEPTH_BUFFER_BIT);

        return TRUE;
    }

    extern "C" u32 DotListD3DL(const RLVX*, const u32, const RendererMode) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 DotListD3DTL(const RTLVX*, const u32, const RendererMode) { return FALSE; }

    extern "C" u32 DrawPixels(void) { return FALSE; }

    extern "C" u32 EnableClipping(const RendererClippingMode) { return TRUE; }

    extern "C" u32 EnableCulling(const RendererCullMode mode)
    {
        State.GL.Mode.Cull = mode;

        return TRUE;
    }

    extern "C" u32 EnableLighting(const RendererLightingMode mode)
    {
        State.GL.Light.Mode = mode;

        return TRUE;
    }

    extern "C" u32 EndScene(void) { return TRUE; }

    extern "C" u32 FreeOptimizedMesh(const u32) { return FALSE; }

    extern "C" u32 FreeVertexBuffer(const void*) { return FALSE; }

    extern "C" u32 LineListD3DL(const RLVX*, const u32, const RendererMode) { return FALSE; }

    extern "C" u32 LineListD3DTL(const RTLVX*, const u32, const RendererMode) { return FALSE; }

    extern "C" u32 LockFrame(void) { return FALSE; }

    extern "C" u32 LockFrameForScreenshot(void) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 PolyList(const RVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (MAX_VERTEX_COUNT < vertexCount) { return FALSE; }

        GL::SelectRendererMode(mode);

        GL::GLF.glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(RVX), &vertexes[0].XYZ);
        glColorPointer(4, GL_FLOAT, sizeof(f32xRGBA), &State.GL.Vertexes.Colors);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RVX), &vertexes[0].UV);
        glNormalPointer(GL_FLOAT, sizeof(RVX), &vertexes[0].Normal);

        if (State.GL.Light.Mode == RendererLightingMode::On)
        {
            glEnableClientState(GL_NORMAL_ARRAY);
        }

        if (State.GL.Textures.MultiBlendOperation == RendererBlendOperation::None
            || State.GL.Textures.MultiBlendOperation == RendererBlendOperation::AddSigned
            || State.GL.Textures.MultiBlendOperation == RendererBlendOperation::Subtract
            || State.GL.Textures.MultiBlendOperation == RendererBlendOperation::AddSmooth)
        {
            GL::GLF.glActiveTextureARB(GL_TEXTURE1_ARB);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glDisable(GL_TEXTURE_2D);
            GL::GLF.glActiveTextureARB(GL_TEXTURE0_ARB);
            glEnable(GL_FOG);
            glEnable(GL_LIGHTING);
        }
        else
        {
            GL::GLF.glClientActiveTextureARB(GL_TEXTURE1_ARB);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, sizeof(RVX), &vertexes[0].UV);
            GL::GLF.glActiveTextureARB(GL_TEXTURE1_ARB);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glEnable(GL_TEXTURE_2D);
            GL::GLF.glActiveTextureARB(GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }

        const f32 material[] = { 1.0f, 1.0f, 1.0f, State.GL.Light.Colors.Alpha };

        glMaterialfv(GL_FRONT, GL_DIFFUSE, material);

        for (u32 x = 0; x < vertexCount; x++)
        {
            auto color = &State.GL.Vertexes.Colors[x];

            color->R = 1.0f;
            color->G = 1.0f;
            color->B = 1.0f;
            color->A = 1.0f;
        }

        glColor3f(1.0f, 1.0f, 1.0f);

        glDrawElements(GL_TRIANGLES, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_SHORT, indexes);

        return TRUE;
    }

    extern "C" u32 PolyListBasis(const RBVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (MAX_VERTEX_COUNT < vertexCount) { return FALSE; }

        GL::SelectRendererMode(mode);

        glEnableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(RBVX), &vertexes[0].XYZ);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RBVX), &vertexes[0].UV);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        if (State.GL.Textures.MultiBlendOperation == RendererBlendOperation::None)
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, sizeof(RBVX), &vertexes[0].Normal);
        }
        else
        {
            GL::GLF.glClientActiveTextureARB(GL_TEXTURE1_ARB);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, sizeof(RBVX), &vertexes[0].UV1);

            GL::GLF.glActiveTextureARB(GL_TEXTURE1_ARB);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            GL::GLF.glActiveTextureARB(GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glDisableClientState(GL_NORMAL_ARRAY);
        }

        if (State.GL.Light.Mode == RendererLightingMode::On)
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, sizeof(RBVX), &vertexes[0].Normal);
            glEnable(GL_LIGHTING);
        }
        else
        {
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisable(GL_LIGHTING);
        }

        glColor3f(1.0f, 1.0f, 1.0f);

        glDrawElements(GL_TRIANGLES, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_SHORT, indexes);

        if (State.GL.Textures.MultiBlendOperation != RendererBlendOperation::None)
        {
            GL::GLF.glClientActiveTextureARB(GL_TEXTURE1_ARB);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            GL::GLF.glClientActiveTextureARB(GL_TEXTURE0_ARB);
            GL::GLF.glActiveTextureARB(GL_TEXTURE0_ARB);

            return TRUE;
        }

        glDisableClientState(GL_NORMAL_ARRAY);

        return TRUE;
    }

    extern "C" u32 PolyListBone(const RBNVX*, const u32, const u16*, const u32, const f32x4*, const Renderers::RendererMode) { return FALSE; }

    extern "C" u32 PolyListD3DL(const RTLVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (MAX_VERTEX_COUNT < vertexCount) { return FALSE; }

        GL::SelectRendererMode(mode);

        glDisable(GL_LIGHTING);

        GL::GLF.glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glDisableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(RTLVX), &vertexes[0].XYZ);
        glColorPointer(4, GL_FLOAT, sizeof(f32xRGBA), &State.GL.Vertexes.Colors);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RTLVX), &vertexes[0].UV);

        for (u32 x = 0; x < vertexCount; x++)
        {
            auto vertex = &vertexes[x];
            auto color = &State.GL.Vertexes.Colors[x];

            color->A = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 24) & 0xff);
            color->R = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 16) & 0xff);
            color->G = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 8) & 0xff);
            color->B = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 0) & 0xff);
        }

        glDrawElements(GL_TRIANGLES, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_SHORT, indexes);

        return TRUE;
    }

    extern "C" u32 PolyListD3DTL(const RTLVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (MAX_VERTEX_COUNT < vertexCount) { return FALSE; }

        GL::SelectRendererMode(mode & (~(RendererMode::U0x80 | RendererMode::U0x40 | RendererMode::U0x8)));

        glDisable(GL_LIGHTING);

        GL::GLF.glClientActiveTextureARB(GL_TEXTURE1_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        GL::GLF.glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glDisableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(RTLVX), &vertexes[0].XYZ);
        glColorPointer(4, GL_FLOAT, sizeof(f32xRGBA), &State.GL.Vertexes.Colors);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RTLVX), &vertexes[0].UV);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();

        auto height = State.Window.Height;
        auto width = State.Window.Width;
        auto clippingNear = State.GL.Mode.Clipping.Near;
        auto clippingFar = State.GL.Mode.Clipping.Far;

        const Matrix4x4 matrix =
        {
             2.0f / width, 0.0f,           0.0f,                                                            0.0f,
             0.0f,         -2.0f / height, 0.0f,                                                            0.0f,
             0.0f,         0.0f,           2.0f / (clippingFar - clippingNear),                             0.0f,
             -1.0f,        1.0f,           -(clippingFar + clippingNear) / (clippingFar - clippingNear),    1.0f
        };

        glLoadMatrixf((f32*)&matrix);

        for (u32 x = 0; x < vertexCount; x++)
        {
            auto vertex = &vertexes[x];
            auto color = &State.GL.Vertexes.Colors[x];

            color->A = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 24) & 0xff);
            color->R = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 16) & 0xff);
            color->G = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 8) & 0xff);
            color->B = ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 0) & 0xff);
        }

        glDrawElements(GL_TRIANGLES, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_SHORT, indexes);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        return TRUE;
    }

    extern "C" u32 PolyListTL(const RCVX * vertexes, const u32 vertexCount, const u16 * indexes, const u32 indexCount, const RendererMode mode)
    {
        if (MAX_VERTEX_COUNT < vertexCount) { return FALSE; }

        GL::SelectRendererMode(mode);

        glDisable(GL_LIGHTING);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(RCVX), &vertexes[0].XYZW);
        glColorPointer(4, GL_FLOAT, sizeof(f32xRGBA), &State.GL.Vertexes.Colors);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RCVX), &vertexes[0].UV);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();

        const auto clippingNear = State.GL.Mode.Clipping.Near;
        const auto clippingFar = State.GL.Mode.Clipping.Far;

        const Matrix4x4 matrix =
        {
            1.0f, 0.0f, 0.0f,                                                       0.0f,
            0.0f, 1.0f, 0.0f,                                                       0.0f,
            0.0f, 0.0f, clippingFar / (clippingFar - clippingNear),                 1.0f,
            0.0f, 0.0f, -clippingNear * clippingFar / (clippingFar - clippingNear), 0.0f
        };

        glLoadMatrixf((f32*)&matrix);

        for (u32 x = 0; x < vertexCount; x++)
        {
            const auto vertex = &vertexes[x];

            auto color = &State.GL.Vertexes.Colors[x];

            color->R = vertex->Color.R;
            color->G = vertex->Color.G;
            color->B = vertex->Color.B;
            color->A = vertex->Color.A;
        }

        glDrawElements(GL_TRIANGLES, indexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT, GL_UNSIGNED_SHORT, indexes);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        return TRUE;
    }

    extern "C" u32 RenderOptimizedMesh(void) { return FALSE; }

    extern "C" u32 RenderVertexBuffer(const void*, const u32, const u16*, const u32, const RendererMode) { return FALSE; }

    extern "C" u32 RenderVertexBufferBasis(const void*, const u32, const u16*, const u32, const RendererMode) { return FALSE; }

    extern "C" u32 RenderVertexBufferBone(const void*, const u32, const u16*, const u32, const f32x4*, const u32, const Renderers::RendererMode) { return FALSE; }

    extern "C" u32 RenderVertexBufferPrelit(const void*, const u32, const u16*, const u32, const RendererMode) { return FALSE; }

    extern "C" u32 SetAlpha(const u32 color)
    {
        glAlphaFunc(GL_GREATER, color * ONE_TWO_HUNDRED_FIFTY_FIFTH);

        return TRUE;
    }

    extern "C" u32 SetAmbientLight(const f32) { return TRUE; }

    extern "C" u32 SetAmbientLightColor(const f32, const f32, const f32) { return TRUE; }

    extern "C" u32 SetColorTable(const u8*, u16*) { return FALSE; }

    extern "C" u32 SetDestinationBlend(const RendererDestinationBlend mode)
    {
        switch (mode)
        {
        case RendererDestinationBlend::InverseAlpha: { State.GL.Mode.DestinationBlendMode = GL_ONE_MINUS_SRC_ALPHA; break; }
        case RendererDestinationBlend::One: { State.GL.Mode.DestinationBlendMode = GL_ONE; break; }
        case RendererDestinationBlend::Color: { State.GL.Mode.DestinationBlendMode = GL_SRC_COLOR; break; }
        case RendererDestinationBlend::InverseColor: { State.GL.Mode.DestinationBlendMode = GL_ONE_MINUS_SRC_COLOR; break; }
        case RendererDestinationBlend::Zero: { State.GL.Mode.DestinationBlendMode = GL_ZERO; break; }
        }

        return TRUE;
    }

    extern "C" u32 SetDirtyRect(const u32, const u32, const u32, const u32) { return FALSE; }

    extern "C" u32 SetFog(const f32 r, const f32 g, const f32 b, const f32 start, const f32 end)
    {
        const f32 color[] = { r, g, b, 0.0f };

        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogfv(GL_FOG_COLOR, color);
        glFogf(GL_FOG_START, start);
        glFogf(GL_FOG_END, end);

        return TRUE;
    }

    extern "C" u32 SetGloss(const f32) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SetLightColor(const f32 r, const f32 g, const f32 b, const u32 indx)
    {
        State.GL.Light.Colors.R[indx] = r;
        State.GL.Light.Colors.G[indx] = g;
        State.GL.Light.Colors.B[indx] = b;

        return TRUE;
    }

    extern "C" u32 SetLightConstants(const f32, const f32, const f32, const f32 alpha)
    {
        State.GL.Light.Colors.Alpha = alpha;

        return TRUE;
    }

    extern "C" u32 SetLightVector(const f32x3*, const f32, const u32) { return TRUE; }

    extern "C" u32 SetMultiTextureBlend(const RendererBlendOperation operation)
    {
        State.GL.Textures.MultiBlendOperation = operation;

        return TRUE;
    }

    extern "C" u32 SetOmniLight(const f32xRGB*, const f32, const u32) { return FALSE; }

    extern "C" u32 SetSourceBlend(const RendererSourceBlendMode mode)
    {
        switch (mode)
        {
        case RendererSourceBlendMode::Alpha: { State.GL.Mode.SourceBlendMode = GL_SRC_ALPHA; break; }
        case RendererSourceBlendMode::One: { State.GL.Mode.SourceBlendMode = GL_ONE; break; }
        case RendererSourceBlendMode::Zero: { State.GL.Mode.SourceBlendMode = GL_ZERO; break; }
        }

        return TRUE;
    }

    extern "C" u32 SetStencilEnable(const RendererStencilMode) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SetStencilFunc(const RendererStencilFunction) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SetStencilPass(const RendererStencilPass) { return FALSE; } // NOTE: Not being called by the application.

    extern "C" u32 SetTextureClamp(const RendererClampMode s, const RendererClampMode t, const u32)
    {
        State.GL.Textures.Clamp.S = s;
        State.GL.Textures.Clamp.T = t;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s == RendererClampMode::Wrap ? GL_REPEAT : GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t == RendererClampMode::Wrap ? GL_REPEAT : GL_CLAMP);

        return TRUE;
    }

    extern "C" u32 SetTransform(const Transform * transform, const f32 zNear, const f32 zFar)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf((f32*)&transform->ModelView);

        const auto clippingNear = State.GL.Mode.Clipping.Near;
        const auto clippingFar = State.GL.Mode.Clipping.Far;

        State.GL.Transform.Projection.M11 = zNear;
        State.GL.Transform.Projection.M12 = 0.0f;
        State.GL.Transform.Projection.M13 = 0.0f;
        State.GL.Transform.Projection.M14 = 0.0f;

        State.GL.Transform.Projection.M21 = 0.0f;
        State.GL.Transform.Projection.M22 = State.GL.Textures.Render.Direction * zFar;
        State.GL.Transform.Projection.M23 = 0.0f;
        State.GL.Transform.Projection.M24 = 0.0f;

        State.GL.Transform.Projection.M31 = 0.0f;
        State.GL.Transform.Projection.M32 = 0.0f;
        State.GL.Transform.Projection.M33 = (clippingNear + clippingFar) / (clippingFar - clippingNear);
        State.GL.Transform.Projection.M34 = 1.0f;

        State.GL.Transform.Projection.M41 = 0.0f;
        State.GL.Transform.Projection.M42 = 0.0f;
        State.GL.Transform.Projection.M43 = (2.0f * clippingNear * clippingFar) / (clippingNear - clippingFar);
        State.GL.Transform.Projection.M44 = 0.0f;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glLoadMatrixf((f32*)&State.GL.Transform.Projection);

        CopyMemory(&State.GL.Transform.ModelView, &transform->ModelView, sizeof(Matrix4x4));

        for (u32 x = 0; x < MAX_LIGHT_COUNT; x++)
        {
            auto light = &State.GL.Light.Lights[x];
            auto position = &transform->Light.XYZ[x];

            auto length = sqrtf(position->X * position->X + position->Y * position->Y + position->Z * position->Z);

            if (length == 0.0f) { length = 1.0f; }

            light->Position.X = -1.0f / length * position->X;
            light->Position.Y = -1.0f / length * position->Y;
            light->Position.Z = -1.0f / length * position->Z;
            light->Position.W = 0.0f;

            light->Color.R = length * State.GL.Light.Colors.R[x];
            light->Color.G = length * State.GL.Light.Colors.G[x];
            light->Color.B = length * State.GL.Light.Colors.B[x];
            light->Color.A = 1.0f;
        }

        glLightfv(GL_LIGHT0, GL_POSITION, (f32*)&State.GL.Light.Lights[0].Position);
        glLightfv(GL_LIGHT1, GL_POSITION, (f32*)&State.GL.Light.Lights[1].Position);
        glLightfv(GL_LIGHT2, GL_POSITION, (f32*)&State.GL.Light.Lights[2].Position);

        glLightfv(GL_LIGHT0, GL_DIFFUSE, (f32*)&State.GL.Light.Lights[0].Color);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, (f32*)&State.GL.Light.Lights[1].Color);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, (f32*)&State.GL.Light.Lights[2].Color);

        return TRUE;
    }

    extern "C" u32 SetTransformO2W(const Matrix4x4*) { return FALSE; }

    extern "C" u32 SetTransformW2L(const Matrix4x4*, const f32, const f32) { return FALSE; }

    extern "C" u32 SetViewPort(const f32 x0, const f32 y0, const f32 x1, const f32 y1, const f32 zNear, const f32 zFar)
    {
        glViewport((s32)x0, (s32)y0, (s32)(x1 - x0), (s32)(y1 - y0));

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);

        State.GL.Mode.Clipping.Near = zNear;
        State.GL.Mode.Clipping.Far = zFar;

        return TRUE;
    }

    extern "C" u32 Toggle(void)
    {
        SwapBuffers(State.Window.DeviceContext);

        return TRUE;
    }

    extern "C" u32 UnlockFrame(void) { return FALSE; }
}