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
#include "Renderers.Basic.hxx"
#include "Renderers.Basic.Textures.hxx"
#include "Renderers.Basic.Vertexes.hxx"
#include "Renderers.Modules.Basic.hxx"

#define RENDERER_MODULE_ACQUIRE_GAME_WINDOW_MODE_NAME "APIDLLisInWindowMode"
#define RENDERER_MODULE_ACQUIRE_GRAPHICS_CARD_LIST_NAME "APIDLLbuildCardList"
#define RENDERER_MODULE_ACQUIRE_MODULE_NAME_NAME "APIDLLacquireModuleName"
#define RENDERER_MODULE_ACQUIRE_TEXTURE_INFORMATION_NAME "APIDLLgetTextureInfo"
#define RENDERER_MODULE_ALLOCATE_OPTIMIZED_MESH_NAME "APIDLLallocOptimizedMesh"
#define RENDERER_MODULE_ALLOCATE_TEXTURE_NAME "APIDLLallocateTexture"
#define RENDERER_MODULE_ALLOCATE_VERTEX_BUFFER_NAME "APIDLLallocVertexBuffer"
#define RENDERER_MODULE_BEGIN_CUBE_MAP_RENDER_NAME "APIDLLbeginCubeMapRender"
#define RENDERER_MODULE_BEGIN_RENDER_TO_TEXTURE_NAME "APIDLLbeginRenderToTexture"
#define RENDERER_MODULE_BEGIN_SCENE_NAME "APIDLLbeginScene"
#define RENDERER_MODULE_CLEAR_NAME "APIDLLclear"
#define RENDERER_MODULE_CLEAR_Z_BUFFER_NAME "APIDLLclearZBuffer"
#define RENDERER_MODULE_COPY_BACK_BUFFER_TO_RENDER_TEXTURE_NAME "APIDLLcopyBackBufferToRenderTexture"
#define RENDERER_MODULE_CREATE_GAME_WINDOW_NAME "APIDLLcreateWindow"
#define RENDERER_MODULE_DESTROY_GAME_WINDOW_NAME "APIDLLdestroyWindow"
#define RENDERER_MODULE_DOT_LIST_D3DL_NAME "APIDLLdotListD3DL"
#define RENDERER_MODULE_DOT_LIST_D3DTL_NAME "APIDLLdotListD3DTL"
#define RENDERER_MODULE_DRAW_PIXELS_NAME "APIDLLdrawPixels"
#define RENDERER_MODULE_ENABLE_CLIPPING_NAME "APIDLLenableClipping"
#define RENDERER_MODULE_ENABLE_CULLING_NAME "APIDLLenableCulling"
#define RENDERER_MODULE_ENABLE_LIGHTING_NAME "APIDLLenableLighting"
#define RENDERER_MODULE_END_CUBE_MAP_RENDER_NAME "APIDLLendCubeMapRender"
#define RENDERER_MODULE_END_RENDER_TO_TEXTURE_NAME "APIDLLendRenderToTexture"
#define RENDERER_MODULE_END_SCENE_NAME "APIDLLendScene"
#define RENDERER_MODULE_FREE_OPTIMIZED_MESH_NAME "APIDLLfreeOptimizedMesh"
#define RENDERER_MODULE_FREE_TEXTURE_NAME "APIDLLfreeTexture"
#define RENDERER_MODULE_FREE_VERTEX_BUFFER_NAME "APIDLLfreeVertexBuffer"
#define RENDERER_MODULE_HAS_SHADER_SUPPORT_NAME "APIDLLhasShaderSupport"
#define RENDERER_MODULE_INIT_NAME "APIDLLinit"
#define RENDERER_MODULE_KILL_NAME "APIDLLkill"
#define RENDERER_MODULE_LINE_LIST_D3DL_NAME "APIDLLlineListD3DL"
#define RENDERER_MODULE_LINE_LIST_D3DTL_NAME "APIDLLlineListD3DTL"
#define RENDERER_MODULE_LOCK_FRAME_FOR_SCREENSHOT_NAME "APIDLLlockFrameForScreenshot"
#define RENDERER_MODULE_LOCK_FRAME_NAME "APIDLLlockFrame"
#define RENDERER_MODULE_POLY_LIST_BASIS_NAME "APIDLLpolyListBasis"
#define RENDERER_MODULE_POLY_LIST_BONE_NAME "APIDLLpolyListBone"
#define RENDERER_MODULE_POLY_LIST_D3DL_NAME "APIDLLpolyListD3DL"
#define RENDERER_MODULE_POLY_LIST_D3DTL_NAME "APIDLLpolyListD3DTL"
#define RENDERER_MODULE_POLY_LIST_NAME "APIDLLpolyList"
#define RENDERER_MODULE_POLY_LIST_TL_NAME "APIDLLpolyListTL"
#define RENDERER_MODULE_RENDER_OPTIMIZED_MESH_NAME "APIDLLrenderOptimizedMesh"
#define RENDERER_MODULE_RENDER_VERTEX_BUFFER_BASIS_NAME "APIDLLrenderVertexBufferBasis"
#define RENDERER_MODULE_RENDER_VERTEX_BUFFER_BONE_NAME "APIDLLrenderVertexBufferBone"
#define RENDERER_MODULE_RENDER_VERTEX_BUFFER_NAME "APIDLLrenderVertexBuffer"
#define RENDERER_MODULE_RENDER_VERTEX_BUFFER_PRELIT_NAME "APIDLLrenderVertexBufferPrelit"
#define RENDERER_MODULE_RESET_TEXTURE_CACHE_NAME "APIDLLresetTextureCache"
#define RENDERER_MODULE_RESTORE_VIDEO_MODE_NAME "APIDLLrestoreVideoMode"
#define RENDERER_MODULE_SELECT_CUBE_TEXTURE_NAME "APIDLLselectCubeTexture"
#define RENDERER_MODULE_SELECT_GAME_WINDOW_MODE_NAME "APIDLLsetWindowMode"
#define RENDERER_MODULE_SELECT_GAME_WINDOW_NAME "APIDLLselectWindow"
#define RENDERER_MODULE_SELECT_GRAPHICS_CARD_NAME "APIDLLselectCard"
#define RENDERER_MODULE_SELECT_RENDER_TEXTURE_NAME "APIDLLselectRenderTexture"
#define RENDERER_MODULE_SELECT_TEXTURE_NAME "APIDLLselectTexture"
#define RENDERER_MODULE_SELECT_VIDEO_MODE_NAME "APIDLLsetVideoMode2"
#define RENDERER_MODULE_SET_ALPHA_NAME "APIDLLsetAlphaRef"
#define RENDERER_MODULE_SET_AMBIENT_LIGHT_COLOR_NAME "APIDLLsetAmbientLightColor"
#define RENDERER_MODULE_SET_AMBIENT_LIGHT_NAME "APIDLLsetAmbientLight"
#define RENDERER_MODULE_SET_COLOR_TABLE_NAME "APIDLLsetColorTable16"
#define RENDERER_MODULE_SET_DESTINATION_BLEND_NAME "APIDLLsetDestBlend"
#define RENDERER_MODULE_SET_DIRTY_RECT_NAME "APIDLLsetDirtyRect"
#define RENDERER_MODULE_SET_FOG_NAME "APIDLLsetFog"
#define RENDERER_MODULE_SET_GLOSS_NAME "APIDLLsetGloss"
#define RENDERER_MODULE_SET_LIGHT_COLOR_NAME "APIDLLsetLightColor"
#define RENDERER_MODULE_SET_LIGHT_CONSTANTS_NAME "APIDLLsetLightConstants"
#define RENDERER_MODULE_SET_LIGHT_VECTOR_NAME "APIDLLsetLightVector"
#define RENDERER_MODULE_SET_MULTI_TEXTURE_BLEND_NAME "APIDLLsetMultiTextureBlend"
#define RENDERER_MODULE_SET_OMNI_LIGHT_NAME "APIDLLsetOmniLight"
#define RENDERER_MODULE_SET_SOURCE_BLEND_NAME "APIDLLsetSrcBlend"
#define RENDERER_MODULE_SET_STENCIL_ENABLE_NAME "APIDLLsetStencilEnable"
#define RENDERER_MODULE_SET_STENCIL_FUNC_NAME "APIDLLsetStencilFunc"
#define RENDERER_MODULE_SET_STENCIL_PASS_NAME "APIDLLsetStencilPass"
#define RENDERER_MODULE_SET_TEXTURE_CLAMP_NAME "APIDLLsetTextureClamp"
#define RENDERER_MODULE_SET_TRANSFORM_NAME "APIDLLsetTransform"
#define RENDERER_MODULE_SET_TRANSFORM_O2W_NAME "APIDLLsetTransformO2W"
#define RENDERER_MODULE_SET_TRANSFORM_W2L_NAME "APIDLLsetTransformW2L"
#define RENDERER_MODULE_SET_VIEWPORT_NAME "APIDLLsetViewport"
#define RENDERER_MODULE_TOGGLE_NAME "APIDLLtoggle"
#define RENDERER_MODULE_UNLOCK_FRAME_NAME "APIDLLunlockFrame"
#define RENDERER_MODULE_UPLOAD_TEXTURE_NAME "APIDLLuploadTexture"
#define RENDERER_MODULE_VIDEO_RESTORE_NAME "APIDLLvideoRestore"
#define RENDERER_MODULE_VIDEO_SAVE_NAME "APIDLLvideoSave"

