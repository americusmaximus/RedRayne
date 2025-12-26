#include "Renderer.hxx"
#include "Renderer.Module.hxx"
#include "Native.hxx"
#include "App.Windows.hxx"

using namespace App::Windows;
using namespace Renderers::Modules;
using namespace Native;

namespace Renderer::Module
{
    RendererModuleContainer RendererModule;

    void ReleaseRendererModule(void)
    {
        *RendererState._IsActive = FALSE;
        *RendererModule.State._IsActive = FALSE;

        if (*RendererModule.State._Handle != NULL)
        {
            ReleaseModule(*RendererModule.State._Handle);

            *RendererModule.State._Handle = NULL;
        }

        // TODO sort a-z once all * are gone.
        RendererModule.AllocateOptimizedMesh = NULL;
        RendererModule.AllocateTexture = NULL;
        *RendererModule._AllocateVertexBuffer = NULL;
        RendererModule.BeginCubeMapRender = NULL;
        RendererModule.BeginRenderToTexture = NULL;
        RendererModule.BeginScene = NULL;
        RendererModule.Clear = NULL;
        RendererModule.ClearZBuffer = NULL;
        RendererModule.CopyBackBufferToRenderTexture = NULL;
        RendererModule.CreateGameWindow = NULL;
        RendererModule.AcquireGraphicsCardList = NULL;
        RendererModule.DestroyGameWindow = NULL;
        *RendererModule._DotListD3DL = NULL;
        *RendererModule._DotListD3DTL = NULL;
        *RendererModule._DrawPixels = NULL;
        *RendererModule._EnableClipping = NULL;
        RendererModule.EnableCulling = NULL;
        RendererModule.EnableLighting = NULL;
        RendererModule.EndCubeMapRender = NULL;
        RendererModule.EndRenderToTexture = NULL;
        RendererModule.EndScene = NULL;
        *RendererModule._FreeOptimizedMesh = NULL;
        RendererModule.FreeTexture = NULL;
        *RendererModule._FreeVertexBuffer = NULL;
        RendererModule.AcquireModuleName = NULL;
        RendererModule.AcquireTextureInformation = NULL;
        RendererModule.HasShaderSupport = NULL;
        RendererModule.Init = NULL;
        RendererModule.AcquireGameWindowMode = NULL;
        RendererModule.Kill = NULL;
        *RendererModule._LineListD3DL = NULL;
        *RendererModule._LineListD3DTL = NULL;
        RendererModule.LockFrame = NULL;
        RendererModule.LockFrameForScreenshot = NULL;
        RendererModule.PolyList = NULL;
        RendererModule.PolyListBasis = NULL;
        RendererModule.PolyListBone = NULL;
        *RendererModule._PolyListD3DL = NULL;
        *RendererModule._PolyListD3DTL = NULL;
        *RendererModule._PolyListTL = NULL;
        *RendererModule._RenderOptimizedMesh = NULL;
        *RendererModule._RenderVertexBuffer = NULL;
        *RendererModule._RenderVertexBufferBasis = NULL;
        *RendererModule._RenderVertexBufferBone = NULL;
        *RendererModule._RenderVertexBufferPrelit = NULL;
        RendererModule.ResetTextureCache = NULL;
        RendererModule.RestoreVideoMode = NULL;
        RendererModule.SelectCubeTexture = NULL;
        RendererModule.SelectGameWindow = NULL;
        RendererModule.SelectGraphicsCard = NULL;
        *RendererModule._SelectRenderTexture = NULL;
        RendererModule.SelectTexture = NULL;
        RendererModule.SetAlpha = NULL;
        RendererModule.SetAmbientLight = NULL;
        RendererModule.SetAmbientLightColor = NULL;
        *RendererModule._SetColorTable = NULL;
        RendererModule.SetDestinationBlend = NULL;
        RendererModule.SetDirtyRect = NULL;
        RendererModule.SetFog = NULL;
        RendererModule.SelectGameWindowMode = NULL;
        RendererModule.SetGloss = NULL;
        RendererModule.SetLightColor = NULL;
        RendererModule.SetLightConstants = NULL;
        RendererModule.SetLightVector = NULL;
        RendererModule.SetMultiTextureBlend = NULL;
        RendererModule.SetOmniLight = NULL;
        RendererModule.SetSourceBlend = NULL;
        RendererModule.SetStencilEnable = NULL;
        RendererModule.SetStencilFunc = NULL;
        RendererModule.SetStencilPass = NULL;
        RendererModule.SetTextureClamp = NULL;
        RendererModule.SetTransform = NULL;
        RendererModule.SetTransformO2W = NULL;
        RendererModule.SetTransformW2L = NULL;
        RendererModule.SelectVideoMode = NULL;
        RendererModule.SetViewPort = NULL;
        RendererModule.Toggle = NULL;
        RendererModule.UnlockFrame = NULL;
        *RendererModule._UploadTexture = NULL;
        RendererModule.VideoRestore = NULL;
        RendererModule.VideoSave = NULL;
    }

