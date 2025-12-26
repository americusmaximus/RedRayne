#include "App.Options.hxx"
#include "Logger.hxx"
#include "IO.hxx"
#include "IO.Ini.hxx"
#include "Renderer.hxx"
#include "Renderer.Module.hxx"
#include "Sounds.Devices.hxx"
#include "Sounds.hxx"
#include "Strings.hxx"
#include "Memory.hxx"
#include "Input.Controllers.hxx"
#include "Console.hxx"
#include "App.hxx"
#include "Input.hxx"

#include <stdio.h>

#define OPTIONS_INI_NAME ".\\system\\rayne.ini"

#define OPTIONS_INI_GRAPHICS_SECTION_NAME "Graphics"
#define OPTIONS_INI_GRAPHICS_WIDTH_PROPERTY_NAME "gamePIXX"
#define OPTIONS_INI_GRAPHICS_HEIGHT_PROPERTY_NAME "gamePIXY"
#define OPTIONS_INI_GRAPHICS_BITS_PER_PIXEL_PROPERTY_NAME "gameBPP"
#define OPTIONS_INI_GRAPHICS_IS_USING_MODULE_PROPERTY_NAME "useDirect3D"
#define OPTIONS_INI_GRAPHICS_ACCELERATED_GRAPHICS_PORT_PROPERTY_NAME "useAGPFlag"
#define OPTIONS_INI_GRAPHICS_RENDERER_MODULE_NAME_PROPERTY_NAME "rendererDLLPath"
#define OPTIONS_INI_GRAPHICS_IS_FIRST_TIME_PROPERTY_NAME "firstTimeFlag"
#define OPTIONS_INI_GRAPHICS_SUBTITLES_PROPERTY_NAME "subtitleMode"
#define OPTIONS_INI_GRAPHICS_BLOOD_PROPERTY_NAME "bloodFlag"
#define OPTIONS_INI_GRAPHICS_SHADOWS_PROPERTY_NAME "shadowFlag"
#define OPTIONS_INI_GRAPHICS_QUIMBY_PROPERTY_NAME "quimbyFlag"
#define OPTIONS_INI_GRAPHICS_SELECTED_DEVICE_INDEX_PROPERTY_NAME "currentBoard"
#define OPTIONS_INI_GRAPHICS_BRIGHTNESS_PROPERTY_NAME "gameBrightness"
#define OPTIONS_INI_GRAPHICS_SHADOW_MODE_PROPERTY_NAME "gameRenderShadows"
#define OPTIONS_INI_GRAPHICS_SCREEN_MODE_PROPERTY_NAME "gameWideScreenFlag"
#define OPTIONS_INI_GRAPHICS_BUMP_MODE_PROPERTY_NAME "bumpMapFlag"
#define OPTIONS_INI_GRAPHICS_TEXTURE_DETAIL_MODE_PROPERTY_NAME "textureDetail"
#define OPTIONS_INI_GRAPHICS_REFLECTION_MODE_PROPERTY_NAME "reflectionFlag"

#define OPTIONS_INI_SOUND_SECTION_NAME "Sound"
#define OPTIONS_INI_SOUND_DEVICE_NAME_PROPERTY_NAME "DeviceName"
#define OPTIONS_INI_SOUND_RECORDING_DEVICE_NAME_PROPERTY_NAME "RecordingDeviceName"
#define OPTIONS_INI_SOUND_IS_ACCELERATION_ACTIVE_PROPERTY_NAME "AccelEnabled"
#define OPTIONS_INI_SOUND_MAXIMUM_SOFTWARE_LATENCY_PROPERTY_NAME "MaxSwLatency"
#define OPTIONS_INI_SOUND_BITS_PROPERTY_NAME "Bits"
#define OPTIONS_INI_SOUND_CHANNELS_PROPERTY_NAME "Channels"
#define OPTIONS_INI_SOUND_HZ_PROPERTY_NAME "Hz"
#define OPTIONS_INI_SOUND_MUTE_PROPERTY_NAME "Mute"
#define OPTIONS_INI_SOUND_SFX_CHANNEL_ACTIVE_PROPERTY_NAME "SfxChannel%dEnabled"
#define OPTIONS_INI_SOUND_SFX_CHANNEL_VOLUME_PROPERTY_NAME "SfxChannel%dVol"

