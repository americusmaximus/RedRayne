#pragma once

#include "Renderers.Basic.hxx"
#include "Renderers.Modules.Basic.hxx"
#include "Native.Basic.hxx"
#include "Assets.Textures.hxx"
#include "Renderer.Module.hxx"

#define QUAD_VERTEX_COUNT 4
#define QUAD_TRIANGLE_COUNT 2

#define FREE_TEXTURE_INDEX (-1)

#define MAX_RENDERER_MODULE_COUNT 16
#define RENDERER_MODULE_FILE_SEARCH_PATTERN "%s\\*.*"
#define RENDERER_MODULE_FILE_EXTENSION ".DLL"

#define RENDERER_BLOOD_VERTEX_COUNT 4096

namespace Renderer
{
    // TODO: TRUE/FALSE?
    enum class ToggleMode : u32
    {
        Unknown0 = 0,// TODO
        Unknown1 = 1// TODO
    };

    // TODO Looks like TLVertex
    struct ProperTLVertex
    {
        f32 Unk1;
        f32 Unk2;
        f32 Unk3;
        f32 Unk4;
        u32 Unk5;
        f32 Unk6;
        f32 Unk7;
        f32x2 UV;
        Graphics::f32xRGBA Color;
        f32 Unk14;
        f32 Unk15;
        f32 Unk16;
    };

    struct Quad
    {
        s32 Count; //TODO
        u32 Color;
        f32 X[4];
        f32 Y[4];
        f32 Z[4];
        f32 UVX[4];
        f32 UVY[4];
        s32 Unk2; //TODO
        s32 Unk3; //TODO
    };

    struct UnknownRendererStruct
    {
        s32 IndexCount;
        s32 VertexCount;
        // TODO
    };

    struct RendererContainer
    {
        struct
        {
            BOOL* _IsUsingModule = (BOOL*)0x008fd4b4; // TODO

            char ModuleName[MAX_RENDERER_MODULE_NAME_LENGTH];

            // TODO char ModuleName[MAX_RENDERER_MODULE_FILE_NAME_LENGTH] = GRAPHICS_RENDERER_NAME_DIRECTX_8_1;
            char* _ModuleFileName = (char*)0x00627ef0;

            u32 SelectedDeviceIndex;// = (u32*)0x00727360; // TODO

            Renderers::RendererAcceleratedGraphicsPortMode* _AcceleratedGraphicsPort = (Renderers::RendererAcceleratedGraphicsPortMode*)0x008fd4b8;//TODO
        } Options;

        struct
        {
            u32 Count;
            char Names[MAX_RENDERER_MODULE_COUNT][MAX_IO_FILE_NAME_LENGTH];
        } Modules;

        BOOL* _IsVideoMode = (BOOL*)0x009018f0; // TODO, TODO name
        BOOL* _IsActive = (BOOL*)0x0068caa0; // TODO
        BOOL* _IsRenderingToTexture = (BOOL*)0x00738ce8; // TODO
        u32* _BitsPerPixel = (u32*)0x008fd61c; // TODO


        u32* _Unknown1 = (u32*)0x0084af00;//TODO type looks like enum
        u32* _Unknown2 = (u32*)0x00730378;//TODO

        struct
        {
            u32 Count;// = (u32*)0x00727364; // TODO
            char* APIs[MAX_DEVICE_COUNT];// = (char***)0x00727368; // TODO
            char* Names[MAX_DEVICE_COUNT];// = (char***)0x007273a8; // TODO
            u32 DeviceIDs[MAX_DEVICE_COUNT];// = (u32**)0x007273e8; // TODO
            u32 VendorIDs[MAX_DEVICE_COUNT];// = (u32**)0x00727428; // TODO
        } Devices;

        struct
        {
            u32* _BitsPerPixel = (u32*)0x008fd624; // TODO
            BOOL* _Mode = (BOOL*)0x008fd628; // TODO name IsActive
        } Lock;

        struct
        {
            // u8 RGB[768]
            u8* _RGB = (u8*)0x0068a968;// TODO

