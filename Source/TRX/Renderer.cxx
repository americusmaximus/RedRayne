#include "Logger.hxx"
#include "Mathematics.Basic.hxx"
#include "Memory.hxx"
#include "Monitor.hxx"
#include "Renderer.hxx"
#include "Renderer.Module.hxx"
#include "Renderer.Packet.hxx"
#include "Strings.hxx"
#include "App.Windows.hxx"
#include "Assets.Textures.hxx"

#include <math.h>

#define RENDERER_RECTANGLE_VERTEX_COUNT 4
#define RENDERER_RECTANGLE_TRIANGLE_COUNT 2
#define RENDERER_RECTANGLE_INDEX_COUNT 6

using namespace IO::Path;
using namespace Renderer::Packet;
using namespace App;
using namespace App::Windows;
using namespace Assets;
using namespace Assets::Textures;
using namespace Graphics;
using namespace Logger;
using namespace Mathematics;
using namespace Memory;
using namespace Monitor;
using namespace Renderers::Modules;
using namespace Renderer::Module;
using namespace Strings;
using namespace Renderers;


namespace Renderer
{
    RendererContainer RendererState;
    RendererFontContainer RendererFont;
    RendererMonitorContainer RendererMonitor;
    RendererWindowContainer RendererWindow;

    void InitializeRendererModules(void)
    {
        char dir[STANDARD_IO_DIRECTORY_NAME_LENGTH];

        if (!GetCurrentDirectoryA(STANDARD_IO_DIRECTORY_NAME_LENGTH, dir))
        {
            LogError("Unable to acquire current directory.");
        }

        char pattern[MAX_IO_FILE_NAME_LENGTH];

        sprintf(pattern, RENDERER_MODULE_FILE_SEARCH_PATTERN, dir);

        WIN32_FIND_DATAA data;

        ZeroMemory(&data, sizeof(WIN32_FIND_DATAA));

        const auto handle = FindFirstFileA(pattern, &data);

        if (handle == INVALID_HANDLE_VALUE) { return; }

        while (FindNextFileA(handle, &data) && RendererState.Modules.Count < MAX_RENDERER_MODULE_COUNT)
        {
            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { continue; }

            char ext[MAX_IO_FILE_NAME_LENGTH];

            SplitPath(data.cFileName, NULL, NULL, NULL, ext);

            if (EqualStrings(ext, RENDERER_MODULE_FILE_EXTENSION))
            {
                strcpy(RendererState.Modules.Names[RendererState.Modules.Count], data.cFileName);

                RendererState.Modules.Count = RendererState.Modules.Count + 1;
            }
        }

        FindClose(handle);
    }

    void SelectRendererVideoMode1(void)
    {
        *RendererState._IsVideoMode = TRUE;

        if (*RendererState.Options._IsUsingModule)
        {
            const auto result = SelectRendererVideoMode2(*MonitorState._Width, *MonitorState._Height, *MonitorState._BitsPerPixel, 0);

            if (!result)
            {
                *RendererState.Options._IsUsingModule = FALSE;
                *RendererState._IsActive = FALSE;
            }
        }
    }

    u32 SelectRendererVideoMode2(const u32 width, const u32 height, const u32 bpp, const u32 indx)
    {
        const auto bitsPerPixel = bpp < GRAPHICS_BITS_PER_PIXEL_16 ? GRAPHICS_BITS_PER_PIXEL_16 : bpp;

        if (!*RendererModule.State._IsActive)
        {
            if (!InitializeRendererModule2(*RendererModule.State._HWND))
            {
                return FALSE;
            }
        }

        if (RendererModule.SelectGameWindowMode != NULL)
        {
            RendererModule.SelectGameWindowMode(*RendererWindow._WindowMode);
        }

        if (!RendererModule.SelectVideoMode(width, height, bitsPerPixel, RendererState.UINT_ARRAY_0067f040, indx))
        {
            *RendererState._IsActive = FALSE;

            return FALSE;
        }

        (*RendererModule._SetColorTable)(RendererState.Colors._RGB, RendererState.Colors16Bit._RGBA);

        SelectRendererColors();

        *RendererState._IsActive = TRUE;
        *RendererState._BitsPerPixel = bitsPerPixel;

        return TRUE;
    }

    u32 SelectRendererGraphicsCard(const u32 indx)
    {
        if (!*RendererModule.State._IsActive) { return FALSE; }

        *RendererModule.State._SelectedDeviceIndex = indx;

        return RendererModule.SelectGraphicsCard(indx);
    }

    void ResetRendererTextureCache1(void)
    {
        LogMessage("[TEXTURE] Resetting texture cache...\n");

        *TextureCacheState._CurrentIndex = INVALID_TEXTURE_INDEX;
        *TextureCacheState._NextIndex = MIN_VALID_TEXTURE_INDEX;

        for (u32 x = MIN_VALID_TEXTURE_INDEX; x < *TextureCacheState._Count; x++)
        {
            ReleaseRendererTexture2(&(*TextureCacheState._Textures)[x]);
        }

        if (RendererModule.ResetTextureCache != NULL)
        {
            RendererModule.ResetTextureCache();
        }

        *TextureCacheState._Unknown2 = FALSE;
    }

    void ReleaseRendererTexture1(Texture* tex)
    {
        if (tex->Indexes.RendererIndex != INVALID_TEXTURE_INDEX)
        {
            if (RendererModule.FreeTexture != NULL)
            {
                RendererModule.FreeTexture(tex->Indexes.RendererIndex);
            }

            tex->Indexes.RendererIndex = INVALID_TEXTURE_INDEX;
        }
    }

    void ReleaseRendererTexture2(Texture* tex)
    {
        ReleaseRendererTexture1(tex);
        ReleaseRendererTexture3(tex);

        ZeroMemory(tex, sizeof(Texture));
    }

    void ReleaseRendererTexture3(Texture* tex)
    {
        if (tex->Data.ARGB != NULL)
        {
            ReleaseMemory1(tex->Data.ARGB);
            tex->Data.ARGB = NULL;
        }

        if (tex->Data.Alphas != NULL)
        {
            ReleaseMemory1(tex->Data.Alphas);
            tex->Data.Alphas = NULL;
        }

        if (tex->Data.Colors != NULL)
        {
            ReleaseMemory1(tex->Data.Colors);
            tex->Data.Colors = NULL;
        }
    }

