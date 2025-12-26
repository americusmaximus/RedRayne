#include "App.hxx"
#include "App.Options.hxx"
#include "App.Windows.hxx"
#include "Logger.hxx"
#include "Memory.hxx"
#include "Monitor.hxx"
#include "Renderer.hxx"
#include "Renderer.Module.hxx"
#include "Strings.hxx"
#include "UI.hxx"
#include "Mathematics.Basic.hxx"

#define MAX_CONFIGURATION_MESSAGE_LENGTH 256

#define OPTIONS_TITLE "BloodRayne Graphics Options"

#define OPTIONS_RESOLUTION_NAME "Resolution\t%dx%d"
#define OPTIONS_RESOLUTION_INDEX 0

#define OPTIONS_COLOR_DEPTH_NAME "Color Depth\t%d"
#define OPTIONS_COLOR_DEPTH_INDEX 1

#define OPTIONS_RENDERER_NAME "Renderer\t%s"
#define OPTIONS_RENDERER_INDEX 2

#define OPTIONS_GRAPHICS_CARD_NAME "Graphics Card\t%s"
#define OPTIONS_GRAPHICS_CARD_INDEX 3

#define OPTIONS_TEXTURE_DETAIL_NAME "Texture Detail\t%s"
#define OPTIONS_TEXTURE_DETAIL_INDEX 4

#define OPTIONS_BUMP_MAPPING_NAME "Bump Mapping\t%s"
#define OPTIONS_BUMP_MAPPING_INDEX 5

#define OPTIONS_DYNAMIC_SHADOWS_NAME "Dynamic Shadows\t%s"
#define OPTIONS_DYNAMIC_SHADOWS_INDEX 6

#define OPTIONS_WATER_REFLECTIONS_NAME "Water Reflections\t%s"
#define OPTIONS_WATER_REFLECTIONS_INDEX 7

#define OPTIONS_APPLY_DEFAULT_NAME "Apply Default Options"
#define OPTIONS_APPLY_DEFAULT_INDEX 8

#define OPTIONS_PLAY_GAME_NAME "Play BloodRayne"
#define OPTIONS_PLAY_GAME_INDEX 9

#define MAX_WATER_REFLECTION_OPTIONS_COUNT 3
#define MAX_OPTIONS_LABEL_LENGTH 32

#define OPTIONS_COMPLEX_SLOWEST_LABEL "Complex (Slowest)"
#define OPTIONS_HIGH_SLOWER_LABEL "High (Slower)"
#define OPTIONS_NORMAL_FASTER_LABEL "Normal (Faster)"
#define OPTIONS_OFF_FASTER_LABEL "Off (Faster)"
#define OPTIONS_OFF_FASTEST_LABEL "Off (Fastest)"
#define OPTIONS_ON_SLOWER_LABEL "On (Slower)"
#define OPTIONS_SIMPLE_SLOWER_LABEL "Simple (Slower)"

#define OPTIONS_GRAPHICS_CARD_NOT_FOUND "No Graphics Card Detected"

using namespace App::Options;
using namespace App::Windows;
using namespace Logger;
using namespace Mathematics;
using namespace Memory;
using namespace Monitor;
using namespace Renderer::Module;
using namespace Renderer;
using namespace Renderers;
using namespace Strings;
using namespace UI;
using namespace Mathematics;

namespace App
{
    ApplicationContainer AppState;

    const char WaterReflectionModeNames[MAX_WATER_REFLECTION_OPTIONS_COUNT][MAX_OPTIONS_LABEL_LENGTH] =
    {
        OPTIONS_OFF_FASTEST_LABEL,
        OPTIONS_SIMPLE_SLOWER_LABEL,
        OPTIONS_COMPLEX_SLOWEST_LABEL
    };

    typedef const void (CDECLAPI* FUN_0059DD40) (void);// TODO
    static FUN_0059DD40 FUN_0059dd40 = (FUN_0059DD40)0x0059dd40;// TODO

    typedef const s32(CDECLAPI* PICKLISTCONTROLACQUIREINDEX) (PickListControl* self, const char* title, const s32 indx, const s32 unk);// TODO
    static PICKLISTCONTROLACQUIREINDEX PickListControlAcquireIndex = (PICKLISTCONTROLACQUIREINDEX)0x00455300;// TODO