            f32* _Unk = (f32*)0x0072f6dc; // TODO
        } Colors;

        struct
        {
            // u16 RGBA[256]
            u16* _RGBA = (u16*)0x00689d40;// TODO

            struct
            {
                u32* _Unknown01 = (u32*)0x0068a94c; // TODO
                u32* _Unknown02 = (u32*)0x00626640; // TODO, def value
                u32* _Unknown03 = (u32*)0x0068a958; // TODO
                u32* _Unknown04 = (u32*)0x00626648; // TODO, def value
                u32* _Unknown05 = (u32*)0x0068a964; // TODO
                u32* _Unknown06 = (u32*)0x00626650; // TODO, def value
                u32* _Unknown07 = (u32*)0x00626658; // TODO, def value
                u32* _Unknown08 = (u32*)0x00626678; // TODO, def value
                u32* _Unknown09 = (u32*)0x006266e0; // TODO, def value
                u32* _Unknown10 = (u32*)0x006266a0; // TODO, def value
                u32* _Unknown11 = (u32*)0x00626698; // TODO, def value
                u32* _Unknown12 = (u32*)0x006266c0; // TODO, def value
            } Unknown;
        } Colors16Bit;

        struct
        {
            // uint[256]
            u32* _RGBA = (u32*)0x00689f40; // TODO, name
        } Colors32Bit;

        // u32 Unk[1440];
        u32** UINT_ARRAY_0067f040 = (u32**)0x0067f040;// TODO

        struct
        {
            Renderers::RendererCullMode* _CullMode = (Renderers::RendererCullMode*)0x007303e0; // TODO
            u32 Alpha;// = (u32*)0x007302f8; // TODO
            Renderers::RendererDestinationBlend* _DestinationBlend = (Renderers::RendererDestinationBlend*)0x007302e8; // TODO
            Renderers::RendererClippingMode* _Clipping = (Renderers::RendererClippingMode*)0x00733e10; // TODO
            Renderers::RendererBlendOperation* _MultiTextureBlend = (Renderers::RendererBlendOperation*)0x00738ce0; // TODO


            Renderers::RendererDestinationBlend* _DestinationBlendCopy1 = (Renderers::RendererDestinationBlend*)0x0068cd34; // TODO



            Renderers::RendererMode* _UnknownMode1 = (Renderers::RendererMode*)0x0068cd3c;// TODO
            Renderers::RendererMode* _UnknownMode2 = (Renderers::RendererMode*)0x007302e0;// TODO
            Renderers::RendererMode* _UnknownMode3 = (Renderers::RendererMode*)0x007302e4;// TODO
            u32* _UnknownMode4 = (u32*)0x0068cd40;// TODO
            u32* _UnknownMode5 = (u32*)0x007302f4;// TODO Type
            u32* _UnknownMode6 = (u32*)0x007302f0;// TODO Type
        } Mode;

        struct
        {
            BOOL* _IsComplete = (BOOL*)0x00733e14;

            f32* _zNear = (f32*)0x0073036c;//TODO
            f32* _zFar = (f32*)0x00730370;//TODO

            f32* _Scale = (f32*)0x00730374;//TODO

            // NOTE: These "BOX" caclulations are being done for the "Settings" screen at the beginning of the game.
            struct
            {
                f32* _X0 = (f32*)0x007302fc;// TODO
                f32* _Y0 = (f32*)0x00730300;// TODO

                f32* _X1 = (f32*)0x00730304;// TODO
                f32* _Y1 = (f32*)0x00730308;// TODO

                f32* _CenterX = (f32*)0x00730314;//TODO
                f32* _HalfWidth = (f32*)0x0073030c;//TODO

                f32* _CenterY = (f32*)0x00730318;//TODO
                f32* _HalfHeight = (f32*)0x00730310;//TODO

                u32* _RoundX = (u32*)0x0068ac68;//TODO
                u32* _RoundY = (u32*)0x0068ac6c;//TODO
                u32* _MaxWidth = (u32*)0x0068ac70;//TODO
                u32* _MaxHeight = (u32*)0x0068ac74;//TODO
            } Box;