    void SelectRendererColors(void)
    {
        switch (*MonitorState._BitsPerPixel)
        {
        case GRAPHICS_BITS_PER_PIXEL_16:
        {
            for (u32 x = 0; x < MAX_COLOR_TABLE_BUFFER_SIZE; x++)
            {
                RendererState.Colors16Bit._RGBA[x] =
                    (u16)(RendererState.Colors._RGB[x * 3 + 0] / *RendererFont.Color.Shift._R << (*RendererMonitor.Color.Shift._R & 0x1f))
                    | (u16)(RendererState.Colors._RGB[x * 3 + 1] / *RendererFont.Color.Shift._G << (*RendererMonitor.Color.Shift._G & 0x1f))
                    | (u16)(RendererState.Colors._RGB[x * 3 + 2] / *RendererFont.Color.Shift._B << (*RendererMonitor.Color.Shift._B & 0x1f));
            }

            const u32 unk1 = 0xff >> (*RendererState.Colors16Bit.Unknown._Unknown01 & 0x1f);
            *RendererState.Colors16Bit.Unknown._Unknown02 = unk1 << (*RendererMonitor.Color.Shift._R & 0x1f);

            const u32 unk2 = 0xff >> (*RendererState.Colors16Bit.Unknown._Unknown03 & 0x1f);
            *RendererState.Colors16Bit.Unknown._Unknown04 = unk2 << (*RendererMonitor.Color.Shift._G & 0x1f);

            const u32 unk3 = 0xff >> (*RendererState.Colors16Bit.Unknown._Unknown05 & 0x1f);
            *RendererState.Colors16Bit.Unknown._Unknown06 = unk3 << (*RendererMonitor.Color.Shift._B & 0x1f);

            *RendererState.Colors16Bit.Unknown._Unknown07 = (unk1 << (*RendererState.Colors16Bit.Unknown._Unknown01 & 0x1f)) << 0x10;
            *RendererState.Colors16Bit.Unknown._Unknown08 = (unk2 << (*RendererState.Colors16Bit.Unknown._Unknown03 & 0x1f)) << 8;

            *RendererState.Colors16Bit.Unknown._Unknown09 = *RendererState.Colors16Bit.Unknown._Unknown05;
            *RendererState.Colors16Bit.Unknown._Unknown10 = *RendererState.Colors16Bit.Unknown._Unknown01 + *RendererState.Colors16Bit.Unknown._Unknown03 + *RendererState.Colors16Bit.Unknown._Unknown05;
            *RendererState.Colors16Bit.Unknown._Unknown11 = unk3 << (*RendererState.Colors16Bit.Unknown._Unknown05 & 0x1f);
            *RendererState.Colors16Bit.Unknown._Unknown12 = *RendererState.Colors16Bit.Unknown._Unknown03 + *RendererState.Colors16Bit.Unknown._Unknown05;

            break;
        }
        case GRAPHICS_BITS_PER_PIXEL_32:
        {
            for (u32 x = 0; x < MAX_COLOR_TABLE_BUFFER_SIZE; x++)
            {
                RendererState.Colors32Bit._RGBA[x] =
                    ((u32)RendererState.Colors._RGB[x * 3 + 0] << (*RendererMonitor.Color.Shift._R & 0x1f))
                    | ((u32)RendererState.Colors._RGB[x * 3 + 1] << (*RendererMonitor.Color.Shift._G & 0x1f))
                    | ((u32)RendererState.Colors._RGB[x * 3 + 2] << (*RendererMonitor.Color.Shift._B & 0x1f));
            }
            break;
        }
        }
    }

    void SelectRendererAmbientLight(const f32 color)
    {
        *RendererState.Light.Color._R = color;
        *RendererState.Light.Color._G = color;
        *RendererState.Light.Color._B = color;

        if (*RendererState._IsActive)
        {
            if (RendererModule.SetAmbientLightColor != NULL)
            {
                RendererModule.SetAmbientLightColor(color, color, color);
                return;
            }

            if (RendererModule.SetAmbientLight != NULL)
            {
                RendererModule.SetAmbientLight(color);
            }
        }
    }

    void EndRendererScene(void)
    {
        if (RendererModule.EndScene != NULL && *RendererState._IsActive)
        {
            RendererModule.EndScene();
        }
    }

    void SelectRendererLightConstants2(const f32 diffuse, const f32 specular, const f32 power)
    {
        *RendererState.Light.Constant._Diffuse = diffuse;
        *RendererState.Light.Constant._Specular = specular;
        *RendererState.Light.Constant._Power = power;

        if (RendererModule.SetLightConstants != NULL && *RendererState._IsActive)
        {
            RendererModule.SetLightConstants(diffuse, specular, power, *RendererState.Light.Color._A);
        }
    }

    void SelectRendererTextureClamp(const RendererClampMode u, const RendererClampMode v, const u32 indx)
    {
        if (RendererModule.SetTextureClamp != NULL && *RendererState._IsActive)
        {
            RendererModule.SetTextureClamp(u, v, indx);
        }
    }

    void SelectRendererLighting(const RendererLightingMode mode)
    {
        *RendererState.Light._Mode = mode;

        if (RendererModule.EnableLighting != NULL)
        {
            RendererModule.EnableLighting(mode);
        }
    }

    void SelectRendererLightConstants4(const u32 color)
    {
        *RendererState.Light.Constant.Color._R = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * ((color >> 16) & 0xff);
        *RendererState.Light.Constant.Color._G = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * ((color >> 8) & 0xff);
        *RendererState.Light.Constant.Color._B = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * ((color >> 0) & 0xff);

        const auto alpha = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * ((color >> 24) & 0xff);

        if (*RendererState.Light.Color._A != alpha)
        {
            *RendererState.Light.Color._A = alpha;

            if (RendererModule.SetLightConstants != NULL && *RendererState._IsActive)
            {
                RendererModule.SetLightConstants(*RendererState.Light.Constant._Diffuse,
                    *RendererState.Light.Constant._Specular, *RendererState.Light.Constant._Power, alpha);
            }
        }

        *RendererState.Color._Clear = color;
    }

    void SelectRendererSourceBlend(const RendererSourceBlendMode mode)
    {
        if (RendererModule.SetSourceBlend != NULL)
        {
            RendererModule.SetSourceBlend(mode);
        }
    }

    void SelectRendererLightColor(const f32 r, const f32 g, const f32 b, const u32 indx)
    {
        RendererState.Light.Colors._R[indx] = r;
        RendererState.Light.Colors._G[indx] = g;
        RendererState.Light.Colors._B[indx] = b;

        if (RendererModule.SetLightColor != NULL)
        {
            RendererModule.SetLightColor(r, g, b, indx);
        }
    }

    void SelectRendererDestinationBlend(const RendererDestinationBlend mode)
    {
        *RendererState.Mode._DestinationBlend = mode;

        if (RendererModule.SetDestinationBlend != NULL)
        {
            RendererModule.SetDestinationBlend(mode);
        }
    }

    void SelectRendererLightConstants1(const f32 alpha)
    {
        const auto alphaValue = Clamp<f32>(alpha, 0.0f, 1.0f);

        *RendererState.Light.Color._A = alphaValue;

        if (RendererModule.SetLightConstants != NULL && *RendererState._IsActive)
        {
            RendererModule.SetLightConstants(*RendererState.Light.Constant._Diffuse,
                *RendererState.Light.Constant._Specular, *RendererState.Light.Constant._Power, alpha);
        }

        const auto alphaColor = (u32)roundf(alphaValue * 255.0f);

        *RendererState.Light.Constant._UnknownAlpha = alphaColor;
        *RendererState.Color._Clear = *RendererState.Color._Clear & 0xffffff | (alphaColor << 24); // TODO: color masks
    }

    u32 SelectRendererMultiTextureBlend(const RendererBlendOperation operation)
    {
        *RendererState.Mode._MultiTextureBlend = operation;

        if (RendererModule.SetMultiTextureBlend != NULL)
        {
            return RendererModule.SetMultiTextureBlend(operation);
        }

        return FALSE;
    }

    Matrix4x4* AcquireRendererTransformW2L(void)
    {
        return RendererState.Transform._W2L;
    }

    void SelectRendererLightVector(const f32x3* xyz, const f32 length, const u32 indx)
    {
        auto len = xyz->Z * xyz->Z + xyz->X * xyz->X + xyz->Y * xyz->Y;

        auto normalized = (0.0f < len) ? (length / sqrtf(len)) : 0.0f;

        if (RendererModule.SetLightVector != NULL && *RendererState._IsActive)
        {
            const f32x3 position =
            {
                .X = xyz->X * normalized,
                .Y = xyz->Y * normalized,
                .Z = xyz->Z * normalized
            };

            RendererModule.SetLightVector(&position, length, indx);
        }

        (*RendererState.Transform._TransformPointer)->Light.XYZ[indx].X = xyz->X * normalized;
        (*RendererState.Transform._TransformPointer)->Light.XYZ[indx].Y = xyz->Y * normalized;
        (*RendererState.Transform._TransformPointer)->Light.XYZ[indx].Z = xyz->Z * normalized;

        *RendererState.Fog._Mode = TRUE;
    }