    void StartApplication(void)
    {
        InitializeRendererModules();

        if (RendererState.Modules.Count == 0)
        {
            LogError("Unable to acquire external graphics renderer(s). Software rendering is not supported.");
        }

        *RendererState.Options._IsUsingModule = TRUE;

        {
            const auto hdc = GetDC(*AppWindowState._HWND);

            if (hdc == NULL)
            {
                AppState._App->Options.Graphics.BitsPerPixel = GRAPHICS_BITS_PER_PIXEL_32;
            }
            else
            {
                AppState._App->Options.Graphics.BitsPerPixel = GetDeviceCaps(hdc, BITSPIXEL);

                ReleaseDC(*AppWindowState._HWND, hdc);
            }
        }

        AppState._App->Options.Graphics.Width =
            Clamp(AppState._App->Options.Graphics.Width, (u32)MIN_GRAPHICS_RESOLUTION_WIDTH, (u32)MAX_GRAPHICS_RESOLUTION_WIDTH);

        AppState._App->Options.Graphics.Height =
            Clamp(AppState._App->Options.Graphics.Height, (u32)MIN_GRAPHICS_RESOLUTION_HEIGHT, (u32)MAX_GRAPHICS_RESOLUTION_HEIGHT);


        ReleaseRenderer();

        RendererState.Options.SelectedDeviceIndex = 0;

        InitializeRendererModule2(NULL);

        RendererState.Devices.Count = 0;

        if (*RendererState.Options._IsUsingModule)
        {
            AcquireRendererGraphicsCards(&RendererState.Devices.Count,
                RendererState.Devices.APIs, RendererState.Devices.Names,
                RendererState.Devices.DeviceIDs, RendererState.Devices.VendorIDs);
        }

        s32 indx = OPTIONS_PLAY_GAME_INDEX;
        PickListControl control;
        char buffer[MAX_CONFIGURATION_MESSAGE_LENGTH];

        while (TRUE)
        {
            FUN_0059dd40();

            PickListControlInitialize(&control);

            sprintf(buffer, OPTIONS_RESOLUTION_NAME,
                AppState._App->Options.Graphics.Width, AppState._App->Options.Graphics.Height);
            StringListControlAttach((StringListControl*)&control, buffer);

            sprintf(buffer, OPTIONS_COLOR_DEPTH_NAME, AppState._App->Options.Graphics.BitsPerPixel);
            StringListControlAttach((StringListControl*)&control, buffer);

            sprintf(buffer, OPTIONS_RENDERER_NAME,
                (*RendererState.Options._IsUsingModule && IsNotNullOrEmpty(RendererState.Options.ModuleName))
                ? RendererState.Options.ModuleName : RendererState.Options._ModuleFileName);
            StringListControlAttach((StringListControl*)&control, buffer);

            sprintf(buffer, OPTIONS_GRAPHICS_CARD_NAME,
                *RendererState.Options._IsUsingModule
                ? RendererState.Devices.Names[RendererState.Options.SelectedDeviceIndex]
                : OPTIONS_GRAPHICS_CARD_NOT_FOUND);
            StringListControlAttach((StringListControl*)&control, buffer);

            sprintf(buffer, OPTIONS_TEXTURE_DETAIL_NAME,
                *AppOptionsState.Graphics._TextureDetailMode == ApplicationTextureDetailOptionsMode::Normal
                ? OPTIONS_NORMAL_FASTER_LABEL : OPTIONS_HIGH_SLOWER_LABEL);
            StringListControlAttach((StringListControl*)&control, buffer);

            sprintf(buffer, OPTIONS_BUMP_MAPPING_NAME,
                *AppOptionsState.Graphics._TextureBumpMappingMode == ApplicationTextureBumpMapOptionsMode::Off
                ? OPTIONS_OFF_FASTER_LABEL : OPTIONS_ON_SLOWER_LABEL);
            StringListControlAttach((StringListControl*)&control, buffer);

            sprintf(buffer, OPTIONS_DYNAMIC_SHADOWS_NAME,
                *AppOptionsState.Graphics._ShadowMode == ApplicationShadowOptionsMode::Off
                ? OPTIONS_OFF_FASTER_LABEL : OPTIONS_ON_SLOWER_LABEL);
            StringListControlAttach((StringListControl*)&control, buffer);

            sprintf(buffer, OPTIONS_WATER_REFLECTIONS_NAME,
                WaterReflectionModeNames[(u32)*AppOptionsState.Graphics._WaterReflectionMode]);
            StringListControlAttach((StringListControl*)&control, buffer);

            StringListControlAttach((StringListControl*)&control, OPTIONS_APPLY_DEFAULT_NAME);
            StringListControlAttach((StringListControl*)&control, OPTIONS_PLAY_GAME_NAME);
            
            indx = PickListControlAcquireIndex(&control, OPTIONS_TITLE, indx, 0);

            switch (indx)
            {
            case OPTIONS_RESOLUTION_INDEX:
            {
                AcquireDisplayOptions(*AppWindowState._HWND);

                if (DisplayState.Count == 0) { break; }

                for (u32 x = 0; x < DisplayState.Count; x++)
                {
                    const auto options = &DisplayState.Options[x];

                    if (options->Width == AppState._App->Options.Graphics.Width
                        && options->Height == AppState._App->Options.Graphics.Height
                        && options->BitsPerPixel == AppState._App->Options.Graphics.BitsPerPixel)
                    {
                        const auto actual = &DisplayState.Options[(x + 1) % DisplayState.Count];

                        AppState._App->Options.Graphics.Width = actual->Width;
                        AppState._App->Options.Graphics.Height = actual->Height;
                        AppState._App->Options.Graphics.BitsPerPixel = actual->BitsPerPixel;

                        break;
                    }
                }

                for (u32 x = 0; x < DisplayState.Count; x++)
                {
                    const auto options = &DisplayState.Options[x];

                    if (options->Width == AppState._App->Options.Graphics.Width
                        && options->Height == AppState._App->Options.Graphics.Height)
                    {
                        const auto actual = &DisplayState.Options[(x + 1) % DisplayState.Count];

                        AppState._App->Options.Graphics.Width = actual->Width;
                        AppState._App->Options.Graphics.Height = actual->Height;
                        AppState._App->Options.Graphics.BitsPerPixel = actual->BitsPerPixel;

                        break;
                    }
                }

                break;
            }
            case OPTIONS_COLOR_DEPTH_INDEX:
            {
                AcquireDisplayOptions(*AppWindowState._HWND);

                if (DisplayState.Count == 0) { break; }

                for (u32 x = 0; x < DisplayState.Count; x++)
                {
                    const auto options = &DisplayState.Options[x];

                    if (options->Width == AppState._App->Options.Graphics.Width
                        && options->Height == AppState._App->Options.Graphics.Height
                        && options->BitsPerPixel != AppState._App->Options.Graphics.BitsPerPixel)
                    {
                        AppState._App->Options.Graphics.BitsPerPixel = options->BitsPerPixel;

                        break;
                    }
                }

                break;
            }
            case OPTIONS_RENDERER_INDEX:
            {
                for (u32 x = 0; x < RendererState.Modules.Count; x++)
                {
                    if (EqualStrings(RendererState.Options._ModuleFileName, RendererState.Modules.Names[x]))
                    {
                        *RendererState.Options._IsUsingModule = TRUE;

                        ReleaseRenderer();

                        strcpy(RendererState.Options._ModuleFileName, RendererState.Modules.Names[(x + 1) % RendererState.Modules.Count]);

                        InitializeRendererModule2(NULL);

                        RendererState.Devices.Count = 0;

                        if (*RendererState.Options._IsUsingModule)
                        {
                            AcquireRendererGraphicsCards(&RendererState.Devices.Count,
                                RendererState.Devices.APIs, RendererState.Devices.Names,
                                RendererState.Devices.DeviceIDs, RendererState.Devices.VendorIDs);
                        }

                        break;
                    }
                }

                break;
            }
            case OPTIONS_GRAPHICS_CARD_INDEX:
            {
                if (RendererState.Devices.Count != 0)
                {
                    RendererState.Options.SelectedDeviceIndex =
                        (RendererState.Options.SelectedDeviceIndex + 1) % RendererState.Devices.Count;
                }

                break;
            }
            case OPTIONS_TEXTURE_DETAIL_INDEX:
            {
                switch (*AppOptionsState.Graphics._TextureDetailMode)
                {
                case ApplicationTextureDetailOptionsMode::Normal:
                {
                    *AppOptionsState.Graphics._TextureDetailMode = ApplicationTextureDetailOptionsMode::High;

                    break;
                }
                case ApplicationTextureDetailOptionsMode::High:
                {
                    *AppOptionsState.Graphics._TextureDetailMode = ApplicationTextureDetailOptionsMode::Normal;

                    break;
                }
                }

                break;
            }
            case OPTIONS_BUMP_MAPPING_INDEX:
            {
                switch (*AppOptionsState.Graphics._TextureBumpMappingMode)
                {
                case ApplicationTextureBumpMapOptionsMode::On:
                {
                    *AppOptionsState.Graphics._TextureBumpMappingMode = ApplicationTextureBumpMapOptionsMode::Off;

                    break;
                }
                case ApplicationTextureBumpMapOptionsMode::Off:
                {
                    *AppOptionsState.Graphics._TextureBumpMappingMode = ApplicationTextureBumpMapOptionsMode::On;

                    break;
                }
                }

                break;
            }
            case OPTIONS_DYNAMIC_SHADOWS_INDEX:
            {
                switch (*AppOptionsState.Graphics._ShadowMode)
                {
                case ApplicationShadowOptionsMode::On:
                {
                    *AppOptionsState.Graphics._ShadowMode = ApplicationShadowOptionsMode::Off;

                    break;
                }
                case ApplicationShadowOptionsMode::Off:
                {
                    *AppOptionsState.Graphics._ShadowMode = ApplicationShadowOptionsMode::On;

                    break;
                }
                }

                break;
            }
            case OPTIONS_WATER_REFLECTIONS_INDEX:
            {
                switch (*AppOptionsState.Graphics._WaterReflectionMode)
                {
                case ApplicationWaterReflectionOptionsMode::Off:
                {
                    *AppOptionsState.Graphics._WaterReflectionMode = ApplicationWaterReflectionOptionsMode::Simple;

                    break;
                }
                case ApplicationWaterReflectionOptionsMode::Simple:
                {
                    *AppOptionsState.Graphics._WaterReflectionMode = ApplicationWaterReflectionOptionsMode::Complex;

                    break;
                }
                case ApplicationWaterReflectionOptionsMode::Complex:
                {
                    *AppOptionsState.Graphics._WaterReflectionMode = ApplicationWaterReflectionOptionsMode::Off;

                    break;
                }
                }

                break;
            }
            case OPTIONS_APPLY_DEFAULT_INDEX:
            {
                *RendererState.Options._AcceleratedGraphicsPort = RendererAcceleratedGraphicsPortMode::X2;

                AppState._App->Options.Graphics.Width = MIN_GRAPHICS_RESOLUTION_WIDTH;
                AppState._App->Options.Graphics.Height = MIN_GRAPHICS_RESOLUTION_HEIGHT;
                AppState._App->Options.Graphics.BitsPerPixel = GRAPHICS_BITS_PER_PIXEL_32;

                *RendererState.Options._IsUsingModule = TRUE;

                ReleaseRenderer();

                RendererState.Options.SelectedDeviceIndex = 0;

                InitializeRendererModule2(NULL);

                RendererState.Devices.Count = 0;

                if (*RendererState.Options._IsUsingModule)
                {
                    AcquireRendererGraphicsCards(&RendererState.Devices.Count,
                        RendererState.Devices.APIs, RendererState.Devices.Names,
                        RendererState.Devices.DeviceIDs, RendererState.Devices.VendorIDs);
                }

                *AppOptionsState.Graphics._TextureDetailMode = ApplicationTextureDetailOptionsMode::Normal;
                *AppOptionsState.Graphics._ShadowMode = ApplicationShadowOptionsMode::Off;
                *AppOptionsState.Graphics._WaterReflectionMode = ApplicationWaterReflectionOptionsMode::Off;
                *AppOptionsState.Graphics._TextureBumpMappingMode = ApplicationTextureBumpMapOptionsMode::Off;

                break;
            }
            case OPTIONS_PLAY_GAME_INDEX:
            {
                if (!*RendererState.Options._IsUsingModule)
                {
                    if (MessageBoxA(*AppWindowState._HWND,
                        "BloodRayne requires 3D hardware acceleration.\nPlease select valid hardware renderer.",
                        AppWindowState.WindowName, MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL)
                    {
                        return;
                    }

                    break;
                }

                SelectRendererGraphicsCard(RendererState.Options.SelectedDeviceIndex);

                //*AppOptionsState.Graphics._ScreenMode =
                //    ((f32)AppState._App->Options.Graphics.Width / (f32)AppState._App->Options.Graphics.Height) < 1.5f
                //    ? ApplicationWideScreenMode::Off : ApplicationWideScreenMode::On;

                *RendererWindow._WindowMode = RendererWindowMode::Off;

                SelectAppWindowResolution2(AppState._App->Options.Graphics.Width, AppState._App->Options.Graphics.Height, AppState._App->Options.Graphics.BitsPerPixel);

                SelectRendererVideModeResetTextureCache();

                InitializeApplication1();

                RestoreRendererVideoMode2(TRUE);

                PickListControlDispose(&control, DisposeMode::None);

                return;
            }
            default:
            {
                PickListControlDispose(&control, DisposeMode::None);

                return;
            }
            }

            PickListControlDispose(&control, DisposeMode::None);
        }
    }


}