            f32* _UnknownValue1 = (f32*)0x00733e18;//TODO
            f32* _UnknownValue2 = (f32*)0x00612bc8;//TODO
        } ViewPort;

        struct
        {
            Renderers::RendererLightingMode* _Mode = (Renderers::RendererLightingMode*)0x007303dc; // TODO

            struct
            {
                f32* _R = (f32*)0x007303b8; // TODO
                f32* _G = (f32*)0x007303b4; // TODO
                f32* _B = (f32*)0x007303b0; // TODO
                f32* _A = (f32*)0x007302ec; // TODO
            } Color;

            struct
            {
                // TODO arrays of MAX_LIGHT_COUNT
                f32* _R = (f32*)0x00730380; // TODO
                f32* _G = (f32*)0x0073038c; // TODO
                f32* _B = (f32*)0x00730398; // TODO
            } Colors;

            struct
            {
                f32* _Diffuse = (f32*)0x007303a4; // TODO
                f32* _Specular = (f32*)0x007303a8; // TODO
                f32* _Power = (f32*)0x007303ac; // TODO

                u32* _UnknownAlpha = (u32*)0x0061e9cc; // TODO, def value

                struct
                {
                    f32* _R = (f32*)0x00733e1c; // TODO
                    f32* _G = (f32*)0x00733e20; // TODO
                    f32* _B = (f32*)0x00733e24; // TODO
                } Color;
            } Constant;

            f32x3* _XYZ = (f32x3*)0x008fecf0; // TODO
        } Light;

        struct
        {
            BOOL* _Mode = (BOOL*)0x00621248; // TODO . TODO. Name & use? IsActive?

            struct
            {
                f32* _R = (f32*)0x007303c8; // TODO
                f32* _G = (f32*)0x007303cc; // TODO
                f32* _B = (f32*)0x007303d0; // TODO

                u32* _Color = (u32*)0x0068cd44; // TODO
            } Color;

            f32* _Start = (f32*)0x007303d4; // TODO
            f32* _End = (f32*)0x007303d8; // TODO

            f32* _Unknown = (f32*)0x00738c9c; // TODO
        } Fog;

        struct
        {
            u32* _Clear = (u32*)0x00733e28; // TODO
        } Color;

        struct
        {
            f32* _zNear = (f32*)0x0073035c; // TODO
            f32* _zFar = (f32*)0x00730360; // TODO

            Mathematics::Matrix4x4* _O2W = (Mathematics::Matrix4x4*)0x00738c5c; // TODO V4[4] ???
            Mathematics::Matrix4x4* _W2L = (Mathematics::Matrix4x4*)0x00738ca0; // TODO

            Renderers::Transform** _TransformPointer = (Renderers::Transform**)0x007303e4; // TODO

            Mathematics::Matrix4x4* _UnknownMatrix1 = (Mathematics::Matrix4x4*)0x0073031c; // TODO

            s32* _Unknown101 = (s32*)0x007303e8;//TODO
        } Transform;

        struct
        {
            u32* _FrameCounter = (u32*)0x0068cdd8; // TODO

        } Counters;

        struct
        {
            void** _AllocatedMemory2 = (void**)0x00738c58;//TODO
            void** _AllocatedMemory3 = (void**)0x0070c858;//TODO
            void** _AllocatedMemory4 = (void**)0x0070c85c;//TODO
            void** _AllocatedMemory5 = (void**)0x0070c860;//TODO

            void** _AllocatedMemory6 = (void**)0x0070f86c;//TODO
            s32* _AllocatedMemory6VertexCount = (s32*)0x0070f868;//TODO
            s32* _AllocatedMemory6IndexCount = (s32*)0x0070f870;//TODO
            u16* _AllocatedMemory6Indexes = (u16*)0x0070f874;//TODOD array of 192

            void** _AllocatedMemory7 = (void**)0x008c20e4;//TODO

            struct
            {
                void** _AllocatedMemory1 = (void**)0x00730dec;//TODO
                void** _AlignedAllocatedMemory1 = (void**)0x00730df0;//TODO
                void** _AlignedAllocatedMemory1Copy = (void**)0x00730df4;//
            } Vertexes;
        } Allocated;