    void SelectRendererFog(const u32 r, const u32 g, const u32 b, const f32 start, const f32 end)
    {
        *RendererState.Fog.Color._R = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * r;
        *RendererState.Fog.Color._G = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * g;
        *RendererState.Fog.Color._B = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * b;

        *RendererState.Fog._Start = start;
        *RendererState.Fog._End = end;

        if (*MonitorState._BitsPerPixel == GRAPHICS_BITS_PER_PIXEL_32)
        {
            *RendererState.Fog.Color._Color =
                (b << (*RendererMonitor.Color.Shift._B & 0x1f))
                | (g << (*RendererMonitor.Color.Shift._G & 0x1f))
                | (r << (*RendererMonitor.Color.Shift._R & 0x1f));
        }
        else
        {
            *RendererState.Fog.Color._Color = (r << 16) | (g << 8) | b;
        }

        if (RendererModule.SetFog != NULL && *RendererState._IsActive)
        {
            RendererModule.SetFog(*RendererState.Fog.Color._R, *RendererState.Fog.Color._G, *RendererState.Fog.Color._B, start, end);
        }

        *RendererState.Fog._Mode = TRUE;
        *RendererState.Fog._Unknown = 1.0f / (*RendererState.Fog._End - *RendererState.Fog._Start);
    }

    void SelectRendererClearColor(const u32 color)
    {
        *RendererState.Light.Constant.Color._R = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * ((color >> 16) & 0xff);
        *RendererState.Light.Constant.Color._G = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * ((color >> 8) & 0xff);
        *RendererState.Light.Constant.Color._B = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * ((color >> 0) & 0xff);

        const auto alpha = (u32)roundf(255.0f * *RendererState.Light.Color._A);

        *RendererState.Color._Clear = color & 0x00ffffff | alpha << 24;
    }

    u32 SelectRendererRectangle(const u32 x, const u32 y, const u32 width, const u32 height)
    {
        if (RendererModule.SetDirtyRect != NULL)
        {
            RendererModule.SetDirtyRect(x, y, width, height);
        }

        return TRUE;
    }

    void ActivateRendererClipping(void)
    {
        const auto clipping = RendererClippingMode::On | RendererClippingMode::Unknown2 | RendererClippingMode::Unknown4 | RendererClippingMode::Unknown8 | RendererClippingMode::Unknown10;

        *RendererState.Mode._Clipping = clipping;

        if (*RendererModule._EnableClipping != NULL && *RendererState._IsActive)
        {
            (*RendererModule._EnableClipping)(clipping);
        }
    }

    void DeActivateRendererClipping(void)
    {
        *RendererState.Mode._Clipping = RendererClippingMode::Off;

        if (*RendererModule._EnableClipping != NULL && *RendererState._IsActive)
        {
            (*RendererModule._EnableClipping)(RendererClippingMode::Off);
        }
    }

    f32 CalculateAverateRendererLightColor(void)
    {
        return 0.33333334f * (*RendererState.Light.Color._R + *RendererState.Light.Color._G + *RendererState.Light.Color._B);
    }

    void BeginRendererScene(void)
    {
        if (RendererModule.BeginScene != NULL && *RendererState._IsActive)
        {
            RendererModule.BeginScene();
        }
    }

    void BeginRendererRenderToTexture(const u32 indx)
    {
        *RendererState._IsRenderingToTexture = TRUE;

        if (RendererModule.BeginRenderToTexture != NULL)
        {
            RendererModule.BeginRenderToTexture(indx);
            return;
        }

        // Note: Originally an error.
        LogMessage("[WARNING] Current renderer does not support rendering to texture.\n");
    }

    void EndRendererRenderToTexture(void)
    {
        *RendererState._IsRenderingToTexture = FALSE;

        if (RendererModule.EndRenderToTexture != NULL)
        {
            RendererModule.EndRenderToTexture();
        }
    }

    // TODO: No need to return anything.
    BOOL SelectRendererVideModeResetTextureCache(void)
    {
        SelectRendererVideoMode1();
        ResetRendererTextureCache1();

        return TRUE;
    }

    void AllocateRendererTexture(Texture* tex)
    {
        ReleaseRendererTexture1(tex);

        if (*RendererState._IsActive && RendererModule.AllocateTexture != NULL)
        {
            tex->Indexes.RendererIndex = RendererModule.AllocateTexture(tex);

            if (tex->Indexes.RendererIndex == 0)
            {
                LogError("Unable to allocate a texture on the video card. Please lower your graphics settings.");
            }
        }
    }

    u32 ReleaseRenderer(void)
    {
        if (*RendererModule.State._IsActive)
        {
            RendererModule.Kill();

            ReleaseRendererModule();
        }

        return TRUE;
    }

    void UploadRendererTexture(Texture* tex)
    {
        if (*RendererState._IsActive && tex->Data.ARGB != NULL)
        {
            if (tex->Indexes.RendererIndex == 0)
            {
                AllocateRendererTexture(tex);
            }

            if (*RendererModule._UploadTexture != NULL)
            {
                (*RendererModule._UploadTexture)(tex->Indexes.RendererIndex, tex);
            }
        }
    }

    void SaveRendererVideoMode(void)
    {
        LogMessage("[RENDERER] Saving video mode...\n");

        if (*RendererState.Options._IsUsingModule && *RendererState._IsVideoMode)
        {
            if (RendererModule.VideoSave != NULL)
            {
                RendererModule.VideoSave();
                *RendererState._IsActive = FALSE;
                return;
            }

            RestoreRendererVideoMode1();

            if (*RendererWindow._WindowMode == RendererWindowMode::Off)
            {
                ShowWindow(*AppWindowState._HWND, SW_MINIMIZE);
            }
        }
    }

    u32 RestoreRendererVideoMode1(void)
    {
        if (*RendererState._IsActive)
        {
            *RendererState._IsActive = FALSE;

            if (RendererModule.RestoreVideoMode != NULL)
            {
                return RendererModule.RestoreVideoMode();
            }
        }

        return FALSE;
    }

    BOOL RestoreRendererVideoMode2(BOOL value)
    {
        *RendererState._IsVideoMode = FALSE;

        if (!*RendererState.Options._IsUsingModule) { return value; }

        if (*RendererState._IsActive)
        {
            *RendererState._IsActive = FALSE;

            if (RendererModule.RestoreVideoMode != NULL)
            {
                return RendererModule.RestoreVideoMode();
            }
        }

        return FALSE;
    }

    void SelectRendererAlpha(const u32 value)
    {
        RendererState.Mode.Alpha = value;

        if (RendererModule.SetAlpha != NULL)
        {
            RendererModule.SetAlpha(value);
        }
    }

    u32 AcquireRendererAlpha(void)
    {
        return RendererState.Mode.Alpha;
    }

    void SelecteRendererCulling(const RendererCullMode mode)
    {
        *RendererState.Mode._CullMode = mode;

        if (RendererModule.EnableCulling != NULL && *RendererState._IsActive)
        {
            RendererModule.EnableCulling(mode);
        }
    }

    void SelectRendererRenderTexture(const u32 indx)
    {
        if (*RendererModule._SelectRenderTexture != NULL)
        {
            (*RendererModule._SelectRenderTexture)(indx);
        }
    }

    void SelectRendererTexture(Texture* tex, const u32 indx)
    {
        if (*RendererState._IsActive)
        {
            if (tex->Indexes.RendererIndex == 0)
            {
                UploadRendererTexture(tex);
            }

            if (tex->Indexes.RendererIndex != 0 && RendererModule.SelectTexture != NULL)
            {
                RendererModule.SelectTexture(tex->Indexes.RendererIndex, indx);
            }
        }
    }

    u32 AcquireRendererClearColor(void)
    {
        return *RendererState.Color._Clear;
    }

    void SelectRendererOmniLight(const f32xRGB* rgb, const f32 alpha, const u32 color)
    {
        if (RendererModule.SetOmniLight != NULL)
        {
            RendererModule.SetOmniLight(rgb, alpha, color);
        }
    }

