#include "Input.hxx"
#include "Input.Controllers.hxx"
#include "Time.hxx"
#include "App.Windows.hxx"

using namespace Input::Controllers;
using namespace Time;
using namespace App::Windows;

namespace Input
{
    InputContainer InputState;

    void ReleaseUnknownInput(void)
    {
        *InputState.Text._InputIndex1 = 0;
        *InputState.Text._InputIndex2 = 0;

        *InputState._Unknown103 = 0;
    }

    void ReleaseMouseInput(void)
    {
        *InputState.Mouse._WheelDelta = 0;

        *ControllerState.Mouse._X = 0;
        *ControllerState.Mouse._Y = 0;

        *InputState._Unk2 = 0;
    }

    typedef const u32(CDECLAPI* ACQUIRETIMEDEBUG) (void);
    ACQUIRETIMEDEBUG AcquireTimeTODO = (ACQUIRETIMEDEBUG)0x005cc380;

    void ReleaseInput(void)
    {
        ReleaseUnknownInput();
        ReleaseMouseInput();

        for (u32 x = 0; x < MAX_INPUT_ELEMENT_COUNT; x++)
        {
            InputState._InputPressed[x] = FALSE;
        }

        const auto start = AcquireTime() >> 0x10; // Division by 65536.

        while (TRUE)
        {
            const auto now = AcquireTime() >> 0x10; // Division by 65536.

            if (start != now) { break; }

            ReleaseUnknownInput();
        }

        ReleaseUnknownInput();
        ReleaseMouseInput();

        for (u32 x = 0; x < MAX_INPUT_ELEMENT_COUNT; x++)
        {
            InputState._InputPressed[x] = FALSE;
        }
    }

    void HandleInputText(const u32 code)
    {
        InputState.Text._Text[*InputState.Text._InputIndex1] = code;

        *InputState.Text._InputIndex1 = *InputState.Text._InputIndex1 + 1;

        if ((MAX_INPUT_TEXT_LENGTH - 1) < *InputState.Text._InputIndex1)
        {
            *InputState.Text._InputIndex1 = 0;
        }

        if (*InputState.Text._InputIndex1 == *InputState.Text._InputIndex2)
        {
            *InputState.Text._InputIndex2 = *InputState.Text._InputIndex2 + 1;

            if ((MAX_INPUT_TEXT_LENGTH - 1) < *InputState.Text._InputIndex2)
            {
                *InputState.Text._InputIndex2 = 0;
            }
        }
    }

    void InputSelectMouse(const s32 x, const s32 y)
    {
        POINT point = { .x = x, .y = y };

        ClientToScreen(*AppWindowState._HWND, &point);
        SetCursorPos(point.x, point.y);

        *InputState.Mouse._X = x;
        *InputState.Mouse._Y = y;
    }
}