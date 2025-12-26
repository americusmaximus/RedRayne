#include "Input.Controllers.hxx"
#include "Logger.hxx"
#include "App.Windows.hxx"

#define CONTROLLER_WINMM_LIBRARY_NAME "WINMM.DLL"
#define CONTROLLER_JOY_GET_POS_EX_NAME "joyGetPosEx"

#define MOUSE_DEVICE_OBJECT_DATA_FORMAT_COUNT 7

#define MAX_CONTROLLER_NON_POVBUTTON_COUNT 28

using namespace App::Options;
using namespace App::Windows;
using namespace Logger;

namespace Input::Controllers
{
    ControllerContainer ControllerState;

    DIOBJECTDATAFORMAT ObjectDataFormats[MOUSE_DEVICE_OBJECT_DATA_FORMAT_COUNT] =
    {
        {
            .pguid = &GUID_XAxis,
            .dwOfs = offsetof(InputMouseState, X),
            .dwType = DIDFT_ANYINSTANCE | DIDFT_AXIS,
            .dwFlags = 0
        },
        {
            .pguid = &GUID_YAxis,
            .dwOfs = offsetof(InputMouseState, Y),
            .dwType = DIDFT_ANYINSTANCE | DIDFT_AXIS,
            .dwFlags = 0
        },
        {
            .pguid = &GUID_ZAxis,
            .dwOfs = offsetof(InputMouseState, Z),
            .dwType = 0x80ffff03, // TODO XXX | DIDFT_ANYINSTANCE | DIDFT_AXIS,
            .dwFlags = 0
        },
        {
            .pguid = NULL,
            .dwOfs = offsetof(InputMouseState, B1),
            .dwType = DIDFT_ANYINSTANCE | DIDFT_BUTTON,
            .dwFlags = 0
        },
        {
            .pguid = NULL,
            .dwOfs = offsetof(InputMouseState, B2),
            .dwType = DIDFT_ANYINSTANCE | DIDFT_BUTTON,
            .dwFlags = 0
        },
        {
            .pguid = NULL,
            .dwOfs = offsetof(InputMouseState, B3),
            .dwType = 0x80FFFF0C, //TODO XXX | DIDFT_ANYINSTANCE | DIDFT_BUTTON,
            .dwFlags = 0
        },
        {
            .pguid = NULL,
            .dwOfs = offsetof(InputMouseState, B4),
            .dwType = 0x80FFFF0C, //TODO XXX | DIDFT_ANYINSTANCE | DIDFT_BUTTON,
            .dwFlags = 0
        }
    };

    const DIDATAFORMAT MouseInputDataFormat =
    {
        .dwSize = sizeof(DIDATAFORMAT),
        .dwObjSize = sizeof(DIOBJECTDATAFORMAT),
        .dwFlags = DIDF_RELAXIS,
        .dwDataSize = sizeof(InputMouseState),
        .dwNumObjs = MOUSE_DEVICE_OBJECT_DATA_FORMAT_COUNT,
        .rgodf = ObjectDataFormats
    };