    void SelectRendererLightConstants3(const f32 a, const f32 r, const f32 g, const f32 b)
    {
        *RendererState.Light.Constant.Color._R = r;
        *RendererState.Light.Constant.Color._G = g;
        *RendererState.Light.Constant.Color._B = b;

        if (*RendererState.Light.Color._A != a)
        {
            *RendererState.Light.Color._A = a;

            if (RendererModule.SetLightConstants != NULL && *RendererState._IsActive)
            {
                RendererModule.SetLightConstants(*RendererState.Light.Constant._Diffuse,
                    *RendererState.Light.Constant._Specular, *RendererState.Light.Constant._Power, a);
            }
        }

        const auto ac = (u32)roundf(255.0f * a);
        const auto rc = (u32)roundf(255.0f * r);
        const auto gc = (u32)roundf(255.0f * g);
        const auto bc = (u32)roundf(255.0f * b);

        *RendererState.Color._Clear = (ac << 24) | (rc << 16) | (gc << 8) | bc;
    }

    BOOL LockRendererFrame(void)
    {
        if (!*RendererState._IsActive)
        {
            *RendererState.Lock._Mode = *RendererState._IsActive;
        }
        else
        {
            if (RendererModule.LockFrame())
            {
                *RendererState.Lock._BitsPerPixel = *MonitorState._BitsPerPixel;

                *MonitorState._BitsPerPixel = *RendererState._BitsPerPixel;

                *RendererState._IsActive = FALSE;
                *RendererState.Lock._Mode = TRUE;
            }
        }

        return TRUE;
    }

    BOOL AcquireRendererGraphicsCards(u32* count, char** apis, char** names, u32* dids, u32* vids)
    {
        if (!*RendererModule.State._IsActive) { return FALSE; }

        return RendererModule.AcquireGraphicsCardList(count, apis, names, dids, vids);
    }

    void SelectRendererTransform(void)
    {
        if (*RendererState.Fog._Mode)
        {
            *RendererState.Fog._Mode = FALSE;

            if (RendererModule.SetTransform != NULL && *RendererState._IsActive)
            {
                RendererModule.SetTransform(*RendererState.Transform._TransformPointer, *RendererState.Transform._zNear, *RendererState.Transform._zFar);
            }
        }
    }

    void ResetRendererTextureCache2(void)
    {
        ResetRendererTextureCache1();
        ReleaseMemory3(*TextureCacheState._Textures);

        *TextureCacheState._Count = 0;
        *TextureCacheState._NextIndex = 0;
        *TextureCacheState._Textures = NULL;
    }

    BOOL UnlockRendererFrame1(void)
    {
        return UnlockRendererFrame2(FALSE);
    }

    // TODO param as an enum?
    BOOL UnlockRendererFrame2(const BOOL mode)
    {
        if (*RendererState.Lock._Mode)
        {
            *RendererState._IsActive = TRUE;
            *MonitorState._BitsPerPixel = *RendererState.Lock._BitsPerPixel;

            if (mode)
            {
                *RendererState.Lock._Mode = FALSE;
            }

            return RendererModule.UnlockFrame();
        }

        return FALSE;
    }


    BOOL ToggleRendererScreen(const ToggleMode mode)
    {
        if (!*RendererState._IsActive) { return FALSE; }

        if (mode == ToggleMode::Unknown0 && !*RendererState.Lock._Mode)
        {
            // TODO RendererWindow.Unk4

            LockRendererFrame();

            // TODO RendererWindow.Unk4

            UnlockRendererFrame2(FALSE);
        }

        return RendererModule.Toggle();
    }
    
    void ClearRendererScreen(const RendererClearMode mode)
    {
        if ((mode & (RendererClearMode::Target | RendererClearMode::Depth)) != RendererClearMode::None)
        {
            if (*RendererState._IsActive)
            {
                auto color = *RendererState.Color._Clear;

                if ((mode & RendererClearMode::ClearColor) == RendererClearMode::None)
                {
                    color = GRAPHICS_COLOR_BLACK;

                    if ((mode & RendererClearMode::FogColor) != RendererClearMode::None)
                    {
                        const auto r = (u32)roundf(*RendererState.Fog.Color._R * 255.0f);
                        const auto g = (u32)roundf(*RendererState.Fog.Color._G * 255.0f);
                        const auto b = (u32)roundf(*RendererState.Fog.Color._B * 255.0f);

                        color = (r << 24) | (g << 16) | b;
                    }
                }

                if (RendererModule.Clear != NULL)
                {
                    RendererModule.Clear(mode, color);
                }

                return;
            }

            if ((mode & RendererClearMode::Target) != RendererClearMode::None)
            {
                PopulateRendererUnknown41();
            }

            if ((mode & RendererClearMode::Depth) != RendererClearMode::None)
            {
                PopulateRendererUnknown42();
            }
        }
    }

    u32 SelectRendererViewPort(const f32 x0, const f32 y0, const f32 x1, const f32 y1)
    {
        auto actualX0 = x0;
        auto actualY0 = y0;
        auto actualX1 = x1;
        auto actualY1 = y1;

        if (!*RendererState.ViewPort._IsComplete)
        {
            actualX0 = x0 * *RendererState.ViewPort._Scale;
            actualY0 = y0 * *RendererState.ViewPort._Scale;
            actualX1 = x1 * *RendererState.ViewPort._Scale;
            actualY1 = y1 * *RendererState.ViewPort._Scale;
        }

        {
            const auto halfWidth = 0.5f * (x1 - x0);
            const auto halfHeight = 0.5f * (y1 - y0);

            *RendererState.ViewPort.Box._X0 = roundf(actualX0);
            *RendererState.ViewPort.Box._Y0 = roundf(actualY0);
            *RendererState.ViewPort.Box._X1 = roundf(actualX1);
            *RendererState.ViewPort.Box._Y1 = roundf(actualY1);

            *RendererState.ViewPort.Box._CenterX = actualX0 + halfWidth;
            *RendererState.ViewPort.Box._HalfWidth = halfWidth;
            *RendererState.ViewPort.Box._MaxWidth = (u32)(actualX1)-1;

            *RendererState.ViewPort.Box._CenterY = actualY0 + halfHeight;
            *RendererState.ViewPort.Box._HalfHeight = -halfHeight;
            *RendererState.ViewPort.Box._MaxHeight = (u32)(actualY1)-1;

            *RendererState.ViewPort.Box._RoundX = (u32)*RendererState.ViewPort.Box._X0;
            *RendererState.ViewPort.Box._RoundY = (u32)*RendererState.ViewPort.Box._Y0;
        }

        if (!*RendererState.ViewPort._IsComplete && RendererModule.SetViewPort != NULL && *RendererState._IsActive)
        {
            return RendererModule.SetViewPort(actualX0, actualY0, actualX1, actualY1, *RendererState.ViewPort._zNear, *RendererState.ViewPort._zFar);
        }

        return FALSE;
    }

    void SelectRendererTransformW2L(const void*, const Mathematics::Matrix4x4* matrix, const f32 zNear, const f32 zFar)
    {
        SelectRendererTransform();

        SelectRendererTransformO2W(TRUE);

        MultiplyMatrix(RendererState.Transform._W2L, RendererState.Transform._O2W, matrix);

        if (RendererModule.SetTransformW2L != NULL)
        {
            RendererModule.SetTransformW2L(RendererState.Transform._W2L, zNear, zFar);
        }

        return;
    }

