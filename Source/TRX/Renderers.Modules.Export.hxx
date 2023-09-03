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
#include "Native.Basic.hxx"
#include "Renderers.Basic.hxx"
#include "Renderers.Basic.Textures.hxx"
#include "Renderers.Basic.Vertexes.hxx"
#include "Renderers.Modules.Basic.hxx"

namespace Renderers::Modules
{
    extern "C" u32 CDECLAPI AcquireGraphicsCardList(u32*, char**, char**, u32*, u32*);
    extern "C" u32 CDECLAPI AcquireModuleName(char*);
    extern "C" u32 CDECLAPI AllocateOptimizedMesh(void);
    extern "C" u32 CDECLAPI AllocateTexture(const Assets::Texture*);
    extern "C" u32 CDECLAPI AllocateVertexBuffer(const void*, const u32, const u32, void**);
    extern "C" u32 CDECLAPI BeginCubeMapRender(const u32, const Renderers::RendererCubeMapSide);
    extern "C" u32 CDECLAPI BeginRenderToTexture(const u32);
    extern "C" u32 CDECLAPI BeginScene(void);
    extern "C" u32 CDECLAPI Clear(const Renderers::RendererClearMode, const u32);
    extern "C" u32 CDECLAPI ClearZBuffer(void);
    extern "C" u32 CDECLAPI CopyBackBufferToRenderTexture(void);
    extern "C" u32 CDECLAPI CreateGameWindow(const u32, const u32, const u32, u32**, const HWND);
    extern "C" u32 CDECLAPI DestroyGameWindow(const u32);
    extern "C" u32 CDECLAPI DotListD3DL(const Renderers::RLVX*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI DotListD3DTL(const Renderers::RTLVX*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI DrawPixels(void);
    extern "C" u32 CDECLAPI EnableClipping(const Renderers::RendererClippingMode);
    extern "C" u32 CDECLAPI EnableCulling(const Renderers::RendererCullMode);
    extern "C" u32 CDECLAPI EnableLighting(const Renderers::RendererLightingMode);
    extern "C" u32 CDECLAPI EndCubeMapRender(void);
    extern "C" u32 CDECLAPI EndRenderToTexture(void);
    extern "C" u32 CDECLAPI EndScene(void);
    extern "C" u32 CDECLAPI FreeOptimizedMesh(const u32);
    extern "C" u32 CDECLAPI FreeTexture(const u32);
    extern "C" u32 CDECLAPI FreeVertexBuffer(const void*);
    extern "C" u32 CDECLAPI GetTextureInfo(const u32);
    extern "C" u32 CDECLAPI HasShaderSupport(void);
    extern "C" u32 CDECLAPI Init(const HWND, const Renderers::Modules::RendererModuleInitializeArguments*);
    extern "C" u32 CDECLAPI IsGameWindowMode(void);
    extern "C" u32 CDECLAPI Kill(void);
    extern "C" u32 CDECLAPI LineListD3DL(const Renderers::RLVX*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI LineListD3DTL(const Renderers::RTLVX*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI LockFrame(void);
    extern "C" u32 CDECLAPI LockFrameForScreenshot(void);
    extern "C" u32 CDECLAPI PolyList(const Renderers::RVX*, const u32, const u16*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI PolyListBasis(const Renderers::RBVX*, const u32, const u16*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI PolyListBone(const Renderers::RBNVX*, const u32, const u16*, const u32, const f32x4 *, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI PolyListD3DL(const Renderers::RTLVX*, const u32, const u16*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI PolyListD3DTL(const Renderers::RTLVX*, const u32, const u16*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI PolyListTL(const Renderers::RCVX*, const u32, const u16*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI RenderOptimizedMesh(void);
    extern "C" u32 CDECLAPI RenderVertexBuffer(const void*, const u32, const u16*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI RenderVertexBufferBasis(const void*, const u32, const u16*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI RenderVertexBufferBone(const void*, const u32, const u16*, const u32, const f32x4*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI RenderVertexBufferPrelit(const void*, const u32, const u16*, const u32, const Renderers::RendererMode);
    extern "C" u32 CDECLAPI ResetTextureCache(void);
    extern "C" u32 CDECLAPI RestoreVideoMode(void);
    extern "C" u32 CDECLAPI SelectCubeTexture(const u32, const u32);
    extern "C" u32 CDECLAPI SelectGameWindow(const u32, u32**);
    extern "C" u32 CDECLAPI SelectGraphicsCard(const u32);
    extern "C" u32 CDECLAPI SelectRenderTexture(const u32);
    extern "C" u32 CDECLAPI SelectTexture(const u32, const u32);
    extern "C" u32 CDECLAPI SelectVideoMode(const u32, const u32, const u32, u32**, const u32);
    extern "C" u32 CDECLAPI SetAlpha(const u32);
    extern "C" u32 CDECLAPI SetAmbientLight(const f32);
    extern "C" u32 CDECLAPI SetAmbientLightColor(const f32, const f32, const f32);
    extern "C" u32 CDECLAPI SetColorTable(const u8*, u16*);
    extern "C" u32 CDECLAPI SetDestinationBlend(const Renderers::RendererDestinationBlend);
    extern "C" u32 CDECLAPI SetDirtyRect(const u32, const u32, const u32, const u32);
    extern "C" u32 CDECLAPI SetFog(const f32, const f32, const f32, const f32, const f32);
    extern "C" u32 CDECLAPI SetGameWindowMode(const Renderers::RendererWindowMode);
    extern "C" u32 CDECLAPI SetGloss(const f32);
    extern "C" u32 CDECLAPI SetLightColor(const f32, const f32, const f32, const u32);
    extern "C" u32 CDECLAPI SetLightConstants(const f32, const f32, const f32, const f32);
    extern "C" u32 CDECLAPI SetLightVector(const f32x3*, const f32, const u32);
    extern "C" u32 CDECLAPI SetMultiTextureBlend(const Renderers::RendererBlendOperation);
    extern "C" u32 CDECLAPI SetOmniLight(const Graphics::f32xRGB*, const f32, const u32);
    extern "C" u32 CDECLAPI SetSourceBlend(const Renderers::RendererSourceBlendMode);
    extern "C" u32 CDECLAPI SetStencilEnable(const Renderers::RendererStencilMode);
    extern "C" u32 CDECLAPI SetStencilFunc(const Renderers::RendererStencilFunction);
    extern "C" u32 CDECLAPI SetStencilPass(const Renderers::RendererStencilPass);
    extern "C" u32 CDECLAPI SetTextureClamp(const Renderers::RendererClampMode, const Renderers::RendererClampMode, const u32);
    extern "C" u32 CDECLAPI SetTransform(const Renderers::Transform*, const f32, const f32);
    extern "C" u32 CDECLAPI SetTransformO2W(const Mathematics::Matrix4x4*);
    extern "C" u32 CDECLAPI SetTransformW2L(const Mathematics::Matrix4x4*, const f32, const f32);
    extern "C" u32 CDECLAPI SetViewPort(const f32, const f32, const f32, const f32, const f32, const f32);
    extern "C" u32 CDECLAPI Toggle(void);
    extern "C" u32 CDECLAPI UnlockFrame(void);
    extern "C" u32 CDECLAPI UploadTexture(const u32, const Assets::Texture*);
    extern "C" u32 CDECLAPI VideoRestore(void);
    extern "C" u32 CDECLAPI VideoSave(void);
}