    BOOL InitializeRendererModule2(const HWND hwnd)
    {
        BOOL invalid = FALSE;

        if (hwnd != NULL) { *RendererModule.State._HWND = hwnd; }

        if (!*RendererState.Options._IsUsingModule) { return FALSE; }

        ResetRendererTextureCache1();

        *RendererModule.State._Handle = InitializeModule(RendererState.Options._ModuleFileName);

        if (*RendererModule.State._Handle == NULL)
        {
            *RendererState.Options._IsUsingModule = FALSE;

            return FALSE;
        }

        RendererModule.Init = (RENDERERINIT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_INIT_NAME);
        if (RendererModule.Init == NULL) { invalid = TRUE; }

        RendererModule.Kill = (RENDERERKILL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_KILL_NAME);
        if (RendererModule.Kill == NULL) { invalid = TRUE; }

        RendererModule.Toggle = (RENDERERTOGGLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_TOGGLE_NAME);
        if (RendererModule.Toggle == NULL) { invalid = TRUE; }

        RendererModule.AcquireGameWindowMode = (RENDERERACQUIREGAMEWINDOWMODE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_GAME_WINDOW_MODE_NAME);
        RendererModule.LockFrameForScreenshot = (RENDERERLOCKFRAMEFORSCREENSHOT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LOCK_FRAME_FOR_SCREENSHOT_NAME);
        RendererModule.SetDirtyRect = (RENDERERSETDIRTYRECT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_DIRTY_RECT_NAME);
        RendererModule.SelectGameWindowMode = (RENDERERSELECTGAMEWINDOWMODE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_GAME_WINDOW_MODE_NAME);
        RendererModule.CreateGameWindow = (RENDERERCREATEGAMEWINDOW)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_CREATE_GAME_WINDOW_NAME);
        RendererModule.DestroyGameWindow = (RENDERERDESTROYGAMEWINDOW)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DESTROY_GAME_WINDOW_NAME);
        RendererModule.SelectGameWindow = (RENDERERSELECTGAMEWINDOW)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_GAME_WINDOW_NAME);

        RendererModule.SelectVideoMode = (RENDERERSELECTVIDEOMODE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_VIDEO_MODE_NAME);
        if (RendererModule.SelectVideoMode == NULL) { invalid = TRUE; }

        RendererModule.RestoreVideoMode = (RENDERERRESTOREVIDEOMODE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RESTORE_VIDEO_MODE_NAME);
        if (RendererModule.RestoreVideoMode == NULL) { invalid = TRUE; }

        RendererModule.BeginScene = (RENDERERBEGINSCENE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_BEGIN_SCENE_NAME);
        if (RendererModule.BeginScene == NULL) { invalid = TRUE; }

        RendererModule.EndScene = (RENDERERENDSCENE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_END_SCENE_NAME);
        if (RendererModule.EndScene == NULL) { invalid = TRUE; }

        RendererModule.LockFrame = (RENDERERLOCKFRAME)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LOCK_FRAME_NAME);
        if (RendererModule.LockFrame == NULL) { invalid = TRUE; }

        RendererModule.UnlockFrame = (RENDERERUNLOCKFRAME)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_UNLOCK_FRAME_NAME);
        if (RendererModule.UnlockFrame == NULL) { invalid = TRUE; }

        RendererModule.AllocateTexture = (RENDERERALLOCATETEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ALLOCATE_TEXTURE_NAME);
        if (RendererModule.AllocateTexture == NULL) { invalid = TRUE; }

        RendererModule.SelectTexture = (RENDERERSELECTTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_TEXTURE_NAME);
        if (RendererModule.SelectTexture == NULL) { invalid = TRUE; }

        *RendererModule._UploadTexture = (RENDERERUPLOADTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_UPLOAD_TEXTURE_NAME);
        if (*RendererModule._UploadTexture == NULL) { invalid = TRUE; }

        RendererModule.FreeTexture = (RENDERERFREETEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_FREE_TEXTURE_NAME);
        if (RendererModule.FreeTexture == NULL) { invalid = TRUE; }

        RendererModule.Clear = (RENDERERCLEAR)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_CLEAR_NAME);
        if (RendererModule.Clear == NULL) { invalid = TRUE; }

        *RendererModule._SetColorTable = (RENDERERSETCOLORTABLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_COLOR_TABLE_NAME);
        if (*RendererModule._SetColorTable == NULL) { invalid = TRUE; }

        RendererModule.SelectGraphicsCard = (RENDERERSELECTGRAPHICSCARD)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_GRAPHICS_CARD_NAME);
        if (RendererModule.SelectGraphicsCard == NULL) { invalid = TRUE; }

        RendererModule.AcquireGraphicsCardList = (RENDERERACQUIREGRAPHICSCARDLIST)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_GRAPHICS_CARD_LIST_NAME);
        if (RendererModule.AcquireGraphicsCardList == NULL) { invalid = TRUE; }

        RendererModule.AcquireModuleName = (RENDERERACQUIREMODULENAME)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_MODULE_NAME_NAME);

        RendererModule.AcquireTextureInformation = (RENDERERACQUIRETEXTUREINFORMATION)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ACQUIRE_TEXTURE_INFORMATION_NAME);
        RendererModule.AllocateOptimizedMesh = (RENDERERALLOCATEOPTIMIZEDMESH)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ALLOCATE_OPTIMIZED_MESH_NAME);
        *RendererModule._AllocateVertexBuffer = (RENDERERALLOCATEVERTEXBUFFER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ALLOCATE_VERTEX_BUFFER_NAME);
        RendererModule.BeginCubeMapRender = (RENDERERBEGINCUBEMAPRENDER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_BEGIN_CUBE_MAP_RENDER_NAME);
        RendererModule.BeginRenderToTexture = (RENDERERBEGINRENDERTOTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_BEGIN_RENDER_TO_TEXTURE_NAME);
        RendererModule.ClearZBuffer = (RENDERERCLEARZBUFFER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_CLEAR_Z_BUFFER_NAME);
        RendererModule.CopyBackBufferToRenderTexture = (RENDERERCOPYBACKBUFFERTORENDERTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_COPY_BACK_BUFFER_TO_RENDER_TEXTURE_NAME);
        *RendererModule._DotListD3DL = (RENDERERDOTLISTD3DL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DOT_LIST_D3DL_NAME);
        *RendererModule._DotListD3DTL = (RENDERERDOTLISTD3DTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DOT_LIST_D3DTL_NAME);
        *RendererModule._DrawPixels = (RENDERERDRAWPIXELS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_DRAW_PIXELS_NAME);
        *RendererModule._EnableClipping = (RENDERERENABLECLIPPING)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ENABLE_CLIPPING_NAME);
        RendererModule.EnableCulling = (RENDERERENABLECULLING)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ENABLE_CULLING_NAME);
        RendererModule.EnableLighting = (RENDERERENABLELIGHTING)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_ENABLE_LIGHTING_NAME);
        RendererModule.EndCubeMapRender = (RENDERERENDCUBEMAPRENDER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_END_CUBE_MAP_RENDER_NAME);
        RendererModule.EndRenderToTexture = (RENDERERENDRENDERTOTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_END_RENDER_TO_TEXTURE_NAME);
        *RendererModule._FreeOptimizedMesh = (RENDERERFREEOPTIMIZEDMESH)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_FREE_OPTIMIZED_MESH_NAME);
        *RendererModule._FreeVertexBuffer = (RENDERERFREEVERTEXBUFFER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_FREE_VERTEX_BUFFER_NAME);
        RendererModule.HasShaderSupport = (RENDERERHASSHADERSUPPORT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_HAS_SHADER_SUPPORT_NAME);
        *RendererModule._LineListD3DL = (RENDERERLINELISTD3DL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LINE_LIST_D3DL_NAME);
        *RendererModule._LineListD3DTL = (RENDERERLINELISTD3DTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_LINE_LIST_D3DTL_NAME);
        RendererModule.PolyList = (RENDERERPOLYLIST)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_NAME);
        RendererModule.PolyListBasis = (RENDERERPOLYLISTBASIS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_BASIS_NAME);
        RendererModule.PolyListBone = (RENDERERPOLYLISTBONE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_BONE_NAME);
        *RendererModule._PolyListD3DL = (RENDERERPOLYLISTD3DL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_D3DL_NAME);
        *RendererModule._PolyListD3DTL = (RENDERERPOLYLISTD3DTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_D3DTL_NAME);
        *RendererModule._PolyListTL = (RENDERERPOLYLISTTL)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_POLY_LIST_TL_NAME);
        *RendererModule._RenderOptimizedMesh = (RENDERERRENDEROPTIMIZEDMESH)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RENDER_OPTIMIZED_MESH_NAME);
        *RendererModule._RenderVertexBuffer = (RENDERERRENDERVERTEXBUFFER)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RENDER_VERTEX_BUFFER_NAME);
        *RendererModule._RenderVertexBufferBasis = (RENDERERRENDERVERTEXBUFFERBASIS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RENDER_VERTEX_BUFFER_BASIS_NAME);
        *RendererModule._RenderVertexBufferBone = (RENDERERRENDERVERTEXBUFFERBONE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RENDER_VERTEX_BUFFER_BONE_NAME);
        *RendererModule._RenderVertexBufferPrelit = (RENDERERRENDERVERTEXBUFFERPRELIT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RENDER_VERTEX_BUFFER_PRELIT_NAME);
        RendererModule.ResetTextureCache = (RENDERERRESETTEXTURECACHE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_RESET_TEXTURE_CACHE_NAME);
        RendererModule.SelectCubeTexture = (RENDERERSELECTCUBETEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_CUBE_TEXTURE_NAME);
        *RendererModule._SelectRenderTexture = (RENDERERSELECTRENDERTEXTURE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SELECT_RENDER_TEXTURE_NAME);
        RendererModule.SetAlpha = (RENDERERSETALPHA)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_ALPHA_NAME);
        RendererModule.SetAmbientLight = (RENDERERSETAMBIENTLIGHT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_AMBIENT_LIGHT_NAME);
        RendererModule.SetAmbientLightColor = (RENDERERSETAMBIENTLIGHTCOLOR)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_AMBIENT_LIGHT_COLOR_NAME);
        RendererModule.SetDestinationBlend = (RENDERERSETDESTINATIONBLEND)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_DESTINATION_BLEND_NAME);
        RendererModule.SetFog = (RENDERERSETFOG)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_FOG_NAME);
        RendererModule.SetGloss = (RENDERERSETGLOSS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_GLOSS_NAME);
        RendererModule.SetLightColor = (RENDERERSETLIGHTCOLOR)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_LIGHT_COLOR_NAME);
        RendererModule.SetLightConstants = (RENDERERSETLIGHTCONSTANTS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_LIGHT_CONSTANTS_NAME);
        RendererModule.SetLightVector = (RENDERERSETLIGHTVECTOR)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_LIGHT_VECTOR_NAME);
        RendererModule.SetMultiTextureBlend = (RENDERERSETMULTITEXTUREBLEND)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_MULTI_TEXTURE_BLEND_NAME);
        RendererModule.SetOmniLight = (RENDERERSETOMNILIGHT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_OMNI_LIGHT_NAME);
        RendererModule.SetSourceBlend = (RENDERERSETSOURCEBLEND)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_SOURCE_BLEND_NAME);
        RendererModule.SetStencilEnable = (RENDERERSETSTENCILENABLE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_STENCIL_ENABLE_NAME);
        RendererModule.SetStencilFunc = (RENDERERSETSTENCILFUNC)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_STENCIL_FUNC_NAME);
        RendererModule.SetStencilPass = (RENDERERSETSTENCILPASS)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_STENCIL_PASS_NAME);
        RendererModule.SetTextureClamp = (RENDERERSETTEXTURECLAMP)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TEXTURE_CLAMP_NAME);
        RendererModule.SetTransform = (RENDERERSETTRANSFORM)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TRANSFORM_NAME);
        RendererModule.SetTransformO2W = (RENDERERSETTRANSFORMO2W)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TRANSFORM_O2W_NAME);
        RendererModule.SetTransformW2L = (RENDERERSETTRANSFORMW2L)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_TRANSFORM_W2L_NAME);
        RendererModule.SetViewPort = (RENDERERSETVIEWPORT)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_SET_VIEWPORT_NAME);
        RendererModule.VideoRestore = (RENDERERVIDEORESTORE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_VIDEO_RESTORE_NAME);
        RendererModule.VideoSave = (RENDERERVIDEOSAVE)AcquireLambda(*RendererModule.State._Handle, RENDERER_MODULE_VIDEO_SAVE_NAME);

        if (!invalid)
        {
            *RendererModule.State._IsActive = TRUE;

            const RendererModuleInitializeArguments args =
            {
                .R =
                {
                    .Display = RendererMonitor.Color.Shift._R,
                    .Font = RendererFont.Color.Shift._R,
                    .BitsPerPixel = RendererState.Colors16Bit.Unknown._Unknown01
                },
                .G =
                {
                    .Display = RendererMonitor.Color.Shift._G,
                    .Font = RendererFont.Color.Shift._G,
                    .BitsPerPixel = RendererState.Colors16Bit.Unknown._Unknown03
                },
                .B =
                {
                    .Display = RendererMonitor.Color.Shift._B,
                    .Font = RendererFont.Color.Shift._B,
                    .BitsPerPixel = RendererState.Colors16Bit.Unknown._Unknown05
                },
                .Mode = RendererState.Options._AcceleratedGraphicsPort,
                .Unknown2 = (void*)0x00627ff0 // TODO
            };

            if (RendererModule.Init(*RendererModule.State._HWND, &args))
            {
                if (RendererModule.AcquireModuleName != NULL)
                {
                    if (!RendererModule.AcquireModuleName(RendererState.Options.ModuleName))
                    {
                        strcpy(RendererState.Options.ModuleName, RendererState.Options._ModuleFileName);
                    }
                }
                else
                {
                    strcpy(RendererState.Options.ModuleName, RendererState.Options._ModuleFileName);
                }

                SelectRendererGraphicsCard(*RendererModule.State._SelectedDeviceIndex);

                return TRUE;
            }

            *RendererState.Options._IsUsingModule = FALSE;

            return FALSE;
        }

        ReleaseRendererModule();

        *RendererState.Options._IsUsingModule = FALSE;

        return FALSE;
    }

    BOOL InitializeRendererModule1(const u32 width, const u32 height)
    {
        InitializeRendererModule2(*AppWindowState._HWND);

        return TRUE;
    }
}