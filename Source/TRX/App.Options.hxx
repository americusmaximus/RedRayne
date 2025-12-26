#pragma once

#include "Basic.hxx"

#define MAX_APPLICATION_LANGUAGE_COUNT 7

#define MAX_APPLICATION_OPTIONS_KEY_COUNT 24

#define MAX_APPLICATION_OPTIONS_CONTROLLER_AXIS_COUNT 4
#define MAX_APPLICATION_OPTIONS_CONTROLLER_AXIS_OPTIONS_COUNT 6

namespace App::Options
{
    enum class ApplicationLanguage : u32
    {
        Unknown = 0,
        English = 1,
        French = 2,
        Italian = 3,
        Spanish = 4,
        German = 5,
        Japanese = 6
    };

    enum class ApplicationShadowOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationWideScreenOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationTextureBumpMapOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class ApplicationTextureDetailOptionsMode : u32
    {
        Normal = 0,
        High = 1
    };

    enum class ApplicationWaterReflectionOptionsMode : u32
    {
        Off = 0,
        Simple = 1,
        Complex = 2
    };

    enum class ApplicationFirstTimeOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    struct ApplicationGraphicsOptions
    {
        u32 Width;
        u32 Height;
        u32 BitsPerPixel;
        u32 Subtitles;// TODO Enum
        u32 Blood;// TODO ennum
        u32 Unk1;// TODO
        u32 Unk2;//TODO
        u32 Quimby;// TODO enum
        u32 Shadows; // TODO enum
    };

    enum class ApplicationOptionsInput : u32
    {
        BladeAttack = 0,
        WeaponAttack = 1,
        CenterView = 2,
        Jump = 3,
        HarpponBite = 4,
        QuickTurn = 5,
        BloodRage = 6,
        NormalVision = 7,
        AuraVision = 8,
        ExtrudedVision = 9,
        DilatedVision = 10,
        WeaponModeForward = 11,
        WeaponModeReverse = 12,
        ScreenShot = 13,
        Debug = 14,
        Pause = 15,
        MoveForward = 16,
        MoveBackward = 17,
        MoveLeft = 18,
        MoveRight = 19,
        LookUp = 20,
        LookDown = 21,
        LookLeft = 22,
        LookRight = 23
    };

    struct ApplicationKeyOptions
    {
        // TODO: Enumeration for Key Use instead of index
        s32 Primary[MAX_APPLICATION_OPTIONS_KEY_COUNT];
        // TODO: Enumeration for Key Use instead of index
        s32 Secondary[MAX_APPLICATION_OPTIONS_KEY_COUNT];
    };

    struct ApplicationControllerKeyOptions
    {
        // TODO: Enumeration for Key Use instead of index
        s32 Keys[MAX_APPLICATION_OPTIONS_KEY_COUNT];

        // TODO: Enumeration for axis instead of index
        s32 Axes[MAX_APPLICATION_OPTIONS_CONTROLLER_AXIS_COUNT];

        // TODO: Enumeration for axis instead of index
        BOOL AxesInversion[MAX_APPLICATION_OPTIONS_CONTROLLER_AXIS_COUNT];
    };

    struct ApplicationOptions
    {
        ApplicationGraphicsOptions Graphics;
        ApplicationKeyOptions Keys;
        ApplicationControllerKeyOptions Controller;
        BOOL AutoCenterPitch;//TODO type
        f32 MouseSensitivityX;
        f32 MouseSensitivityY;
        int Unk1;//TODO
        BOOL IsControllerActive;//TODO type, enum?
    };

    struct ApplicationControllerAxisOptions
    {
        s32 Minimum;
        s32 Maximum;
        s32 Center;
    };

    struct ApplicationControllerOptions
    {
        // TODO: X, Y, Z, R, U, V
        ApplicationControllerAxisOptions Axes[MAX_APPLICATION_OPTIONS_CONTROLLER_AXIS_OPTIONS_COUNT];
    };

    struct ApplicationOptionsContainer
    {
        ApplicationLanguage Language = ApplicationLanguage::English;

        ApplicationFirstTimeOptionsMode* _FirstTimeMode = (ApplicationFirstTimeOptionsMode*)0x00721e98;//TODO

        struct
        {
            f32* _Brightness = (f32*)0x00625318;//todo, def value
            ApplicationShadowOptionsMode* _ShadowMode = (ApplicationShadowOptionsMode*)0x0085a348;//TODO
            ApplicationWideScreenOptionsMode* _ScreenMode = (ApplicationWideScreenOptionsMode*)0x0085a344;//TODO
            ApplicationTextureBumpMapOptionsMode* _TextureBumpMappingMode = (ApplicationTextureBumpMapOptionsMode*)0x0084aefc;//TODO
            ApplicationTextureDetailOptionsMode* _TextureDetailMode = (ApplicationTextureDetailOptionsMode*)0x0068ac80;//TODO
            ApplicationWaterReflectionOptionsMode* _WaterReflectionMode = (ApplicationWaterReflectionOptionsMode*)0x00901854;//TODO
        } Graphics;

        // TODO, should be in input...
        // TODO, this is onot an option, this is a state
        ApplicationControllerOptions* _Controllers = (ApplicationControllerOptions*)0x009049ac; // TODO [MAX_CONTROLLER_COUNT]
    };

    extern ApplicationOptionsContainer AppOptionsState;

    ApplicationLanguage AcquireAppLanguage(void);
    const char* AcquireAppLanguageCode(const ApplicationLanguage language);
    void SelectAppLanguage(const ApplicationLanguage language);

    void LoadAppOptions(void);
    void InitializeAppKeyOptions(ApplicationOptions* options);
    void InitializeControllersOptions(ApplicationOptions* options);

    void SaveAppOptions(void);
}