namespace Renderers::Modules
{
    typedef const u32(CDECLAPI* RENDERERACQUIREGAMEWINDOWMODE) (void);
    typedef const u32(CDECLAPI* RENDERERACQUIREGRAPHICSCARDLIST) (u32*, char**, char**, u32*, u32*);
    typedef const u32(CDECLAPI* RENDERERACQUIREMODULENAME) (char*);
    typedef const u32(CDECLAPI* RENDERERACQUIRETEXTUREINFORMATION) (const u32);
    typedef const u32(CDECLAPI* RENDERERALLOCATEOPTIMIZEDMESH) (void);
    typedef const u32(CDECLAPI* RENDERERALLOCATETEXTURE) (const Assets::Texture*);
    typedef const u32(CDECLAPI* RENDERERALLOCATEVERTEXBUFFER) (const void*, const u32, const u32, void**);
    typedef const u32(CDECLAPI* RENDERERBEGINCUBEMAPRENDER) (const u32, const Renderers::RendererCubeMapSide);
    typedef const u32(CDECLAPI* RENDERERBEGINRENDERTOTEXTURE) (const u32);
    typedef const u32(CDECLAPI* RENDERERBEGINSCENE) (void);
    typedef const u32(CDECLAPI* RENDERERCLEAR) (const Renderers::RendererClearMode, const u32);
    typedef const u32(CDECLAPI* RENDERERCLEARZBUFFER) (void);
    typedef const u32(CDECLAPI* RENDERERCOPYBACKBUFFERTORENDERTEXTURE) (void);
    typedef const u32(CDECLAPI* RENDERERCREATEGAMEWINDOW) (const u32, const u32, const u32, void**, const HWND);
    typedef const u32(CDECLAPI* RENDERERDESTROYGAMEWINDOW) (const u32);
    typedef const u32(CDECLAPI* RENDERERDOTLISTD3DL) (const Renderers::RLVX*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERDOTLISTD3DTL) (const Renderers::RTLVX*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERDRAWPIXELS) (void);
    typedef const u32(CDECLAPI* RENDERERENABLECLIPPING) (const Renderers::RendererClippingMode);
    typedef const u32(CDECLAPI* RENDERERENABLECULLING) (const Renderers::RendererCullMode);
    typedef const u32(CDECLAPI* RENDERERENABLELIGHTING) (const Renderers::RendererLightingMode);
    typedef const u32(CDECLAPI* RENDERERENDCUBEMAPRENDER) (void);
    typedef const u32(CDECLAPI* RENDERERENDRENDERTOTEXTURE) (void);
    typedef const u32(CDECLAPI* RENDERERENDSCENE) (void);
    typedef const u32(CDECLAPI* RENDERERFREEOPTIMIZEDMESH) (void);
    typedef const u32(CDECLAPI* RENDERERFREETEXTURE) (const u32);
    typedef const u32(CDECLAPI* RENDERERFREEVERTEXBUFFER) (const void*);
    typedef const u32(CDECLAPI* RENDERERHASSHADERSUPPORT) (void);
    typedef const u32(CDECLAPI* RENDERERINIT) (const HWND, const Renderers::Modules::RendererModuleInitializeArguments*);
    typedef const u32(CDECLAPI* RENDERERKILL) (void);
    typedef const u32(CDECLAPI* RENDERERLINELISTD3DL) (const Renderers::RLVX*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERLINELISTD3DTL) (const Renderers::RTLVX*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERLOCKFRAME) (void);
    typedef const u32(CDECLAPI* RENDERERLOCKFRAMEFORSCREENSHOT)(void);
    typedef const u32(CDECLAPI* RENDERERPOLYLIST) (const Renderers::RVX*, const u32, const u16*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERPOLYLISTBASIS) (const Renderers::RBVX*, const u32, const u16*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERPOLYLISTBONE) (const Renderers::RBNVX*, const u32, const u16*, const u32, const f32x4 *, const u32);
    typedef const u32(CDECLAPI* RENDERERPOLYLISTD3DL) (const Renderers::RTLVX*, const u32, const u16*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERPOLYLISTD3DTL) (const Renderers::RTLVX*, const u32, const u16*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERPOLYLISTTL) (const Renderers::RCVX*, const u32, const u16*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERRENDEROPTIMIZEDMESH) (void);
    typedef const u32(CDECLAPI* RENDERERRENDERVERTEXBUFFER) (const void*, const u32, const u16*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERRENDERVERTEXBUFFERBASIS) (const void*, const u32, const u16*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERRENDERVERTEXBUFFERBONE) (const void*, const u32, const u16*, const u32, const f32x4*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERRENDERVERTEXBUFFERPRELIT) (const void*, const u32, const u16*, const u32, const Renderers::RendererMode);
    typedef const u32(CDECLAPI* RENDERERRESETTEXTURECACHE) (void);
    typedef const u32(CDECLAPI* RENDERERRESTOREVIDEOMODE) (void);
    typedef const u32(CDECLAPI* RENDERERSELECTCUBETEXTURE) (const u32, const u32);
    typedef const u32(CDECLAPI* RENDERERSELECTGAMEWINDOW) (const u32, void**);
    typedef const u32(CDECLAPI* RENDERERSELECTGAMEWINDOWMODE) (const Renderers::RendererWindowMode);
    typedef const u32(CDECLAPI* RENDERERSELECTGRAPHICSCARD) (const u32);
    typedef const u32(CDECLAPI* RENDERERSELECTRENDERTEXTURE) (const u32);
    typedef const u32(CDECLAPI* RENDERERSELECTTEXTURE) (const u32, const u32);
    typedef const u32(CDECLAPI* RENDERERSELECTVIDEOMODE) (const u32, const u32, const u32, u32**, const u32);
    typedef const u32(CDECLAPI* RENDERERSETALPHA) (const u32);
    typedef const u32(CDECLAPI* RENDERERSETAMBIENTLIGHT) (const f32);
    typedef const u32(CDECLAPI* RENDERERSETAMBIENTLIGHTCOLOR) (const f32, const f32, const f32);
    typedef const u32(CDECLAPI* RENDERERSETCOLORTABLE) (const u8*, u16*);
    typedef const u32(CDECLAPI* RENDERERSETDESTINATIONBLEND) (const Renderers::RendererDestinationBlend);
    typedef const u32(CDECLAPI* RENDERERSETDIRTYRECT) (const u32, const u32, const u32, const u32);
    typedef const u32(CDECLAPI* RENDERERSETFOG) (const f32, const f32, const f32, const f32, const f32);
    typedef const u32(CDECLAPI* RENDERERSETGLOSS) (const f32);
    typedef const u32(CDECLAPI* RENDERERSETLIGHTCOLOR) (const f32, const f32, const f32, const u32);
    typedef const u32(CDECLAPI* RENDERERSETLIGHTCONSTANTS) (const f32, const f32, const f32, const f32);
    typedef const u32(CDECLAPI* RENDERERSETLIGHTVECTOR) (const f32x3*, const f32, const u32);
    typedef const u32(CDECLAPI* RENDERERSETMULTITEXTUREBLEND) (const Renderers::RendererBlendOperation);
    typedef const u32(CDECLAPI* RENDERERSETOMNILIGHT) (const Graphics::f32xRGB*, const f32, const u32);
    typedef const u32(CDECLAPI* RENDERERSETSOURCEBLEND) (const Renderers::RendererSourceBlendMode);
    typedef const u32(CDECLAPI* RENDERERSETSTENCILENABLE) (const Renderers::RendererStencilMode);
    typedef const u32(CDECLAPI* RENDERERSETSTENCILFUNC) (const Renderers::RendererStencilFunction);
    typedef const u32(CDECLAPI* RENDERERSETSTENCILPASS) (const Renderers::RendererStencilPass);
    typedef const u32(CDECLAPI* RENDERERSETTEXTURECLAMP) (const Renderers::RendererClampMode, const Renderers::RendererClampMode, const u32);
    typedef const u32(CDECLAPI* RENDERERSETTRANSFORM) (const Renderers::Transform*, const f32, const f32);
    typedef const u32(CDECLAPI* RENDERERSETTRANSFORMO2W) (const Mathematics::Matrix4x4*);
    typedef const u32(CDECLAPI* RENDERERSETTRANSFORMW2L) (const Mathematics::Matrix4x4*, const f32, const f32);
    typedef const u32(CDECLAPI* RENDERERSETVIEWPORT) (const f32, const f32, const f32, const f32, const f32, const f32);
    typedef const u32(CDECLAPI* RENDERERTOGGLE) (void);
    typedef const u32(CDECLAPI* RENDERERUNLOCKFRAME) (void);
    typedef const u32(CDECLAPI* RENDERERUPLOADTEXTURE) (const u32, const Assets::Texture*);
    typedef const u32(CDECLAPI* RENDERERVIDEORESTORE) (void);
    typedef const u32(CDECLAPI* RENDERERVIDEOSAVE) (void);
}