#pragma once


#include "App.Options.hxx"
#include "Direct.Input.hxx"

#define MAX_CONTROLLER_COUNT 4
#define MAX_CONTROLLER_BUTTON_COUNT 32

#define MAX_CONTROLLER_UNKNOWN_BUTTON_COUNT 16

typedef const MMRESULT(WINAPI* JOYGETPOSEX) (UINT uJoyID, LPJOYINFOEX pji);

namespace Input::Controllers
{
    struct ControllerCapabilities
    {
        BOOL HasX;
        BOOL HasY;
        BOOL HasZ;
        BOOL HasR;
        BOOL HasU;
        BOOL HasV;
        BOOL HasPOV;
    };

    struct ControllerAxes
    {
        f32 X;
        f32 Y;
        f32 Z;
        f32 R;
        f32 U;
        f32 V;
    };

    struct Controller
    {
        s32 Type;// TODO enum

        BOOL IsActive;

        ControllerCapabilities Capabilities;

        ControllerAxes Axes;

        s32 Unk4;//TODO
        s32 Unk6; //TODO

        BOOL UnknownButtonState1[MAX_CONTROLLER_UNKNOWN_BUTTON_COUNT];//TODO
        BOOL UnknownButtonState2[MAX_CONTROLLER_UNKNOWN_BUTTON_COUNT];//TODO

        s32 Unk2;
        s32 Unk3;
        s32 Unk1;
        s32 Unk5;

        BOOL CurrentButtonState[MAX_CONTROLLER_BUTTON_COUNT]; // TODO: struct?
        BOOL PreviousButtonState[MAX_CONTROLLER_BUTTON_COUNT]; // TODO: struct?

        s32 Unk7;//TODO
    };

    struct ControllerContainer
    {
        JOYGETPOSEX JoyGetPosEx;// = (JOYGETPOSEX*)0x00904990;//TODO

        IDirectInputA* DirectInput;// = (IDirectInputA**)0x00904994;//TODO
        IDirectInputDeviceA* InputDevice;// = (IDirectInputDeviceA**)0x00904998;//TODO

        struct
        {
            s32* _X = (s32*)0x0067f034;//TODO
            s32* _Y = (s32*)0x0067f038;//TODO
        } Mouse;

        s32* _ControllerCount = (s32*)0x0061ddf0;//TODO default value

        s32* _UnknownControllerState1 = (s32*)0x00904acc;//TODO

        // TODO better name
        u32* _ControllerPorts = (u32*)0x0090499c; // [CONTROLLER_COUNT];
        Controller* _Controllers = (Controller*)0x00670074; // TODO [MAX_CONTROLLER_COUNT];
    };

    extern ControllerContainer ControllerState;

    void InitializeControllers(void);

    void InitializeController1(const s32 indx);
    void InitializeController2(const s32 indx, const s32 type, const u32 port);

    const extern DIDATAFORMAT MouseInputDataFormat;

    struct InputMouseState
    {
        s32 X;
        s32 Y;
        s32 Z;
        u8 B1;
        u8 B2;
        u8 B3;
        u8 B4;
    };

    void AcquireControllerState(void);

    void ReleaseControllers(void);

    void AcquireControllerInputState(const u32 indx);

    f32 AcquireControllerInputAxisState(const u32 value, App::Options::ApplicationControllerAxisOptions* options, const f32 min, const f32 max);

    void AcquireControllerInputState1(const u32 indx);
    void AcquireControllerInputState2(const u32 indx);
}