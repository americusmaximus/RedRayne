#pragma once

#include "Renderers.Modules.Import.hxx"

#define MAX_RENDERER_MODULE_FILE_NAME_LENGTH 256
#define MAX_RENDERER_MODULE_NAME_LENGTH 256

namespace Renderer::Module
{
    struct RendererModuleContainer
    {
        struct
        {
            HWND* _HWND = (HWND*)0x008fd618; // TODO
            BOOL* _IsActive = (BOOL*)0x008fd604; // TODO
            HMODULE* _Handle = (HMODULE*)0x008fd608;// TODO

            u32* _HasShaderSupport = (u32*)0x0084af04; // TODO Type
            u32* _SelectedDeviceIndex = (u32*)0x008fd4c4; // TODO
        } State;

        Renderers::Modules::RENDERERACQUIREGAMEWINDOWMODE AcquireGameWindowMode;// = (RENDERERISGAMEWINDOWMODE*)0x008fd4d4; // TODO
        Renderers::Modules::RENDERERACQUIREGRAPHICSCARDLIST AcquireGraphicsCardList;// = (Renderers::Modules::RENDERERACQUIREGRAPHICSCARDLIST*)0x008fd524; // TODO
        Renderers::Modules::RENDERERACQUIREMODULENAME AcquireModuleName;
        Renderers::Modules::RENDERERACQUIRETEXTUREINFORMATION AcquireTextureInformation;// = (RENDERERGETTEXTUREINFO*)0x008fd528; // TODO
        Renderers::Modules::RENDERERALLOCATEOPTIMIZEDMESH AllocateOptimizedMesh;// = (RENDERERALLOCATEOPTIMIZEDMESH*)0x008fd5c0; // TODO
        Renderers::Modules::RENDERERALLOCATETEXTURE AllocateTexture;// = (RENDERERALLOCATETEXTURE*)0x008fd508; // TODO
        Renderers::Modules::RENDERERALLOCATEVERTEXBUFFER* _AllocateVertexBuffer = (Renderers::Modules::RENDERERALLOCATEVERTEXBUFFER*)0x008fd5cc; // TODO
        Renderers::Modules::RENDERERBEGINCUBEMAPRENDER BeginCubeMapRender;// = (RENDERERBEGINCUBEMAPRENDER*)0x008fd5a4; // TODO
        Renderers::Modules::RENDERERBEGINRENDERTOTEXTURE BeginRenderToTexture;// = (RENDERERBEGINRENDERTOTEXTURE*)0x008fd5b0; // TODO
        Renderers::Modules::RENDERERBEGINSCENE BeginScene;// = (RENDERERBEGINSCENE*)0x008fd4f8; // TODO
        Renderers::Modules::RENDERERCLEAR Clear;// = (RENDERERCLEAR*)0x008fd518; // TODO
        Renderers::Modules::RENDERERCLEARZBUFFER ClearZBuffer;
        Renderers::Modules::RENDERERCOPYBACKBUFFERTORENDERTEXTURE CopyBackBufferToRenderTexture;// = (RENDERERCOPYBACKBUFFERTORENDERTEXTURE*)0x008fd5bc; // TODO
        Renderers::Modules::RENDERERCREATEGAMEWINDOW CreateGameWindow;// = (RENDERERCREATEGAMEWINDOW*)0x008fd4e4; // TODO
        Renderers::Modules::RENDERERDESTROYGAMEWINDOW DestroyGameWindow;// = (RENDERERDESTROYGAMEWINDOW*)0x008fd4e8; // TODO
        Renderers::Modules::RENDERERDOTLISTD3DL* _DotListD3DL = (Renderers::Modules::RENDERERDOTLISTD3DL*)0x008fd568; // TODO
        Renderers::Modules::RENDERERDOTLISTD3DTL* _DotListD3DTL = (Renderers::Modules::RENDERERDOTLISTD3DTL*)0x008fd56c; // TODO
        Renderers::Modules::RENDERERDRAWPIXELS* _DrawPixels = (Renderers::Modules::RENDERERDRAWPIXELS*)0x008fd594; // TODO
        Renderers::Modules::RENDERERENABLECLIPPING* _EnableClipping = (Renderers::Modules::RENDERERENABLECLIPPING*)0x008fd580; // TODO
        Renderers::Modules::RENDERERENABLECULLING EnableCulling;// = (RENDERERENABLECULLING*)0x008fd57c; // TODO
        Renderers::Modules::RENDERERENABLELIGHTING EnableLighting;// = (RENDERERENABLELIGHTING*)0x008fd58c; // TODO
        Renderers::Modules::RENDERERENDCUBEMAPRENDER EndCubeMapRender;// = (RENDERERENDCUBEMAPRENDER*)0x008fd5a8; // TODO
        Renderers::Modules::RENDERERENDRENDERTOTEXTURE EndRenderToTexture;// = (RENDERERENDRENDERTOTEXTURE*)0x008fd5b4; // TODO
        Renderers::Modules::RENDERERENDSCENE EndScene;// = (RENDERERENDSCENE*)0x008fd4fc; // TODO
        Renderers::Modules::RENDERERFREEOPTIMIZEDMESH* _FreeOptimizedMesh = (Renderers::Modules::RENDERERFREEOPTIMIZEDMESH*)0x008fd5c4; // TODO
        Renderers::Modules::RENDERERFREETEXTURE FreeTexture;// = (RENDERERFREETEXTURE*)0x008fd514; // TODO
        Renderers::Modules::RENDERERFREEVERTEXBUFFER* _FreeVertexBuffer = (Renderers::Modules::RENDERERFREEVERTEXBUFFER*)0x008fd5d0; // TODO
        Renderers::Modules::RENDERERHASSHADERSUPPORT HasShaderSupport;// = (RENDERERHASSHADERSUPPORT*)0x008fd600; // TODO
        Renderers::Modules::RENDERERINIT Init;// = (RENDERERINIT*)0x008fd4c8; // TODO
        Renderers::Modules::RENDERERKILL Kill;// = (RENDERERKILL*)0x008fd4cc; // TODO
        Renderers::Modules::RENDERERLINELISTD3DL* _LineListD3DL = (Renderers::Modules::RENDERERLINELISTD3DL*)0x008fd570; // TODO
        Renderers::Modules::RENDERERLINELISTD3DTL* _LineListD3DTL = (Renderers::Modules::RENDERERLINELISTD3DTL*)0x008fd574; // TODO
        Renderers::Modules::RENDERERLOCKFRAME LockFrame;// = (RENDERERLOCKFRAME*)0x008fd500; // TODO
        Renderers::Modules::RENDERERLOCKFRAMEFORSCREENSHOT LockFrameForScreenshot;// = (RENDERERLOCKFRAMEFORSCREENSHOT*)0x008fd4dc; // TODO
        Renderers::Modules::RENDERERPOLYLIST PolyList;// = (Renderers::Modules::RENDERERPOLYLIST*)0x008fd550; // TODO
        Renderers::Modules::RENDERERPOLYLISTBASIS PolyListBasis;// = (RENDERERPOLYLISTBASIS*)0x008fd554; // TODO
        Renderers::Modules::RENDERERPOLYLISTBONE PolyListBone;// = (Renderers::Modules::RENDERERPOLYLISTBONE*)0x008fd558; // TODO
        Renderers::Modules::RENDERERPOLYLISTD3DL* _PolyListD3DL = (Renderers::Modules::RENDERERPOLYLISTD3DL*)0x008fd55c; // TODO
        Renderers::Modules::RENDERERPOLYLISTD3DTL* _PolyListD3DTL = (Renderers::Modules::RENDERERPOLYLISTD3DTL*)0x008fd560; // TODO
        Renderers::Modules::RENDERERPOLYLISTTL* _PolyListTL = (Renderers::Modules::RENDERERPOLYLISTTL*)0x008fd564; // TODO
        Renderers::Modules::RENDERERRENDEROPTIMIZEDMESH* _RenderOptimizedMesh = (Renderers::Modules::RENDERERRENDEROPTIMIZEDMESH*)0x008fd5c8; // TODO
        Renderers::Modules::RENDERERRENDERVERTEXBUFFER* _RenderVertexBuffer = (Renderers::Modules::RENDERERRENDERVERTEXBUFFER*)0x008fd5d4; // TODO
        Renderers::Modules::RENDERERRENDERVERTEXBUFFERBASIS* _RenderVertexBufferBasis = (Renderers::Modules::RENDERERRENDERVERTEXBUFFERBASIS*)0x008fd5dc; // TODO
        Renderers::Modules::RENDERERRENDERVERTEXBUFFERBONE* _RenderVertexBufferBone = (Renderers::Modules::RENDERERRENDERVERTEXBUFFERBONE*)0x008fd5e0; // TODO
        Renderers::Modules::RENDERERRENDERVERTEXBUFFERPRELIT* _RenderVertexBufferPrelit = (Renderers::Modules::RENDERERRENDERVERTEXBUFFERPRELIT*)0x008fd5d8; // TODO
        Renderers::Modules::RENDERERRESETTEXTURECACHE ResetTextureCache;// = (RENDERERRESETTEXTURECACHE*)0x008fd578; // TODO
        Renderers::Modules::RENDERERRESTOREVIDEOMODE RestoreVideoMode;// = (RENDERERRESTOREVIDEOMODE*)0x008fd4f4; // TODO
        Renderers::Modules::RENDERERSELECTCUBETEXTURE SelectCubeTexture;// = (RENDERERSELECTCUBETEXTURE*)0x008fd5ac; // TODO
        Renderers::Modules::RENDERERSELECTGAMEWINDOW SelectGameWindow;// = (RENDERERSELECTGAMEWINDOW*)0x008fd4ec; // TODO
        Renderers::Modules::RENDERERSELECTGAMEWINDOWMODE SelectGameWindowMode;// = (RENDERERSETGAMEWINDOWMODE*)0x008fd4d8; // TODO
        Renderers::Modules::RENDERERSELECTGRAPHICSCARD SelectGraphicsCard;// = (RENDERERSELECTGRAPHICSCARD*)0x008fd520; // TODO
        Renderers::Modules::RENDERERSELECTRENDERTEXTURE* _SelectRenderTexture = (Renderers::Modules::RENDERERSELECTRENDERTEXTURE*)0x008fd5b8; // TODO
        Renderers::Modules::RENDERERSELECTTEXTURE SelectTexture;// = (RENDERERSELECTTEXTURE*)0x008fd510; // TODO
        Renderers::Modules::RENDERERSELECTVIDEOMODE SelectVideoMode;// = (Renderers::Modules::RENDERERSELECTVIDEOMODE*)0x008fd4f0; // TODO
        Renderers::Modules::RENDERERSETALPHA SetAlpha;// = (RENDERERSETALPHA*)0x008fd5e4; // TODO
        Renderers::Modules::RENDERERSETAMBIENTLIGHT SetAmbientLight;// = (RENDERERSETAMBIENTLIGHT*)0x008fd538; // TODO
        Renderers::Modules::RENDERERSETAMBIENTLIGHTCOLOR SetAmbientLightColor;// = (RENDERERSETAMBIENTLIGHTCOLOR*)0x008fd53c; // TODO
        Renderers::Modules::RENDERERSETCOLORTABLE* _SetColorTable = (Renderers::Modules::RENDERERSETCOLORTABLE*)0x008fd51c; // TODO
        Renderers::Modules::RENDERERSETDESTINATIONBLEND SetDestinationBlend;// = (RENDERERSETDESTINATIONBLEND*)0x008fd5f8; // TODO
        Renderers::Modules::RENDERERSETDIRTYRECT SetDirtyRect;// = (RENDERERSETDIRTYRECT*)0x008fd4e0; // TODO
        Renderers::Modules::RENDERERSETFOG SetFog;// = (RENDERERSETFOG*)0x008fd588; // TODO
        Renderers::Modules::RENDERERSETGLOSS SetGloss;// = (RENDERERSETGLOSS*)0x008fd5fc; // TODO
        Renderers::Modules::RENDERERSETLIGHTCOLOR SetLightColor;// = (RENDERERSETLIGHTCOLOR*)0x008fd540; // TODO
        Renderers::Modules::RENDERERSETLIGHTCONSTANTS SetLightConstants;// = (RENDERERSETLIGHTCONSTANTS*)0x008fd530; // TODO
        Renderers::Modules::RENDERERSETLIGHTVECTOR SetLightVector;// = (RENDERERSETLIGHTVECTOR*)0x008fd534; // TODO
        Renderers::Modules::RENDERERSETMULTITEXTUREBLEND SetMultiTextureBlend;// = (RENDERERSETMULTITEXTUREBLEND*)0x008fd5a0; // TODO
        Renderers::Modules::RENDERERSETOMNILIGHT SetOmniLight;// = (RENDERERSETOMNILIGHT*)0x008fd590; // TODO
        Renderers::Modules::RENDERERSETSOURCEBLEND SetSourceBlend;// = (RENDERERSETSOURCEBLEND*)0x008fd5f4; // TODO
        Renderers::Modules::RENDERERSETSTENCILENABLE SetStencilEnable;// = (RENDERERSETSTENCILENABLE*)0x008fd5e8; // TODO
        Renderers::Modules::RENDERERSETSTENCILFUNC SetStencilFunc;// = (RENDERERSETSTENCILFUNC*)0x008fd5f0; // TODO
        Renderers::Modules::RENDERERSETSTENCILPASS SetStencilPass;// = (RENDERERSETSTENCILPASS*)0x008fd5ec; // TODO
        Renderers::Modules::RENDERERSETTEXTURECLAMP SetTextureClamp;// = (RENDERERSETTEXTURECLAMP*)0x008fd584; // TODO
        Renderers::Modules::RENDERERSETTRANSFORM SetTransform;// = (RENDERERSETTRANSFORM*)0x008fd544; // TODO
        Renderers::Modules::RENDERERSETTRANSFORMO2W SetTransformO2W;// = (RENDERERSETTRANSFORMO2W*)0x008fd54c; // TODO
        Renderers::Modules::RENDERERSETTRANSFORMW2L SetTransformW2L;// = (RENDERERSETTRANSFORMW2L*)0x008fd548; // TODO
        Renderers::Modules::RENDERERSETVIEWPORT SetViewPort;// = (RENDERERSETVIEWPORT*)0x008fd52c; // TODO
        Renderers::Modules::RENDERERTOGGLE Toggle;// = (RENDERERTOGGLE*)0x008fd4d0; // TODO
        Renderers::Modules::RENDERERUNLOCKFRAME UnlockFrame;// = (RENDERERUNLOCKFRAME*)0x008fd504; // TODO
        Renderers::Modules::RENDERERUPLOADTEXTURE* _UploadTexture = (Renderers::Modules::RENDERERUPLOADTEXTURE*)0x008fd50c; // TODO
        Renderers::Modules::RENDERERVIDEORESTORE VideoRestore;// = (RENDERERVIDEORESTORE*)0x008fd59c; // TODO
        Renderers::Modules::RENDERERVIDEOSAVE VideoSave;// = (RENDERERVIDEOSAVE*)0x008fd598; // TODO
    };

    extern RendererModuleContainer RendererModule;

    BOOL InitializeRendererModule2(const HWND hwnd);
    BOOL InitializeRendererModule1(const u32, const u32);

    void ReleaseRendererModule(void);
}