    void SelectRendererMode(void)
    {
        *RendererState.Mode._UnknownMode1 = *RendererState.Mode._UnknownMode2 | *RendererState.Mode._UnknownMode3;
        *RendererState.Mode._UnknownMode4 = 0;

        if (*RendererState.Mode._UnknownMode5)
        {
            *RendererState.Mode._UnknownMode1 = *RendererState.Mode._UnknownMode1 | RendererMode::U0x8;
        }

        if (!*RendererState.Mode._UnknownMode6)
        {
            *RendererState.Mode._DestinationBlendCopy1 = *RendererState.Mode._DestinationBlend;
            return;
        }

        *RendererState.Mode._UnknownMode1 = *RendererState.Mode._UnknownMode1 | (RendererMode::U0x100 | RendererMode::U0x20 | RendererMode::U0x2);
        *RendererState.Mode._DestinationBlendCopy1 = *RendererState.Mode._DestinationBlend;
    }

    void ReUploadRendererTextures(void)
    {
        if (RendererModule.ResetTextureCache == NULL) { return; }

        for (u32 x = MIN_VALID_TEXTURE_INDEX; x < *TextureCacheState._NextIndex; x++)
        {
            ReleaseRendererTexture1(&(*TextureCacheState._Textures)[x]);
        }

        RendererModule.ResetTextureCache();

        for (u32 x = MIN_VALID_TEXTURE_INDEX; x < *TextureCacheState._NextIndex; x++)
        {
            UploadRendererTexture(&(*TextureCacheState._Textures)[x]);
        }
    }

    void SelectRendererFogMode(void)
    {
        if (RendererModule.SetFog != NULL && *RendererState._IsActive)
        {
            *RendererState.Mode._UnknownMode5 = TRUE;
        }
    }

    void SelectRendererUnknownMode6FALSE(void)
    {
        *RendererState.Mode._UnknownMode6 = FALSE;
    }

    void SelectUnknownRendererMode6TRUE(void)
    {
        *RendererState.Mode._UnknownMode6 = TRUE;
    }

    Assets::Texture* FindRendererTexture1(TextureDescriptor* desc)
    {
        const auto cacheIndex = desc->TextureCacheIndex;

        if (INVALID_TEXTURE_INDEX < cacheIndex && cacheIndex < *TextureCacheState._NextIndex)
        {
            const auto tex = &(*TextureCacheState._Textures)[cacheIndex];

            if (EqualStrings(desc->Name, tex->Name))
            {
                return tex;
            }
        }

        for (u32 x = MIN_VALID_TEXTURE_INDEX; x < *TextureCacheState._NextIndex; x++)
        {
            const auto tex = &(*TextureCacheState._Textures)[x];

            if (EqualStrings(desc->Name, tex->Name))
            {
                desc->TextureCacheIndex = x;
                return tex;
            }
        }

        return NULL;
    }

    void SelectUnknownRendererMode2(const RendererMode mode)
    {
        *RendererState.Mode._UnknownMode2 = mode;
    }

    void ReleaseRendererTexture4(TextureDescriptor* desc)
    {
        auto tex = FindRendererTexture1(desc);

        if (tex != NULL)
        {
            ReleaseRendererTexture2(tex);
        }

        desc->TextureCacheIndex = FREE_TEXTURE_INDEX;
    }

    void ReleaseRendererTextures1(void)
    {
        ReleaseRendererTexture4(TextureDescriptorState._MENU_BACKGROUND_INGAME);
        ReleaseRendererTexture4(TextureDescriptorState._OB_AR_TARGETFILE);
        ReleaseRendererTexture4(TextureDescriptorState._OB_DE_TARGETFILE);
        ReleaseRendererTexture4(TextureDescriptorState._OB_STAMP);
        ReleaseRendererTexture4(TextureDescriptorState._OB_LA_NEWTOWN_MAP);
        ReleaseRendererTexture4(TextureDescriptorState._EAX_LOGO);
    }

    u32 AcquireAvailableRendererTextureIndex(void)
    {
        u32 result = 0;

        if (*TextureCacheState._NextIndex < *TextureCacheState._Count)
        {
            result = *TextureCacheState._NextIndex;
            *TextureCacheState._NextIndex = *TextureCacheState._NextIndex + 1;
        }
        else
        {
            s32 min = S32_MIN + 1;
            u32 indx = MIN_VALID_TEXTURE_INDEX;

            for (u32 x = MIN_VALID_TEXTURE_INDEX; x < *TextureCacheState._Count; x++)
            {
                result = x;
                const auto tex = &(*TextureCacheState._Textures)[x];

                if (tex->Name == NULL) { break; }

                const s32 diff = (s32)*TextureCacheState._CurrentIndex - (s32)tex->Indexes.CacheIndex;
                if (min < diff)
                {
                    min = diff;
                    indx = x;
                }
            }

            result = indx;
        }

        ReleaseRendererTexture2(&(*TextureCacheState._Textures)[result]);

        return result;
    }

    void RestoreRendererVideoMode(void)
    {
        LogMessage("[RENDERER] Restoring video mode...\n");

        if (*RendererWindow._WindowMode == RendererWindowMode::Off)
        {
            if (*RendererState.Options._IsUsingModule && *RendererState._IsVideoMode)
            {
                if (RendererModule.VideoRestore == NULL)
                {
                    if (SelectRendererVideoMode2(*MonitorState._Width, *MonitorState._Height, *MonitorState._BitsPerPixel, 0) == 0)
                    {
                        *RendererState.Options._IsUsingModule = FALSE;
                        SelectAppWindowResolution2(*MonitorState._Width, *MonitorState._Height, *MonitorState._BitsPerPixel);
                    }

                    SetFocus(*AppWindowState._HWND);
                }
                else
                {
                    RendererModule.VideoRestore();
                    SetFocus(*AppWindowState._HWND);
                    *RendererState._IsActive = TRUE;
                    ReUploadRendererTextures();
                }
            }
        }
        else
        {
            if (*RendererState.Options._IsUsingModule && *RendererState._IsVideoMode)
            {
                if (RendererModule.VideoRestore == NULL)
                {
                    if (SelectRendererVideoMode2(*MonitorState._Width, *MonitorState._Height, *MonitorState._BitsPerPixel, 0) == 0)
                    {

                        *RendererState._IsActive = FALSE;
                        *RendererState.Options._IsUsingModule = FALSE;
                    }

                    SetFocus(*AppWindowState._HWND);
                }
                else
                {
                    RendererModule.VideoRestore();
                    *RendererState._IsActive = TRUE;
                    ReUploadRendererTextures();
                }
            }
        }
    }

    void AllocateRendererTextures1(const u32 count)
    {
        LogMessage("[TEXTURE] Initializing %d textures...\n", count);

        const u32 size = count * sizeof(Texture);

        *TextureCacheState._Textures = (Texture*)AllocateMemory2(size);

        if (*TextureCacheState._Textures == NULL)
        {
            LogError("Unable to allocate texture memory.");
        }

        ZeroMemory(*TextureCacheState._Textures, size);

        *TextureCacheState._Count = count;

        ResetRendererTextureCache1();
    }