        struct
        {
            u16* _QuadIndexes = (u16*)0x006ee83c;// TODO
            s32* _QuadCount = (s32*)0x006f4848;//TODO
            Quad* _Quads = (Quad*)0x006f484c;//TODO: array of unknown size
        } Blood;

        s32** _UnknownPointer1 = (s32**)0x0061f7b4;//
        s32* _UnknownValue1 = (s32*)0x006c5bec;// TODO

        s32** _UnknownPointer2 = (s32**)0x0061f7b8;//
        s32* _UnknownValue2 = (s32*)0x006c5de8;// TODO

        f32* _UnknownFloat1 = (f32*)0x00730364;//TODO
        f32* _UnknownFloat2 = (f32*)0x00730368;//TODO
    };

    extern RendererContainer RendererState;

    // TODO name
    // TODO, should this be part of main window?
    struct RendererWindowContainer
    {
        Renderers::RendererWindowMode* _WindowMode = (Renderers::RendererWindowMode*)0x0068ac7c; // TODO

        /// <summary>
        /// 1440 32-bit value.
        /// 1440 is (guess) the maximum window height the engine was supporting back then.
        /// </summary>
        void** _Unk4 = (void**)0x0067f040;// TODO
    };

    extern RendererWindowContainer RendererWindow;

    struct RendererMonitorContainer
    {
        struct
        {
            struct
            {
                u32* _R = (u32*)0x0068a944;//TODO
                u32* _G = (u32*)0x0068a950;//TODO
                u32* _B = (u32*)0x0068a95c;//TODO
            } Shift;
        } Color;
    };

    extern RendererMonitorContainer RendererMonitor;

    struct RendererFontContainer
    {
        struct
        {
            u32* _R = (u32*)0x006c37cc; //TODO
            u32* _G = (u32*)0x006c37d0; //TODO
            u32* _B = (u32*)0x006c37d4; //TODO

            u16* _ShortColor = (u16*)0x006c37c6;//TODO
            u32* _Color = (u32*)0x006c37c8;//TODO

            struct
            {
                u32* _R = (u32*)0x0068a948;//TODO
                u32* _G = (u32*)0x0068a954;//TODO
                u32* _B = (u32*)0x0068a960;//TODO
            } Shift;
        } Color;
    };

    extern RendererFontContainer RendererFont;

    void InitializeRendererModules(void);

    void SelectRendererColors(void);

    void SelectRendererVideoMode1(void);
    u32 SelectRendererVideoMode2(const u32 width, const u32 height, const u32 bpp, const u32 indx);

    u32 SelectRendererGraphicsCard(const u32 indx);

    void ResetRendererTextureCache1(void);

    void ReleaseRendererTexture1(Assets::Texture* tex);
    void ReleaseRendererTexture2(Assets::Texture* tex);
    void ReleaseRendererTexture3(Assets::Texture* tex);