#define OPTIONS_INI_CONTROL_SECTION_NAME "Control"
#define OPTIONS_INI_CONTROL_IS_ACTIVE_PROPERTY_NAME "gameControl"
#define OPTIONS_INI_CONTROL_MAIN_BUTTON_PROPERTY_NAME "keyButton%d"
#define OPTIONS_INI_CONTROL_ALTERNATIVE_BUTTON_PROPERTY_NAME "alternateKeyButton%d"
#define OPTIONS_INI_CONTROL_CONTROLLER_BUTTON_PROPERTY_NAME "gamepadButton%d"
#define OPTIONS_INI_CONTROL_MOUSE_SENSITIVITY_X_PROPERTY_NAME "mouseSensitivityX"
#define OPTIONS_INI_CONTROL_MOUSE_SENSITIVITY_Y_PROPERTY_NAME "mouseSensitivityY"
#define OPTIONS_INI_CONTROL_AUTO_CENTER_PITCH_PROPERTY_NAME "autoCenterPitch"
#define OPTIONS_INI_CONTROL_GAMEPAD_AXIS_PROPERTY_NAME "gamepadAxis%d"
#define OPTIONS_INI_CONTROL_GAMEPAD_INVERT_AXIS_PROPERTY_NAME "invertAxis%d"
#define OPTIONS_INI_CONTROL_CONTROLLER_TYPE_PROPERTY_NAME "Controller_%d_type"
#define OPTIONS_INI_CONTROL_CONTROLLER_PORT_PROPERTY_NAME "Controller_%d_pc_port"
#define OPTIONS_INI_CONTROL_CONTROLLER_AXIS_MIN_PROPERTY_NAME "Controller_%d_axis_%d_min"
#define OPTIONS_INI_CONTROL_CONTROLLER_AXIS_MAX_PROPERTY_NAME "Controller_%d_axis_%d_max"
#define OPTIONS_INI_CONTROL_CONTROLLER_AXIS_CENTER_PROPERTY_NAME "Controller_%d_axis_%d_center"

#define OPTIONS_INI_DEBUG_SECTION_NAME "Debug"
#define OPTIONS_INI_DEBUG_CONSOLE_ACTIVE_PROPERTY_NAME "logConsoleFlag"

#define OPTIONS_INI_EDITOR_SECTION_NAME "Editor"
#define OPTIONS_INI_GAME_SECTION_NAME "game"
#define OPTIONS_INI_GAME_LANGUAGE_PROPERTY_NAME "language"
#define OPTIONS_INI_GAME_LANGUAGE_PROPERTY_NAME_LENGTH 32

using namespace IO;
using namespace Logger;
using namespace IO::Ini;
using namespace Sounds;
using namespace Strings;
using namespace Input::Controllers;
using namespace Console;
using namespace Memory;
using namespace Renderer;

namespace App::Options
{
    ApplicationOptionsContainer AppOptionsState;

    ApplicationLanguage AcquireAppLanguage(void)
    {
        return AppOptionsState.Language;
    }

    const char* AcquireAppLanguageCode(const ApplicationLanguage language)
    {
        switch (language)
        {
        case ApplicationLanguage::English: { return "EN"; }
        case ApplicationLanguage::French: { return "FR"; }
        case ApplicationLanguage::Italian: { return "IT"; }
        case ApplicationLanguage::Spanish: { return "ES"; }
        case ApplicationLanguage::German: { return "DE"; }
        case ApplicationLanguage::Japanese: { return "JA"; }
        }

        return "???";
    }

    void SelectAppLanguage(const ApplicationLanguage language)
    {
        AppOptionsState.Language = language;
    }