    void SelectRendererTransformO2W(BOOL mode) // TODO: make parameter an enum of 0 and 1 values
    {
        if (!mode && *RendererState.Mode._MultiTextureBlend != RendererBlendOperation::SelectArgument2
            && *RendererState.Mode._MultiTextureBlend != RendererBlendOperation::Modulate
            && *RendererState.Mode._MultiTextureBlend != RendererBlendOperation::Add) {
            return;
        }

        if (*RendererState.Transform._Unknown101 == 0)
        {
            (*RendererState.Transform._O2W).M11 = 1.0f;
            (*RendererState.Transform._O2W).M12 = (f32)*RendererState.Transform._Unknown101;
            (*RendererState.Transform._O2W).M13 = (f32)*RendererState.Transform._Unknown101;

            (*RendererState.Transform._O2W).M21 = (f32)*RendererState.Transform._Unknown101;
            (*RendererState.Transform._O2W).M22 = 1.0f;
            (*RendererState.Transform._O2W).M23 = (f32)*RendererState.Transform._Unknown101;

            (*RendererState.Transform._O2W).M31 = (f32)*RendererState.Transform._Unknown101;
            (*RendererState.Transform._O2W).M32 = (f32)*RendererState.Transform._Unknown101;
            (*RendererState.Transform._O2W).M33 = 1.0f;
            (*RendererState.Transform._O2W).M34 = (f32)*RendererState.Transform._Unknown101;

            (*RendererState.Transform._O2W).M41 = (*RendererState.Transform._UnknownMatrix1).M32 - (*RendererState.Transform._TransformPointer)->Light.Color.R;
            (*RendererState.Transform._O2W).M42 = (*RendererState.Transform._UnknownMatrix1).M33 - (*RendererState.Transform._TransformPointer)->Light.Color.G;
            (*RendererState.Transform._O2W).M43 = (*RendererState.Transform._UnknownMatrix1).M34 - (*RendererState.Transform._TransformPointer)->Light.Color.B;

            if (*RendererModule.SetTransformO2W == NULL)
            {
                (*RendererState.Transform._O2W).M14 = (f32)*RendererState.Transform._Unknown101;

                (*RendererState.Transform._O2W).M24 = (f32)*RendererState.Transform._Unknown101;

                (*RendererState.Transform._O2W).M44 = 1.0f;

                return;
            }
        }
        else
        {
            // TODO: what is this?

            (*RendererState.Transform._O2W).M11 =
                (*RendererState.Transform._TransformPointer)->O2W.M13 * (*RendererState.Transform._UnknownMatrix1).M23
                + (*RendererState.Transform._TransformPointer)->O2W.M11 * (*RendererState.Transform._UnknownMatrix1).M11
                + (*RendererState.Transform._TransformPointer)->O2W.M12 * (*RendererState.Transform._UnknownMatrix1).M14;

            (*RendererState.Transform._O2W).M12 =
                (*RendererState.Transform._TransformPointer)->O2W.M13 * (*RendererState.Transform._UnknownMatrix1).M24
                + (*RendererState.Transform._TransformPointer)->O2W.M11 * (*RendererState.Transform._UnknownMatrix1).M12
                + (*RendererState.Transform._TransformPointer)->O2W.M12 * (*RendererState.Transform._UnknownMatrix1).M21;

            (*RendererState.Transform._O2W).M13 =
                (*RendererState.Transform._TransformPointer)->O2W.M13 * (*RendererState.Transform._UnknownMatrix1).M31
                + (*RendererState.Transform._TransformPointer)->O2W.M11 * (*RendererState.Transform._UnknownMatrix1).M13
                + (*RendererState.Transform._TransformPointer)->O2W.M12 * (*RendererState.Transform._UnknownMatrix1).M22;

            (*RendererState.Transform._O2W).M14 = 0.0f;

            (*RendererState.Transform._O2W).M21 =
                (*RendererState.Transform._TransformPointer)->O2W.M23 * (*RendererState.Transform._UnknownMatrix1).M23
                + (*RendererState.Transform._TransformPointer)->O2W.M22 * (*RendererState.Transform._UnknownMatrix1).M14
                + (*RendererState.Transform._TransformPointer)->O2W.M21 * (*RendererState.Transform._UnknownMatrix1).M11;

            (*RendererState.Transform._O2W).M22 =
                (*RendererState.Transform._TransformPointer)->O2W.M23 * (*RendererState.Transform._UnknownMatrix1).M24
                + (*RendererState.Transform._TransformPointer)->O2W.M22 * (*RendererState.Transform._UnknownMatrix1).M21
                + (*RendererState.Transform._TransformPointer)->O2W.M21 * (*RendererState.Transform._UnknownMatrix1).M12;

            (*RendererState.Transform._O2W).M23 =
                (*RendererState.Transform._TransformPointer)->O2W.M23 * (*RendererState.Transform._UnknownMatrix1).M31
                + (*RendererState.Transform._TransformPointer)->O2W.M22 * (*RendererState.Transform._UnknownMatrix1).M22
                + (*RendererState.Transform._TransformPointer)->O2W.M21 * (*RendererState.Transform._UnknownMatrix1).M13;

            (*RendererState.Transform._O2W).M24 = 0.0f;

            (*RendererState.Transform._O2W).M31 =
                (*RendererState.Transform._TransformPointer)->O2W.M33 * (*RendererState.Transform._UnknownMatrix1).M23
                + (*RendererState.Transform._TransformPointer)->O2W.M32 * (*RendererState.Transform._UnknownMatrix1).M14
                + (*RendererState.Transform._TransformPointer)->O2W.M31 * (*RendererState.Transform._UnknownMatrix1).M11;

            (*RendererState.Transform._O2W).M32 =
                (*RendererState.Transform._TransformPointer)->O2W.M33 * (*RendererState.Transform._UnknownMatrix1).M24
                + (*RendererState.Transform._TransformPointer)->O2W.M32 * (*RendererState.Transform._UnknownMatrix1).M21
                + (*RendererState.Transform._TransformPointer)->O2W.M31 * (*RendererState.Transform._UnknownMatrix1).M12;

            (*RendererState.Transform._O2W).M33 =
                (*RendererState.Transform._TransformPointer)->O2W.M33 * (*RendererState.Transform._UnknownMatrix1).M31
                + (*RendererState.Transform._TransformPointer)->O2W.M32 * (*RendererState.Transform._UnknownMatrix1).M22
                + (*RendererState.Transform._TransformPointer)->O2W.M31 * (*RendererState.Transform._UnknownMatrix1).M13;

            (*RendererState.Transform._O2W).M34 = 0.0f;

            const auto value1 = (-(*RendererState.Transform._TransformPointer)->LightMatrix.M11 * (*RendererState.Transform._TransformPointer)->O2W.M11 - (*RendererState.Transform._TransformPointer)->LightMatrix.M12 * (*RendererState.Transform._TransformPointer)->O2W.M21)
                - (*RendererState.Transform._TransformPointer)->LightMatrix.M13 * (*RendererState.Transform._TransformPointer)->O2W.M31;

            const auto value2 = (-(*RendererState.Transform._TransformPointer)->LightMatrix.M11 * (*RendererState.Transform._TransformPointer)->O2W.M12 - (*RendererState.Transform._TransformPointer)->LightMatrix.M12 * (*RendererState.Transform._TransformPointer)->O2W.M22)
                - (*RendererState.Transform._TransformPointer)->LightMatrix.M13 * (*RendererState.Transform._TransformPointer)->O2W.M32;

            const auto value3 = (-(*RendererState.Transform._TransformPointer)->LightMatrix.M11 * (*RendererState.Transform._TransformPointer)->O2W.M13 - (*RendererState.Transform._TransformPointer)->LightMatrix.M12 * (*RendererState.Transform._TransformPointer)->O2W.M23)
                - (*RendererState.Transform._TransformPointer)->LightMatrix.M13 * (*RendererState.Transform._TransformPointer)->O2W.M33;

            (*RendererState.Transform._O2W).M41 = (*RendererState.Transform._UnknownMatrix1).M32
                + value3 * (*RendererState.Transform._UnknownMatrix1).M23
                + value2 * (*RendererState.Transform._UnknownMatrix1).M14
                + value1 * (*RendererState.Transform._UnknownMatrix1).M11;

            (*RendererState.Transform._O2W).M42 = (*RendererState.Transform._UnknownMatrix1).M33
                + value3 * (*RendererState.Transform._UnknownMatrix1).M24
                + value2 * (*RendererState.Transform._UnknownMatrix1).M21
                + value1 * (*RendererState.Transform._UnknownMatrix1).M12;

            (*RendererState.Transform._O2W).M43 = (*RendererState.Transform._UnknownMatrix1).M34
                + value3 * (*RendererState.Transform._UnknownMatrix1).M31
                + value2 * (*RendererState.Transform._UnknownMatrix1).M22
                + value1 * (*RendererState.Transform._UnknownMatrix1).M13;

            (*RendererState.Transform._O2W).M44 = 1.0f;

            if (RendererModule.SetTransformO2W == NULL) { return; }
        }

        RendererState.Transform._O2W->M44 = 1.0f;
        RendererState.Transform._O2W->M14 = RendererState.Transform._O2W->M34;
        RendererState.Transform._O2W->M24 = RendererState.Transform._O2W->M34;

        RendererModule.SetTransformO2W(RendererState.Transform._O2W);
    }