    void InitializeControllers(void)
    {
        const auto winmm = GetModuleHandleA(CONTROLLER_WINMM_LIBRARY_NAME);

        if (winmm != NULL)
        {
            ControllerState.JoyGetPosEx = (JOYGETPOSEX)GetProcAddress(winmm, CONTROLLER_JOY_GET_POS_EX_NAME);
        }

        // NOTE: Originally: DirectInputCreateA version 0x700.
        auto result = DirectInput8Create(*AppWindowState._Instance, DIRECTINPUT_VERSION, IID_IDirectInput8A, (void**)&ControllerState.DirectInput, NULL);

        if (result == DI_OK && ControllerState.DirectInput != NULL)
        {
            result = (ControllerState.DirectInput)->CreateDevice(GUID_SysMouse, &ControllerState.InputDevice, NULL);

            if (result == DI_OK && ControllerState.InputDevice != NULL)
            {
                result = ControllerState.InputDevice->SetCooperativeLevel(*AppWindowState._HWND, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

                if (result == DI_OK)
                {
                    result = ControllerState.InputDevice->SetDataFormat(&MouseInputDataFormat);

                    if (result == DI_OK)
                    {
                        result = ControllerState.InputDevice->Acquire();

                        if (result == DI_OK)
                        {
                            AcquireControllerState();

                            return;
                        }
                        else
                        {
                            LogMessage("[CONTROL] Unable to acquire mouse device. Code %08X.\n", result);
                        }
                    }
                    else
                    {
                        LogMessage("[CONTROL] Unable to set mouse device data format. Code %08X.\n", result);
                    }
                }
                else
                {
                    LogMessage("[CONTROL] Unable to create mouse device. Code %08X.\n", result);
                }

                ControllerState.InputDevice->Release();
                ControllerState.InputDevice = NULL;
            }
            else
            {
                LogMessage("[CONTROL] Unable to create mouse device. Code %08X.\n", result);
            }
        }
        else
        {
            LogMessage("[CONTROL] Unable to create direct input instance. Code %08X.\n", result);
        }

        AcquireControllerState();
    }

    void InitializeController1(const s32 indx)
    {
        if (ControllerState._Controllers[indx].Type != 0)
        {
            *ControllerState._ControllerCount = *ControllerState._ControllerCount + 1;
        }

        ZeroMemory(&ControllerState._Controllers[indx], sizeof(Controller));
    }

    void InitializeController2(const s32 indx, const s32 type, const u32 port)
    {
        InitializeController1(indx);

        ControllerState._Controllers[indx].IsActive = TRUE;

        ControllerState._Controllers[indx].Type = type;//todo enum?
        ControllerState._ControllerPorts[indx] = port;

        ZeroMemory(&ControllerState._Controllers[indx].Capabilities, sizeof(ControllerCapabilities));

        if (type != 0 && type != 4) // TODO
        {
            JOYCAPSA caps;

            ZeroMemory(&caps, sizeof(JOYCAPSA));

            if (joyGetDevCapsA(port, &caps, sizeof(JOYCAPSA)) == MMSYSERR_NOERROR)
            {
                ControllerState._Controllers[indx].Capabilities.HasX = TRUE;
                ControllerState._Controllers[indx].Capabilities.HasY = TRUE;

                ControllerState._Controllers[indx].Capabilities.HasZ = caps.wCaps & JOYCAPS_HASZ;
                ControllerState._Controllers[indx].Capabilities.HasR = caps.wCaps & JOYCAPS_HASR;
                ControllerState._Controllers[indx].Capabilities.HasPOV = caps.wCaps & JOYCAPS_HASPOV;
            }
            else
            {
                ControllerState._Controllers[indx].IsActive = FALSE;
            }
        }

        *ControllerState._ControllerCount = *ControllerState._ControllerCount + 1;

        AcquireControllerState();
    }

    typedef const void(CDECLAPI* AFUN_00432300) ();//TODO
    AFUN_00432300 FUN_00432300 = (AFUN_00432300)0x00432300;//TODO

    typedef const void(CDECLAPI* AFUN_00432490) ();//TODO
    AFUN_00432490 FUN_00432490 = (AFUN_00432490)0x00432490;//TODO

    void AcquireControllerState(void)
    {
        FUN_00432300();

        for (u32 x = 0; x < MAX_CONTROLLER_COUNT; x++)
        {
            AcquireControllerInputState(x);
        }

        if (ControllerState.InputDevice != NULL)
        {
            InputMouseState state;

            if (ControllerState.InputDevice->GetDeviceState(sizeof(InputMouseState), &state) == DI_OK)
            {
                *ControllerState.Mouse._X = *ControllerState.Mouse._X + state.X;
                *ControllerState.Mouse._Y = *ControllerState.Mouse._Y + state.Y;
            }
        }

        FUN_00432490();
    }

    void ReleaseControllers(void)
    {
        if (ControllerState.InputDevice != NULL)
        {
            ControllerState.InputDevice->Unacquire();
            ControllerState.InputDevice->Release();

            ControllerState.InputDevice = NULL;
        }

        if (ControllerState.DirectInput != NULL)
        {
            ControllerState.DirectInput->Release();

            ControllerState.DirectInput = NULL;
        }
    }

    void AcquireControllerInputState(const u32 indx)
    {
        auto controller = &ControllerState._Controllers[indx];
        const auto port = ControllerState._ControllerPorts[indx];
        auto options = &AppOptionsState._Controllers->Axes[indx];

        if (controller->Type == 0 || controller->Type == 4) { return; } // TODO

        controller->Unk3 = controller->Unk3;
        controller->Unk5 = controller->Unk1;

        CopyMemory(controller->PreviousButtonState, controller->CurrentButtonState, MAX_CONTROLLER_BUTTON_COUNT * sizeof(BOOL));

        if (ControllerState.JoyGetPosEx == NULL)
        {
            JOYINFO joyInfo;

            ZeroMemory(&joyInfo, sizeof(JOYINFO));

            if (joyGetPos(port, &joyInfo) == JOYERR_NOERROR)
            {
                controller->IsActive = TRUE;

                controller->Axes.X = AcquireControllerInputAxisState(joyInfo.wXpos, &options[0], -1.0f, 1.0f);
                controller->Axes.Y = AcquireControllerInputAxisState(joyInfo.wYpos, &options[1], -1.0f, 1.0f);

                if (controller->Capabilities.HasZ)
                {
                    controller->Axes.Z = AcquireControllerInputAxisState(joyInfo.wZpos, &options[2], 0.0f, 1.0f);//TODO shouldn't it be -1.0f as well?
                }

                for (u32 x = 0; x < MAX_CONTROLLER_BUTTON_COUNT; x++)
                {
                    const u32 mask = (1 << (x & 0x1f));
                    controller->CurrentButtonState[x] = (joyInfo.wButtons & mask) != 0;
                }
            }
            else
            {
                controller->IsActive = FALSE;
            }
        }
        else
        {
            JOYINFOEX joyInfo =
            {
                .dwSize = sizeof(JOYINFOEX),
                .dwFlags = JOY_RETURNRAWDATA | JOY_RETURNBUTTONS | JOY_RETURNY | JOY_RETURNX
            };

            if (controller->Capabilities.HasZ) { joyInfo.dwFlags = joyInfo.dwFlags | JOY_RETURNZ; }
            if (controller->Capabilities.HasR) { joyInfo.dwFlags = joyInfo.dwFlags | JOY_RETURNR; }

            if (controller->Capabilities.HasPOV) { joyInfo.dwFlags = joyInfo.dwFlags | JOY_RETURNPOV; }

            if (ControllerState.JoyGetPosEx(port, &joyInfo) == JOYERR_NOERROR)
            {
                controller->IsActive = TRUE;

                controller->Axes.X = AcquireControllerInputAxisState(joyInfo.dwXpos, &options[0], -1.0f, 1.0f);
                controller->Axes.Y = AcquireControllerInputAxisState(joyInfo.dwYpos, &options[1], -1.0f, 1.0f);

                if (controller->Capabilities.HasZ)
                {
                    controller->Axes.Z = AcquireControllerInputAxisState(joyInfo.dwZpos, &options[2], -1.0f, 1.0f);
                }

                if (controller->Capabilities.HasR)
                {
                    controller->Axes.R = AcquireControllerInputAxisState(joyInfo.dwRpos, &options[3], -1.0f, 1.0f);
                }

                for (u32 x = 0; x < MAX_CONTROLLER_NON_POVBUTTON_COUNT; x++)
                {
                    const u32 mask = (1 << (x & 0x1f));
                    controller->CurrentButtonState[x] = (joyInfo.dwButtons & mask) != 0;
                }

                controller->Unk1 = 0;
                controller->Unk2 = 0;

                // TODO: rewrite these conditions!
                // Values are degrees * 100
                if (!controller->Capabilities.HasPOV && joyInfo.dwPOV != JOY_POVCENTERED)
                {
                    if (joyInfo.dwPOV < 2251 || 33749 < joyInfo.dwPOV)
                    {
                        controller->Unk1 = -1;
                    }
                    else if (joyInfo.dwPOV < 6751)
                    {
                        controller->Unk1 = -1;
                        controller->Unk2 = 1;
                    }
                    else if (11250 < joyInfo.dwPOV)
                    {
                        if (joyInfo.dwPOV < 15751)
                        {
                            controller->Unk1 = 1;
                            controller->Unk2 = 1;
                        }
                        else if (joyInfo.dwPOV < 20251)
                        {
                            controller->Unk1 = 1;
                        }
                        else if (joyInfo.dwPOV < 24751)
                        {
                            controller->Unk1 = 1;
                            controller->Unk2 = -1;
                        }
                        else if (29250 < joyInfo.dwPOV)
                        {
                            if (joyInfo.dwPOV < 33751)
                            {
                                controller->Unk1 = -1;
                                controller->Unk2 = -1;
                            }
                        }
                        else
                        {
                            controller->Unk2 = -1;
                        }
                    }
                    else
                    {
                        controller->Unk2 = 1;
                    }
                }

                controller->CurrentButtonState[0x1c] = (-1 < controller->Unk2) ? FALSE : TRUE; // TODO
                controller->CurrentButtonState[0x1d] = (0 < controller->Unk2);//TODO
                controller->CurrentButtonState[0x1e] = (controller->Unk1 < 0);//TODO
                controller->CurrentButtonState[0x1f] = (0 < controller->Unk1);//TODO
            }
            else
            {
                controller->IsActive = FALSE;

                ZeroMemory(&controller->Axes, sizeof(ControllerAxes));
                ZeroMemory(&controller->CurrentButtonState, MAX_CONTROLLER_BUTTON_COUNT * sizeof(BOOL));
            }
        }

        if (controller->Unk2 != controller->Unk3)
        {
            controller->Unk4 = controller->Unk2;
        }

        if (controller->Unk1 != controller->Unk5)
        {
            controller->Unk6 = controller->Unk1;
        }

        for (u32 x = 0; x < MAX_CONTROLLER_UNKNOWN_BUTTON_COUNT; x++)
        {
            if (controller->CurrentButtonState[x] != controller->PreviousButtonState[x])
            {
                controller->UnknownButtonState1[x] = controller->CurrentButtonState[x];
            }
        }

        // TODO: weird loops that do not look like write anywhere..
        /*if (controller->Unk2 == 0 && controller->Unk1 == 0)
        {
            //TODO

            for (u32 x = 0; x < MAX_APPLICATION_OPTIONS_CONTROLLER_AXIS_OPTIONS_COUNT; x++)
            {

            }
        }*/

        controller->Unk7 = 0;
    }

    f32 AcquireControllerInputAxisState(const u32 value, ApplicationControllerAxisOptions* options, const f32 min, const f32 max)
    {
        if (*ControllerState._UnknownControllerState1 == 1)
        {
            if (value < options->Minimum)
            {
                options->Minimum = value;
            }

            if (options->Maximum < value)
            {
                options->Maximum = value;
            }

            options->Center = (options->Minimum + options->Maximum) / 2;
        }
        else if (*ControllerState._UnknownControllerState1 == 2)
        {
            options->Center = value;
        }

        s32 distance = 0;
        f32 limit = 0.0f;

        f32 center = 0.5f * (min + max);

        if (value < options->Center)
        {
            if (options->Center <= options->Minimum)
            {
                return center;
            }

            center = center - min;
            distance = options->Center - options->Minimum;
            limit = min;
        }
        else
        {
            if (options->Maximum <= options->Center)
            {
                return center;
            }

            center = center - max;
            distance = options->Center - options->Maximum;
            limit = max;
        }

        limit = limit + (f32)distance / (center * (f32)(value - limit));

        if (min <= limit)
        {
            if (limit <= max) { return limit; }

            return max;
        }

        return min;
    }

    void AcquireControllerInputState1(const u32 indx)
    {
        *ControllerState._UnknownControllerState1 = 1;
        
        AcquireControllerInputState(indx);

        *ControllerState._UnknownControllerState1 = 0;
    }

    void AcquireControllerInputState2(const u32 indx)
    {
        *ControllerState._UnknownControllerState1 = 2;

        AcquireControllerInputState(indx);

        *ControllerState._UnknownControllerState1 = 0;
    }
}