    void LoadAppSoundOptions(IniFile* ini)
    {
        char buffer[MAX_SOUND_DEVICE_NAME_LENGTH];

        ini->Self->ReadString(ini, OPTIONS_INI_SOUND_DEVICE_NAME_PROPERTY_NAME, buffer, MAX_SOUND_DEVICE_NAME_LENGTH);

        StopSound2();

        for (s32 x = 0; x < AcquireSoundDeviceCount(); x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (!EqualStrings(device.Name, buffer)) { continue; }

            SelectSoundDevice2(x);
        }

        {
            auto value = AcquireSoundMixMode2();
            AbstractIniReadInteger((AbstractIni*)ini, OPTIONS_INI_SOUND_IS_ACCELERATION_ACTIVE_PROPERTY_NAME, (s32*)&value);
            SelectSoundMixMode(value);
        }

        {
            auto value = AcquireMaximumSoftWareSoundLatency();
            AbstractIniReadReal((AbstractIni*)ini, OPTIONS_INI_SOUND_MAXIMUM_SOFTWARE_LATENCY_PROPERTY_NAME, &value);
            SelectMaximumSoftWareSoundLatency(value);
        }

        {
            u32 bits = 0;
            u32 channels = 0;
            u32 hz = 0;

            AcquireSoundOutputOptions(&bits, &channels, &bits);
            AbstractIniReadInteger((AbstractIni*)ini, OPTIONS_INI_SOUND_BITS_PROPERTY_NAME, (s32*)&bits);
            AbstractIniReadInteger((AbstractIni*)ini, OPTIONS_INI_SOUND_CHANNELS_PROPERTY_NAME, (s32*)&channels);
            AbstractIniReadInteger((AbstractIni*)ini, OPTIONS_INI_SOUND_HZ_PROPERTY_NAME, (s32*)&hz);
            SelectSoundOutputOptions(bits, channels, hz);
        }

        {
            auto value = AcquireSoundMuteMode();
            AbstractIniReadInteger((AbstractIni*)ini, OPTIONS_INI_SOUND_MUTE_PROPERTY_NAME, (s32*)&value);
            SelectSoundMuteMode(value);
        }

        for (u32 x = 0; x < SoundState.SFX.ChannelCount; x++)
        {
            sprintf(buffer, OPTIONS_INI_SOUND_SFX_CHANNEL_ACTIVE_PROPERTY_NAME, x);

            auto active = AcquireSoundEffectChannelState(x);
            AbstractIniReadInteger((AbstractIni*)ini, buffer, &active);
            SelectSoundEffectChannelState(x, active);

            sprintf(buffer, OPTIONS_INI_SOUND_SFX_CHANNEL_VOLUME_PROPERTY_NAME, x);

            auto volume = AcquireSoundEffectChannelVolume(x);
            AbstractIniReadReal((AbstractIni*)ini, buffer, &volume);
            SelectSoundEffectChannelVolume1(x, volume);
        }

        IniFileReadString(ini, OPTIONS_INI_SOUND_RECORDING_DEVICE_NAME_PROPERTY_NAME, buffer, MAX_SOUND_DEVICE_NAME_LENGTH);

        StopSoundRecording1();

        for (u32 x = 0; x <= AcquireSoundRecordingDeviceCount(); x++)
        {
            SoundRecordingDevice device;
            AcquireSoundRecordingDevice(x, &device);

            if (!EqualStrings(device.Name, buffer)) { continue; }

            SelectSoundRecordingDevice2(x);
        }
    }