    void InitializeTLVertex1(ProperTLVertex* vertex)
    {
        if (*RendererState.Mode._UnknownMode5 != 0)
        {
            vertex->Unk14 = Clamp((vertex->Unk3 - *RendererState.Fog._Start) * *RendererState.Fog._Unknown, 0.0f, 1.0f);
        }

        vertex->Unk14 = 0.0f;
    }

    void InitializeTLVertex2(f32x3* xyz, ProperTLVertex* vertex)
    {
        const auto transform = *RendererState.Transform._TransformPointer;

        vertex->Unk1 = (xyz->Z - transform->LightMatrix.M13) * transform->O2W.M31
            + (xyz->X - transform->LightMatrix.M11) * transform->O2W.M11
            + (xyz->Y - transform->LightMatrix.M12) * transform->O2W.M21;

        vertex->Unk2 = (xyz->Z - transform->LightMatrix.M13) * transform->O2W.M32
            + (xyz->Y - transform->LightMatrix.M12) * transform->O2W.M22
            + (xyz->X - transform->LightMatrix.M11) * transform->O2W.M12;

        vertex->Unk3 = (xyz->Z - transform->LightMatrix.M13) * transform->O2W.M33
            + (xyz->Y - transform->LightMatrix.M12) * transform->O2W.M23
            + (xyz->X - transform->LightMatrix.M11) * transform->O2W.M13;

        if (*RendererState._Unknown2 == 0)
        {
            const auto abs = fabsf(vertex->Unk3);

            vertex->Unk5 = 0;

            if (abs < vertex->Unk1)
            {
                vertex->Unk5 = vertex->Unk5 | 1; // TODO
            }

            if (vertex->Unk1 < -abs)
            {
                vertex->Unk5 = vertex->Unk5 | 2; // TODO
            }

            if (abs < vertex->Unk2)
            {
                vertex->Unk5 = vertex->Unk5 | 4; // TODO
            }

            if (vertex->Unk2 < -abs)
            {
                vertex->Unk5 = vertex->Unk5 | 8; // TODO
            }

            if (vertex->Unk3 <= *RendererState.ViewPort._zNear)
            {
                vertex->Unk5 = vertex->Unk5 | 0x10; // TODO

                return;
            }

            if (vertex->Unk5 == 0) // TODO
            {
                vertex->Unk4 = 1.0f / vertex->Unk3;

                vertex->Unk6 = *RendererState.ViewPort.Box._CenterX + (vertex->Unk1 * *RendererState.ViewPort.Box._HalfWidth) * vertex->Unk4;
                vertex->Unk7 = *RendererState.ViewPort.Box._CenterY + (vertex->Unk2 * *RendererState.ViewPort.Box._HalfHeight) * vertex->Unk4;

                return;
            }
        }
        else
        {
            vertex->Unk5 = 0;

            if (*RendererState.ViewPort.Box._HalfWidth < vertex->Unk1)
            {
                vertex->Unk5 = vertex->Unk5 | 1; // TODO
            }

            if (vertex->Unk1 < -*RendererState.ViewPort.Box._HalfWidth)
            {
                vertex->Unk5 = vertex->Unk5 | 2; // TODO
            }

            if (-*RendererState.ViewPort.Box._HalfHeight < vertex->Unk2)
            {
                vertex->Unk5 = vertex->Unk5 | 4; // TODO
            }

            if (vertex->Unk2 < *RendererState.ViewPort.Box._HalfHeight)
            {
                vertex->Unk5 = vertex->Unk5 | 8; // TODO
            }

            if (vertex->Unk3 < *RendererState.ViewPort._zNear)
            {
                vertex->Unk5 = vertex->Unk5 | 0x10; // TODO

                return;
            }

            if (vertex->Unk5 == 0)
            {
                vertex->Unk6 = vertex->Unk1 + *RendererState.ViewPort.Box._CenterX;
                vertex->Unk7 = *RendererState.ViewPort.Box._CenterY - vertex->Unk2;
                vertex->Unk4 = 1.0f / vertex->Unk3;

                return;
            }
        }
    }

    typedef const void(CDECLAPI* FUN_004156D0) (void); // TODO
    FUN_004156D0 FUN_004156d0 = (FUN_004156D0)0x004156d0;//TODO

    void RenderBlood1(UnknownRendererStruct* param) // TODO
    {
        if (AppState._App->Options.Graphics.Blood == 0) { return; }

        const auto ovc = *RendererPacketState.Counters._VertexCount;
        const auto oic = *RendererPacketState.Counters._IndexCount;

        ActivateRendererClipping();

        if (0 < *RendererState.Blood._QuadCount)
        {
            LoadTextureDescriptor124(TextureDescriptorState._Blood, 0);
            SelectUnknownRendererMode6TRUE();
            UpdateLights1(RendererState.Light._XYZ);
            SelectUnknownRendererMode2(RendererMode::U0x40);

            if (*RendererState.Blood._QuadCount != 0)
            {
                auto vertexes = (RTLVX*)AllocateMemory(RENDERER_BLOOD_VERTEX_COUNT * sizeof(RTLVX));

                if (vertexes == NULL)
                {
                    LogMessage("[WARNING] Unable to allocate memory for blood vertexes.\n");
                }
                else
                {
                    u32 vi = 0;
                    u32 qi = 0;

                    auto indexes = RendererState.Blood._QuadIndexes;

                    for (u32 x = 0; x < *RendererState.Blood._QuadCount; x++)
                    {
                        const auto quad = &RendererState.Blood._Quads[x];

                        for (u32 xx = 0; xx < 4; xx++) // TODO constant
                        {
                            auto vertex = &vertexes[vi];

                            if (**RendererState._UnknownPointer2 == 1 && 2 < quad->Count)
                            {
                                vertex->XYZ.X = 0.0f;
                                vertex->XYZ.Y = 0.0f;
                                vertex->XYZ.Z = 0.0f;
                            }
                            else
                            {
                                vertex->XYZ.X = quad->X[xx];
                                vertex->XYZ.Y = quad->Y[xx];
                                vertex->XYZ.Z = quad->Z[xx];
                            }

                            vertex->RHW = 0.0f;
                            vertex->Color = quad->Color;

                            vertex->UV.X = quad->UVX[xx];
                            vertex->UV.Y = quad->UVY[xx];

                            vi = vi + 1;
                        }

                        const auto qiv = x * 4; // TODO constant

                        indexes[qi + 0] = qiv + 0;
                        indexes[qi + 1] = qiv + 1;
                        indexes[qi + 2] = qiv + 2;

                        indexes[qi + 3] = qiv + 0;
                        indexes[qi + 4] = qiv + 2;
                        indexes[qi + 5] = qiv + 3;

                        qi = qi + 6;
                    }

                    RenderRendererPacketD3DTLVertexes1(vertexes, *RendererState.Blood._QuadCount * QUAD_VERTEX_COUNT,
                        RendererState.Blood._QuadIndexes, *RendererState.Blood._QuadCount * QUAD_TRIANGLE_COUNT);

                    ReleaseMemory1(vertexes);
                }
            }
            
            SelectUnknownRendererMode2(RendererMode::U0x80 | RendererMode::U0x40);
            SelectRendererUnknownMode6FALSE();
        }

        if (0 < *RendererState.Allocated._AllocatedMemory6VertexCount)
        {
            LoadTextureDescriptor124(TextureDescriptorState._BloodPool, 0);

            SelectUnknownRendererMode6TRUE();
            UpdateLights1(RendererState.Light._XYZ);
            SelectUnknownRendererMode2(RendererMode::U0x40);

            RenderRendererPacketD3DTLVertexes1((RTLVX*)RendererState.Allocated._AllocatedMemory6,
                *RendererState.Allocated._AllocatedMemory6VertexCount,
                RendererState.Allocated._AllocatedMemory6Indexes, *RendererState.Allocated._AllocatedMemory6IndexCount);

            SelectUnknownRendererMode2(RendererMode::U0x80 | RendererMode::U0x40);
            SelectRendererUnknownMode6FALSE();
        }

        FUN_004156d0(); // TODO

        param->IndexCount = param->IndexCount + *RendererPacketState.Counters._IndexCount - oic;
        param->VertexCount = param->IndexCount + *RendererPacketState.Counters._VertexCount - ovc;
    }