    void SelectRendererAmbientLight(const f32 color);
    void EndRendererScene(void);
    void SelectRendererLightConstants2(const f32 diffuse, const f32 specular, const f32 power);
    void SelectRendererTextureClamp(const Renderers::RendererClampMode u, const Renderers::RendererClampMode v, const u32 indx);
    void SelectRendererLighting(const Renderers::RendererLightingMode mode);
    void SelectRendererLightConstants4(const u32 color);
    void SelectRendererSourceBlend(const Renderers::RendererSourceBlendMode mode);
    void SelectRendererLightColor(const f32 r, const f32 g, const f32 b, const u32 indx);
    void SelectRendererDestinationBlend(const Renderers::RendererDestinationBlend mode);
    void SelectRendererLightConstants1(const f32 alpha);
    u32 SelectRendererMultiTextureBlend(const Renderers::RendererBlendOperation operation);
    Mathematics::Matrix4x4* AcquireRendererTransformW2L(void);
    void SelectRendererLightVector(const f32x3* xyz, const f32 length, const u32 indx);
    void SelectRendererFog(const u32 r, const u32 g, const u32 b, const f32 start, const f32 end);
    u32 SelectRendererRectangle(const u32 x, const u32 y, const u32 width, const u32 height);
    void SelectRendererClearColor(const u32 color);
    void ActivateRendererClipping(void);
    void DeActivateRendererClipping(void);
    f32 CalculateAverateRendererLightColor(void);
    void BeginRendererScene(void);
    void BeginRendererRenderToTexture(const u32 indx);
    void EndRendererRenderToTexture(void);
    BOOL SelectRendererVideModeResetTextureCache(void);
    void AllocateRendererTexture(Assets::Texture* tex);
    u32 ReleaseRenderer(void);
    void UploadRendererTexture(Assets::Texture* tex);
    void SaveRendererVideoMode(void);
    u32 RestoreRendererVideoMode1(void);
    BOOL RestoreRendererVideoMode2(BOOL value);
    void SelectRendererAlpha(const u32 value);
    u32 AcquireRendererAlpha(void);
    void SelecteRendererCulling(const Renderers::RendererCullMode mode);
    void SelectRendererRenderTexture(const u32 indx);
    void SelectRendererTexture(Assets::Texture* tex, const u32 indx);
    u32 AcquireRendererClearColor(void);
    void SelectRendererOmniLight(const Graphics::f32xRGB* rgb, const f32 alpha, const u32 color);
    void SelectRendererLightConstants3(const f32 alpha, const f32 r, const f32 g, const f32 b);
    BOOL LockRendererFrame(void);
    BOOL AcquireRendererGraphicsCards(u32* count, char** apis, char** names, u32* dids, u32* vids);
    void SelectRendererTransform(void);
    void ResetRendererTextureCache2(void); // TODO: move to cache things
    BOOL UnlockRendererFrame1(void);
    BOOL UnlockRendererFrame2(const BOOL mode);
    BOOL ToggleRendererScreen(const Renderer::ToggleMode mode);
    void ClearRendererScreen(const Renderers::RendererClearMode mode);
    u32 SelectRendererViewPort(const f32 x, const f32 y, const f32 width, const f32 height);
    void SelectRendererTransformW2L(const void*, const Mathematics::Matrix4x4* matrix, const f32 zNear, const f32 zfar); // TODO: Types
    // TODO O2W
    void SelectRendererMode(void);
    void ReUploadRendererTextures(void);
    void SelectRendererFogMode(void);
    void SelectRendererUnknownMode6FALSE(void);
    void SelectUnknownRendererMode6TRUE(void);

    // TODO Does this belong here? Should we have separate texture cache files?
    Assets::Texture* FindRendererTexture1(Assets::Textures::TextureDescriptor* desc); // TODO: move to cache things

    void SelectUnknownRendererMode2(const Renderers::RendererMode mode);
    void ReleaseRendererTexture4(Assets::Textures::TextureDescriptor* desc);
    void ReleaseRendererTextures1(void);
    u32 AcquireAvailableRendererTextureIndex(void);

    void RestoreRendererVideoMode(void);
    void AllocateRendererTextures1(const u32 count);

    void SelectRendererTransformO2W(BOOL mode);

    void InitializeTLVertex2(f32x3* xyz, ProperTLVertex* vertex);
    void InitializeTLVertex1(ProperTLVertex* vertex);

    void RenderBlood1(UnknownRendererStruct* param);

    void UpdateRendererTransform1(Renderers::Transform* transform);
    void UpdateRendererTransform2(void);
    void UpdateLights1(const f32x3* xyz);

    void PopulateRendererUnknown41(void);
    void PopulateRendererUnknown42(void);

    f32 AcquireRendererViewPortNear(void);
    void RenderRectangle(const s32 x, const s32 y, const s32 width, const s32 height);




    /// TODO
    typedef const void(CDECLAPI* FUN_00501EF0) (Renderers::RTLVX* vertexes, const u32 vertexCount, u16* indexes, const u32 indexCount);//TODO
    static FUN_00501EF0 FUN_00501ef0 = (FUN_00501EF0)0x00501ef0;//TODO 
}