    void InitializeAppKeyOptions(ApplicationOptions* options)
    {
        if (TRUE < options->IsControllerActive)
        {
            options->IsControllerActive = TRUE;
        }

        for (u32 x = 0; x < MAX_APPLICATION_OPTIONS_KEY_COUNT; x++)
        {
            options->Keys.Primary[x] = -1;
            options->Keys.Secondary[x] = -1;
            options->Controller.Keys[x] = -1;
        }

        options->Keys.Primary[(u32)ApplicationOptionsInput::BladeAttack] = INPUT_KEY_LEFT_MOUSE_BUTTON;
        options->Keys.Primary[(u32)ApplicationOptionsInput::WeaponAttack] = INPUT_KEY_RIGHT_MOUSE_BUTTON;
        options->Keys.Primary[(u32)ApplicationOptionsInput::CenterView] = INPUT_KEY_END;
        options->Keys.Primary[(u32)ApplicationOptionsInput::Jump] = INPUT_KEY_SPACE;
        options->Keys.Primary[(u32)ApplicationOptionsInput::HarpponBite] = INPUT_KEY_E;
        options->Keys.Primary[(u32)ApplicationOptionsInput::QuickTurn] = INPUT_KEY_LEFT_ALT;
        options->Keys.Primary[(u32)ApplicationOptionsInput::BloodRage] = INPUT_KEY_CAPS_LOCK;
        options->Keys.Primary[(u32)ApplicationOptionsInput::NormalVision] = INPUT_KEY_1;
        options->Keys.Primary[(u32)ApplicationOptionsInput::AuraVision] = INPUT_KEY_3;
        options->Keys.Primary[(u32)ApplicationOptionsInput::ExtrudedVision] = INPUT_KEY_2;
        options->Keys.Primary[(u32)ApplicationOptionsInput::DilatedVision] = INPUT_KEY_4;
        options->Keys.Primary[(u32)ApplicationOptionsInput::WeaponModeForward] = INPUT_KEY_F;
        options->Keys.Primary[(u32)ApplicationOptionsInput::WeaponModeReverse] = INPUT_KEY_R;
        options->Keys.Primary[(u32)ApplicationOptionsInput::ScreenShot] = INPUT_KEY_F11;
        options->Keys.Primary[(u32)ApplicationOptionsInput::Debug] = INPUT_KEY_8;
        options->Keys.Primary[(u32)ApplicationOptionsInput::Pause] = INPUT_KEY_ESC;
        options->Keys.Primary[(u32)ApplicationOptionsInput::MoveForward] = INPUT_KEY_W;
        options->Keys.Primary[(u32)ApplicationOptionsInput::MoveBackward] = INPUT_KEY_S;
        options->Keys.Primary[(u32)ApplicationOptionsInput::MoveLeft] = INPUT_KEY_A;
        options->Keys.Primary[(u32)ApplicationOptionsInput::MoveRight] = INPUT_KEY_D;

        options->Keys.Secondary[(u32)ApplicationOptionsInput::BladeAttack] = DIK_NUMPAD0;
        options->Keys.Secondary[(u32)ApplicationOptionsInput::WeaponAttack] = DIK_RETURN;
        options->Keys.Secondary[(u32)ApplicationOptionsInput::NormalVision] = DIK_NUMPAD1;
        options->Keys.Secondary[(u32)ApplicationOptionsInput::AuraVision] = DIK_NUMPAD2;
        options->Keys.Secondary[(u32)ApplicationOptionsInput::ExtrudedVision] = DIK_NUMPAD3;
        options->Keys.Secondary[(u32)ApplicationOptionsInput::DilatedVision] = DIK_NUMPAD4;
        options->Keys.Secondary[(u32)ApplicationOptionsInput::WeaponModeForward] = DIK_RBRACKET;
        options->Keys.Secondary[(u32)ApplicationOptionsInput::WeaponModeReverse] = DIK_LBRACKET;
        options->Keys.Secondary[(u32)ApplicationOptionsInput::LookUp] = INPUT_KEY_PAGE_DOWN;
        options->Keys.Secondary[(u32)ApplicationOptionsInput::LookDown] = INPUT_KEY_DEL;
        options->Keys.Secondary[(u32)ApplicationOptionsInput::LookLeft] = INPUT_KEY_LEFT_ARROW;
        options->Keys.Secondary[(u32)ApplicationOptionsInput::LookRight] = INPUT_KEY_RIGHT_ARROW;

        // TODO DIJOFS_BUTTON0 ?
        options->Controller.Keys[(u32)ApplicationOptionsInput::BladeAttack] = 6;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::WeaponAttack] = 7;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::CenterView] = 9;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::Jump] = 3;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::HarpponBite] = 0;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::QuickTurn] = 4;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::BloodRage] = 1;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::NormalVision] = 0x1e;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::AuraVision] = 0x1c;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::ExtrudedVision] = 0x1d;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::DilatedVision] = 0x1f;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::WeaponModeForward] = 2;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::WeaponModeReverse] = 5;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::ScreenShot] = 8;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::Debug] = 0xb;//TODO
        options->Controller.Keys[(u32)ApplicationOptionsInput::Pause] = 10;//TODO

        options->Controller.Axes[0] = 1;
        options->Controller.Axes[1] = 0;
        options->Controller.Axes[2] = 2;
        options->Controller.Axes[3] = 3;

        options->Controller.AxesInversion[0] = FALSE;
        options->Controller.AxesInversion[1] = FALSE;
        options->Controller.AxesInversion[2] = FALSE;
        options->Controller.AxesInversion[3] = TRUE;

        options->AutoCenterPitch = TRUE;

        options->MouseSensitivityX = 1.0f;
        options->MouseSensitivityY = 1.0f;
    }

    void LoadAppControllerOptions(IniFile* ini)
    {
        char buffer[MAX_INI_SECTION_LENGTH];

        for (u32 x = 0; x < MAX_CONTROLLER_COUNT; x++)
        {
            s32 type = 0;
            sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_TYPE_PROPERTY_NAME, x);
            AbstractIniReadInteger((AbstractIni*)ini, buffer, &type);

            s32 port = 0;
            sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_PORT_PROPERTY_NAME, x);
            AbstractIniReadInteger((AbstractIni*)ini, buffer, &port);

            InitializeController2(x, type, port);

            auto controller = AppOptionsState._Controllers[x];

            for (u32 xx = 0; xx < MAX_APPLICATION_OPTIONS_CONTROLLER_AXIS_OPTIONS_COUNT; xx++)
            {
                sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_AXIS_MIN_PROPERTY_NAME, x, xx);
                AbstractIniReadInteger((AbstractIni*)ini, buffer, &controller.Axes[x].Minimum);

                sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_AXIS_MAX_PROPERTY_NAME, x, xx);
                AbstractIniReadInteger((AbstractIni*)ini, buffer, &controller.Axes[x].Maximum);

                sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_AXIS_CENTER_PROPERTY_NAME, x, xx);
                AbstractIniReadInteger((AbstractIni*)ini, buffer, &controller.Axes[x].Center);
            }
        }
    }

    void SelectAppLanguageCode(const char* code)
    {
        for (u32 x = 0; x < MAX_APPLICATION_LANGUAGE_COUNT; x++)
        {
            const auto language = (ApplicationLanguage)x;

            if (EqualStrings(code, AcquireAppLanguageCode(language)))
            {
                SelectAppLanguage(language);

                return;
            }
        }
    }

    void LoadAppOptions(void)
    {
        LogMessage("[INI] Reading .INI file.\n");

        if (!CloseCacheFile2(OPTIONS_INI_NAME, 0))
        {
            LogError("Please install BloodRayne to your hard drive.");
        }

        IniFile ini;

        IniFileInitialize(&ini, OPTIONS_INI_NAME, NULL);

        AbstractIniSelectSection((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SECTION_NAME);
        
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_WIDTH_PROPERTY_NAME, (s32*)&AppState._App->Options.Graphics.Width);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_HEIGHT_PROPERTY_NAME, (s32*)&AppState._App->Options.Graphics.Height);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_BITS_PER_PIXEL_PROPERTY_NAME, (s32*)&AppState._App->Options.Graphics.BitsPerPixel);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_IS_USING_MODULE_PROPERTY_NAME, RendererState.Options._IsUsingModule);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_ACCELERATED_GRAPHICS_PORT_PROPERTY_NAME, (s32*)RendererState.Options._AcceleratedGraphicsPort);
        IniFileReadString(&ini, OPTIONS_INI_GRAPHICS_RENDERER_MODULE_NAME_PROPERTY_NAME, RendererState.Options._ModuleFileName, MAX_RENDERER_MODULE_FILE_NAME_LENGTH);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_IS_FIRST_TIME_PROPERTY_NAME, (s32*)AppOptionsState._FirstTimeMode);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SUBTITLES_PROPERTY_NAME, (s32*)&AppState._App->Options.Graphics.Subtitles);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_BLOOD_PROPERTY_NAME, (s32*)&AppState._App->Options.Graphics.Blood);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SHADOWS_PROPERTY_NAME, (s32*)&AppState._App->Options.Graphics.Shadows);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_QUIMBY_PROPERTY_NAME, (s32*)&AppState._App->Options.Graphics.Quimby);

        RendererState.Options.SelectedDeviceIndex = 0;

        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SELECTED_DEVICE_INDEX_PROPERTY_NAME, (s32*)&RendererState.Options.SelectedDeviceIndex);
        AbstractIniReadReal((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_BRIGHTNESS_PROPERTY_NAME, AppOptionsState.Graphics._Brightness);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SHADOW_MODE_PROPERTY_NAME, (s32*)AppOptionsState.Graphics._ShadowMode);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SCREEN_MODE_PROPERTY_NAME, (s32*)AppOptionsState.Graphics._ScreenMode);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_BUMP_MODE_PROPERTY_NAME, (s32*)AppOptionsState.Graphics._TextureBumpMappingMode);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_TEXTURE_DETAIL_MODE_PROPERTY_NAME, (s32*)AppOptionsState.Graphics._TextureDetailMode);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_REFLECTION_MODE_PROPERTY_NAME, (s32*)AppOptionsState.Graphics._WaterReflectionMode);

        AbstractIniSelectSection((AbstractIni*)&ini, OPTIONS_INI_SOUND_SECTION_NAME);
        
        LoadAppSoundOptions(&ini);

        AbstractIniSelectSection((AbstractIni*)&ini, OPTIONS_INI_CONTROL_SECTION_NAME);
        
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_CONTROL_IS_ACTIVE_PROPERTY_NAME, &AppState._App->Options.IsControllerActive);

        InitializeAppKeyOptions(&AppState._App->Options);

        for (u32 x = 0; x < MAX_APPLICATION_OPTIONS_KEY_COUNT; x++)
        {
            char buffer[MAX_INI_SECTION_LENGTH];

            sprintf(buffer, OPTIONS_INI_CONTROL_MAIN_BUTTON_PROPERTY_NAME, x);
            AbstractIniReadInteger((AbstractIni*)&ini, buffer, &AppState._App->Options.Keys.Primary[x]);
            
            sprintf(buffer, OPTIONS_INI_CONTROL_ALTERNATIVE_BUTTON_PROPERTY_NAME, x);
            AbstractIniReadInteger((AbstractIni*)&ini, buffer, &AppState._App->Options.Keys.Secondary[x]);
            
            sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_BUTTON_PROPERTY_NAME, x);
            AbstractIniReadInteger((AbstractIni*)&ini, buffer, &AppState._App->Options.Controller.Keys[x]);
        }

        AbstractIniReadReal((AbstractIni*)&ini, OPTIONS_INI_CONTROL_MOUSE_SENSITIVITY_X_PROPERTY_NAME, &AppState._App->Options.MouseSensitivityX);
        AbstractIniReadReal((AbstractIni*)&ini, OPTIONS_INI_CONTROL_MOUSE_SENSITIVITY_Y_PROPERTY_NAME, &AppState._App->Options.MouseSensitivityY);
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_CONTROL_AUTO_CENTER_PITCH_PROPERTY_NAME, &AppState._App->Options.AutoCenterPitch);
        
        for (u32 x = 0; x < MAX_APPLICATION_OPTIONS_CONTROLLER_AXIS_COUNT; x++)
        {
            char buffer[MAX_INI_SECTION_LENGTH];

            sprintf(buffer, OPTIONS_INI_CONTROL_GAMEPAD_AXIS_PROPERTY_NAME, x);
            AbstractIniReadInteger((AbstractIni*)&ini, buffer, &AppState._App->Options.Controller.Axes[x]);

            sprintf(buffer, OPTIONS_INI_CONTROL_GAMEPAD_INVERT_AXIS_PROPERTY_NAME, x);
            AbstractIniReadInteger((AbstractIni*)&ini, buffer, &AppState._App->Options.Controller.AxesInversion[x]);
        }

        LoadAppControllerOptions(&ini);
        InitializeControllersOptions(&AppState._App->Options);

        AbstractIniSelectSection((AbstractIni*)&ini, OPTIONS_INI_DEBUG_SECTION_NAME);
        
        AbstractIniReadInteger((AbstractIni*)&ini, OPTIONS_INI_DEBUG_CONSOLE_ACTIVE_PROPERTY_NAME, &(*ConsoleState._ConsolePointer)->IsActive);

        AbstractIniSelectSection((AbstractIni*)&ini, OPTIONS_INI_EDITOR_SECTION_NAME);
        AbstractIniSelectSection((AbstractIni*)&ini, OPTIONS_INI_GAME_SECTION_NAME);

        char language[OPTIONS_INI_GAME_LANGUAGE_PROPERTY_NAME_LENGTH];
        IniFileReadString(&ini, OPTIONS_INI_GAME_LANGUAGE_PROPERTY_NAME, language, OPTIONS_INI_GAME_LANGUAGE_PROPERTY_NAME_LENGTH);
        SelectAppLanguageCode(language);

        IniFileDispose(&ini, DisposeMode::None);
    }

    void InitializeControllersOptions(ApplicationOptions* options)
    {
        if (!options->IsControllerActive)
        {
            InitializeController1(0);

            return;
        }

        if (ControllerState._Controllers[0].Type != 2 || ControllerState._Controllers[0].IsActive == 0)
        {
            InitializeController2(0, 2, 0);
        }
    }

    void SaveAppSoundOptions(IniFile* ini)
    {
        char buffer[MAX_SOUND_DEVICE_NAME_LENGTH];

        if (-1 < SoundDeviceState.SoundDeviceIndex && SoundDeviceState.SoundDeviceIndex < AcquireSoundDeviceCount())
        {
            SoundDevice device;

            AcquireSoundDevice(SoundDeviceState.SoundDeviceIndex, &device);

            strcpy(buffer, device.Name);
        }

        IniFileWriteString(ini, OPTIONS_INI_SOUND_DEVICE_NAME_PROPERTY_NAME, buffer);

        u32 bits = 0;
        u32 channels = 0;
        u32 hz = 0;

        AcquireSoundOutputOptions(&bits, &channels, &hz);
        AbstractIniWriteInteger((AbstractIni*)ini, OPTIONS_INI_SOUND_BITS_PROPERTY_NAME, bits);
        AbstractIniWriteInteger((AbstractIni*)ini, OPTIONS_INI_SOUND_CHANNELS_PROPERTY_NAME, channels);
        AbstractIniWriteInteger((AbstractIni*)ini, OPTIONS_INI_SOUND_HZ_PROPERTY_NAME, hz);

        AbstractIniWriteInteger((AbstractIni*)ini, OPTIONS_INI_SOUND_IS_ACCELERATION_ACTIVE_PROPERTY_NAME, (s32)AcquireSoundMixMode2());

        AbstractIniWriteReal((AbstractIni*)ini, OPTIONS_INI_SOUND_MAXIMUM_SOFTWARE_LATENCY_PROPERTY_NAME, AcquireMaximumSoftWareSoundLatency());

        AbstractIniWriteInteger((AbstractIni*)ini, OPTIONS_INI_SOUND_MUTE_PROPERTY_NAME, AcquireSoundMuteMode());

        for (u32 x = 0; x < SoundState.SFX.ChannelCount; x++)
        {
            sprintf(buffer, OPTIONS_INI_SOUND_SFX_CHANNEL_ACTIVE_PROPERTY_NAME, x);
            const auto active = AcquireSoundEffectChannelState(x);
            AbstractIniWriteInteger((AbstractIni*)ini, buffer, active);

            sprintf(buffer, OPTIONS_INI_SOUND_SFX_CHANNEL_VOLUME_PROPERTY_NAME, x);
            const auto volume = AcquireSoundEffectChannelVolume(x);
            AbstractIniWriteReal((AbstractIni*)ini, buffer, volume);
        }
    }

    void SaveAppControllerOptions(IniFile* ini)
    {
        char buffer[MAX_INI_SECTION_LENGTH];

        for (u32 x = 0; x < MAX_CONTROLLER_COUNT; x++)
        {
            sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_TYPE_PROPERTY_NAME, x);
            AbstractIniWriteInteger((AbstractIni*)ini, buffer, ControllerState._Controllers[x].Type);

            if (ControllerState._Controllers[x].Type != 0)
            {
                sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_PORT_PROPERTY_NAME, x);
                AbstractIniWriteInteger((AbstractIni*)ini, buffer, ControllerState._ControllerPorts[x]);

                const auto controller = &AppOptionsState._Controllers[x];

                for (u32 xx = 0; xx < MAX_APPLICATION_OPTIONS_CONTROLLER_AXIS_COUNT; xx++)
                {
                    sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_AXIS_MIN_PROPERTY_NAME, x, xx);
                    AbstractIniWriteInteger((AbstractIni*)ini, buffer, controller->Axes[x].Minimum);

                    sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_AXIS_MAX_PROPERTY_NAME, x, xx);
                    AbstractIniWriteInteger((AbstractIni*)ini, buffer, controller->Axes[x].Maximum);

                    sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_AXIS_CENTER_PROPERTY_NAME, x, xx);
                    AbstractIniWriteInteger((AbstractIni*)ini, buffer, controller->Axes[x].Center);
                }
            }
        }
    }

    void SaveAppOptions(void)
    {
        LogMessage("[INI] Writing .INI file.\n");

        IniFile ini;

        IniFileInitialize(&ini, OPTIONS_INI_NAME, NULL);

        AbstractIniSelectSection((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SECTION_NAME);
        
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_WIDTH_PROPERTY_NAME, AppState._App->Options.Graphics.Width);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_HEIGHT_PROPERTY_NAME, AppState._App->Options.Graphics.Height);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_BITS_PER_PIXEL_PROPERTY_NAME, AppState._App->Options.Graphics.BitsPerPixel);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_IS_USING_MODULE_PROPERTY_NAME, *RendererState.Options._IsUsingModule);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_ACCELERATED_GRAPHICS_PORT_PROPERTY_NAME, (s32)*RendererState.Options._AcceleratedGraphicsPort);
        IniFileWriteString(&ini, OPTIONS_INI_GRAPHICS_RENDERER_MODULE_NAME_PROPERTY_NAME, RendererState.Options._ModuleFileName);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_IS_FIRST_TIME_PROPERTY_NAME, (s32)*AppOptionsState._FirstTimeMode);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SUBTITLES_PROPERTY_NAME, AppState._App->Options.Graphics.Subtitles);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_BLOOD_PROPERTY_NAME, AppState._App->Options.Graphics.Blood);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SHADOWS_PROPERTY_NAME, AppState._App->Options.Graphics.Shadows);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_QUIMBY_PROPERTY_NAME, AppState._App->Options.Graphics.Quimby);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SELECTED_DEVICE_INDEX_PROPERTY_NAME, RendererState.Options.SelectedDeviceIndex);
        AbstractIniWriteReal((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_BRIGHTNESS_PROPERTY_NAME, *AppOptionsState.Graphics._Brightness);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SHADOW_MODE_PROPERTY_NAME, (s32)*AppOptionsState.Graphics._ShadowMode);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_SCREEN_MODE_PROPERTY_NAME, (s32)*AppOptionsState.Graphics._ScreenMode);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_BUMP_MODE_PROPERTY_NAME, (s32)*AppOptionsState.Graphics._TextureBumpMappingMode);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_TEXTURE_DETAIL_MODE_PROPERTY_NAME, (s32)*AppOptionsState.Graphics._TextureDetailMode);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_GRAPHICS_REFLECTION_MODE_PROPERTY_NAME, (s32)*AppOptionsState.Graphics._WaterReflectionMode);

        AbstractIniSelectSection((AbstractIni*)&ini, OPTIONS_INI_SOUND_SECTION_NAME);
        
        SaveAppSoundOptions(&ini);

        AbstractIniSelectSection((AbstractIni*)&ini, OPTIONS_INI_CONTROL_SECTION_NAME);
        
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_CONTROL_IS_ACTIVE_PROPERTY_NAME, AppState._App->Options.IsControllerActive);

        for (u32 x = 0; x < MAX_APPLICATION_OPTIONS_KEY_COUNT; x++)
        {
            char buffer[MAX_INI_SECTION_LENGTH];

            sprintf(buffer, OPTIONS_INI_CONTROL_MAIN_BUTTON_PROPERTY_NAME, x);
            AbstractIniWriteInteger((AbstractIni*)&ini, buffer, AppState._App->Options.Keys.Primary[x]);

            sprintf(buffer, OPTIONS_INI_CONTROL_ALTERNATIVE_BUTTON_PROPERTY_NAME, x);
            AbstractIniWriteInteger((AbstractIni*)&ini, buffer, AppState._App->Options.Keys.Secondary[x]);

            sprintf(buffer, OPTIONS_INI_CONTROL_CONTROLLER_BUTTON_PROPERTY_NAME, x);
            AbstractIniWriteInteger((AbstractIni*)&ini, buffer, AppState._App->Options.Controller.Keys[x]);
        }

        AbstractIniWriteReal((AbstractIni*)&ini, OPTIONS_INI_CONTROL_MOUSE_SENSITIVITY_X_PROPERTY_NAME, AppState._App->Options.MouseSensitivityX);
        AbstractIniWriteReal((AbstractIni*)&ini, OPTIONS_INI_CONTROL_MOUSE_SENSITIVITY_Y_PROPERTY_NAME, AppState._App->Options.MouseSensitivityY);
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_CONTROL_AUTO_CENTER_PITCH_PROPERTY_NAME, AppState._App->Options.AutoCenterPitch);

        for (u32 x = 0; x < MAX_APPLICATION_OPTIONS_CONTROLLER_AXIS_COUNT; x++)
        {
            char buffer[MAX_INI_SECTION_LENGTH];

            sprintf(buffer, OPTIONS_INI_CONTROL_GAMEPAD_AXIS_PROPERTY_NAME, x);
            AbstractIniWriteInteger((AbstractIni*)&ini, buffer, AppState._App->Options.Controller.Axes[x]);

            sprintf(buffer, OPTIONS_INI_CONTROL_GAMEPAD_INVERT_AXIS_PROPERTY_NAME, x);
            AbstractIniWriteInteger((AbstractIni*)&ini, buffer, AppState._App->Options.Controller.AxesInversion[x]);
        }

        SaveAppControllerOptions(&ini);

        AbstractIniSelectSection((AbstractIni*)&ini, OPTIONS_INI_DEBUG_SECTION_NAME);
        
        AbstractIniWriteInteger((AbstractIni*)&ini, OPTIONS_INI_DEBUG_CONSOLE_ACTIVE_PROPERTY_NAME, (*ConsoleState._ConsolePointer)->IsActive);
        
        AbstractIniSelectSection((AbstractIni*)&ini, OPTIONS_INI_EDITOR_SECTION_NAME);

        IniFileDispose(&ini, DisposeMode::None);
    }
}