    void UpdateLights1(const f32x3* xyz)
    {
        auto transform = *RendererState.Transform._TransformPointer;

        transform->LightMatrix.M11 = RendererState.Transform._UnknownMatrix1->M32 - xyz->X;
        transform->LightMatrix.M12 = RendererState.Transform._UnknownMatrix1->M33 - xyz->Y;
        transform->LightMatrix.M13 = RendererState.Transform._UnknownMatrix1->M34 - xyz->Z;

        UpdateRendererTransform1(transform);
    }

    void UpdateRendererTransform1(Transform* transform)
    {
        *RendererState.Fog._Mode = 1;

        transform->O2W.M41 = (-transform->LightMatrix.M11 * transform->O2W.M11 - transform->LightMatrix.M12 * transform->O2W.M21) - transform->LightMatrix.M13 * transform->O2W.M31;
        transform->O2W.M42 = (-transform->LightMatrix.M11 * transform->O2W.M12 - transform->LightMatrix.M12 * transform->O2W.M22) - transform->LightMatrix.M13 * transform->O2W.M32;
        transform->O2W.M43 = (-transform->LightMatrix.M11 * transform->O2W.M13 - transform->LightMatrix.M12 * transform->O2W.M23) - transform->LightMatrix.M13 * transform->O2W.M33;
        transform->O2W.M44 = 1.0f;

        transform->ModelView.M11 = transform->O2W.M11 * *RendererState._UnknownFloat1;
        transform->ModelView.M13 = transform->O2W.M13;
        transform->ModelView.M12 = transform->O2W.M12 * *RendererState._UnknownFloat2;
        transform->ModelView.M14 = 0.0f;

        transform->ModelView.M21 = transform->O2W.M21 * *RendererState._UnknownFloat1;
        transform->ModelView.M22 = transform->O2W.M22 * *RendererState._UnknownFloat2;
        transform->ModelView.M23 = transform->O2W.M23;
        transform->ModelView.M24 = 0.0f;

        transform->ModelView.M31 = transform->O2W.M31 * *RendererState._UnknownFloat1;
        transform->ModelView.M32 = transform->O2W.M32 * *RendererState._UnknownFloat2;
        transform->ModelView.M33 = transform->O2W.M33;
        transform->ModelView.M34 = 0.0f;


        transform->ModelView.M41 = (-transform->LightMatrix.M11 * transform->ModelView.M11 - transform->LightMatrix.M12 * transform->ModelView.M21) - transform->LightMatrix.M13 * transform->ModelView.M31;
        transform->ModelView.M42 = (-transform->LightMatrix.M11 * transform->ModelView.M12 - transform->LightMatrix.M12 * transform->ModelView.M22) - transform->LightMatrix.M13 * transform->ModelView.M32;
        transform->ModelView.M43 = (-transform->LightMatrix.M11 * transform->ModelView.M13 - transform->LightMatrix.M12 * transform->ModelView.M23) - transform->LightMatrix.M13 * transform->ModelView.M33;
        transform->ModelView.M44 = 1.0f;
    }

    void UpdateRendererTransform2(void)
    {
        UpdateRendererTransform1(*RendererState.Transform._TransformPointer);
    }

    void PopulateRendererUnknown41(void)
    {
        if (*RendererState._IsActive) { return; }

        // TODO incomplete, Unk4
    }

    void PopulateRendererUnknown42(void)
    {
        // TODO: incomplete Unk4
    }

    /// <summary>
    /// Used to determine the zNear to render HUD like the character portrait, actions, etc.
    /// </summary>
    f32 AcquireRendererViewPortNear(void)
    {
        *RendererState.ViewPort._UnknownValue1 = *RendererState.ViewPort._UnknownValue1 - 0.0099999998f;

        if (*RendererState.ViewPort._zNear <= *RendererState.ViewPort._UnknownValue1)
        {
            return *RendererState.ViewPort._UnknownValue1;
        }

        *RendererState.ViewPort._UnknownValue1 = *RendererState.ViewPort._zNear;

        return *RendererState.ViewPort._zNear;
    }

    /// <summary>
    /// User to render HUD like the character portrait, actions, etc.
    /// </summary>
    void RenderRectangle(const s32 x, const s32 y, const s32 width, const s32 height)
    {
        const auto zNear = AcquireRendererViewPortNear();
        const auto rhw = 1.0f / zNear;

        auto vertexes = (RTLVX*)*RendererState.Allocated._AllocatedMemory2;

        vertexes[0].XYZ.X = (f32)x;
        vertexes[0].XYZ.Y = (f32)y;
        vertexes[0].XYZ.Z = zNear;
        vertexes[0].RHW = rhw;
        vertexes[0].Color = *RendererState.Color._Clear;
        vertexes[0].Specular = 0;
        vertexes[0].UV.X = 0.5f;
        vertexes[0].UV.Y = 0.5f;

        vertexes[1].XYZ.X = (f32)width + 1.0f;
        vertexes[1].XYZ.Y = (f32)y;
        vertexes[1].XYZ.Z = zNear;
        vertexes[1].RHW = rhw;
        vertexes[1].Color = *RendererState.Color._Clear;
        vertexes[1].Specular = 0;
        vertexes[1].UV.X = 0.5f;
        vertexes[1].UV.Y = 0.5f;

        vertexes[2].XYZ.X = (f32)width + 1.0f;
        vertexes[2].XYZ.Y = (f32)height + 1.0f;
        vertexes[2].XYZ.Z = zNear;
        vertexes[2].RHW = rhw;
        vertexes[2].Color = *RendererState.Color._Clear;
        vertexes[2].Specular = 0;
        vertexes[2].UV.X = 0.5f;
        vertexes[2].UV.Y = 0.5f;

        vertexes[3].XYZ.X = (f32)x;
        vertexes[3].XYZ.Y = (f32)height + 1.0f;
        vertexes[3].XYZ.Z = zNear;
        vertexes[3].RHW = rhw;
        vertexes[3].Color = *RendererState.Color._Clear;
        vertexes[3].Specular = 0;
        vertexes[3].UV.X = 0.5f;
        vertexes[3].UV.Y = 0.5f;

        LoadTextureDescriptor126(0);

        const auto unk2 = *RendererState.Mode._UnknownMode2; // TODO names
        const auto unk3 = *RendererState.Mode._UnknownMode3; // TODO names

        *RendererState.Mode._UnknownMode2 = RendererMode::None;
        *RendererState.Mode._UnknownMode3 = RendererMode::U0x200 | RendererMode::U0x4;

        static u16 indexes[RENDERER_RECTANGLE_INDEX_COUNT] = { 0, 1, 2, 0, 2, 3 };

        FUN_00501ef0(vertexes, RENDERER_RECTANGLE_VERTEX_COUNT, indexes, RENDERER_RECTANGLE_TRIANGLE_COUNT);

        *RendererState.Mode._UnknownMode2 = unk2;
        *RendererState.Mode._UnknownMode3 = unk3;
    }
}