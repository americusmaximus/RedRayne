#include "Localization.hxx"
#include "Native.Basic.hxx"
#include "Input.hxx"

namespace Localization
{
    LocalizationContainer LocalizationState;

    const char* LocalizeKeyName1(const u32 code)
    {
        if (code < INPUT_KEY_LEFT_MOUSE_BUTTON)
        {
            if (GetKeyNameTextA((code & 0x1ff) << 16, LocalizationState.KeyNameBuffer, MAX_LOCALIZATION_KEY_NAME_BUFFER_LENGTH) != 0)
            {
                return LocalizationState.KeyNameBuffer;
            }
        }

        switch (code)
        {
        case INPUT_KEY_ESC: { return "ESC"; }
        case INPUT_KEY_1: { return "1"; }
        case INPUT_KEY_2: { return "2"; }
        case INPUT_KEY_3: { return "3"; }
        case INPUT_KEY_4: { return "4"; }
        case INPUT_KEY_5: { return "5"; }
        case INPUT_KEY_6: { return "6"; }
        case INPUT_KEY_7: { return "7"; }
        case INPUT_KEY_8: { return "8"; }
        case INPUT_KEY_9: { return "9"; }
        case INPUT_KEY_0: { return "0"; }
        case INPUT_KEY_MINUS: { return "-"; }
        case INPUT_KEY_EQUAL: { return "="; }
        case INPUT_KEY_BACKSPACE: { return "BACKSPACE"; }
        case INPUT_KEY_TAB: { return "TAB"; }
        case INPUT_KEY_Q: { return "Q"; }
        case INPUT_KEY_W: { return "W"; }
        case INPUT_KEY_E: { return "E"; }
        case INPUT_KEY_R: { return "R"; }
        case INPUT_KEY_T: { return "T"; }
        case INPUT_KEY_Y: { return "Y"; }
        case INPUT_KEY_U: { return "U"; }
        case INPUT_KEY_I: { return "I"; }
        case INPUT_KEY_O: { return "O"; }
        case INPUT_KEY_P: { return "P"; }
        case INPUT_KEY_OPEN_BRACKET: { return "["; }
        case INPUT_KEY_CLOSE_BRACKET: { return "]"; }
        case INPUT_KEY_ENTER: { return "ENTER"; }
        case INPUT_KEY_LEFT_CTRL: { return "LEFT CONTROL"; }
        case INPUT_KEY_A: { return "A"; }
        case INPUT_KEY_S: { return "S"; }
        case INPUT_KEY_D: { return "D"; }
        case INPUT_KEY_F: { return "F"; }
        case INPUT_KEY_G: { return "G"; }
        case INPUT_KEY_H: { return "H"; }
        case INPUT_KEY_J: { return "J"; }
        case INPUT_KEY_K: { return "K"; }
        case INPUT_KEY_L: { return "L"; }
        case INPUT_KEY_SEMI_COLON: { return ";"; }
        case INPUT_KEY_SINGLE_QUOTE: { return "'"; }
        case INPUT_KEY_BACK_TICK: { return "`"; }
        case INPUT_KEY_LEFT_SHIFT: { return "LEFT SHIFT"; }
        case INPUT_KEY_BACK_SLASH: { return "\\"; }
        case INPUT_KEY_Z: { return "Z"; }
        case INPUT_KEY_X: { return "X"; }
        case INPUT_KEY_C: { return "C"; }
        case INPUT_KEY_V: { return "V"; }
        case INPUT_KEY_B: { return "B"; }
        case INPUT_KEY_N: { return "N"; }
        case INPUT_KEY_M: { return "M"; }
        case INPUT_KEY_COMA: { return ","; }
        case INPUT_KEY_DOT: { return "."; }
        case INPUT_KEY_FORWARD_SLASH: { return "/"; }
        case INPUT_KEY_RIGHT_SHIFT: { return "RIGHT SHIFT"; }
        case INPUT_KEY_PRINT_SCREEN: { return "PRINT SCREEN"; }
        case INPUT_KEY_LEFT_ALT: { return "LEFT ALT"; }
        case INPUT_KEY_SPACE: { return "SPACE"; }
        case INPUT_KEY_CAPS_LOCK: { return "CAPS LOCK"; }
        case INPUT_KEY_F1: { return "F1"; }
        case INPUT_KEY_F2: { return "F2"; }
        case INPUT_KEY_F3: { return "F3"; }
        case INPUT_KEY_F4: { return "F4"; }
        case INPUT_KEY_F5: { return "F5"; }
        case INPUT_KEY_F6: { return "F6"; }
        case INPUT_KEY_F7: { return "F7"; }
        case INPUT_KEY_F8: { return "F8"; }
        case INPUT_KEY_F9: { return "F9"; }
        case INPUT_KEY_F10: { return "F10"; }
        case INPUT_KEY_NUM_LOCK: { return "NUM LOCK"; }
        case INPUT_KEY_SCROLL_LOCK: { return "SCROLL LOCK"; }
        case INPUT_KEY_NUM_PAD_7: { return "NUMPAD 7"; }
        case INPUT_KEY_NUM_PAD_8: { return "NUMPAD 8"; }
        case INPUT_KEY_NUM_PAD_9: { return "NUMPAD 9"; }
        case INPUT_KEY_NUM_PAD_MINUS: { return "NUMPAD -"; }
        case INPUT_KEY_NUM_PAD_4: { return "NUMPAD 4"; }
        case INPUT_KEY_NUM_PAD_5: { return "NUMPAD 5"; }
        case INPUT_KEY_NUM_PAD_6: { return "NUMPAD 6"; }
        case INPUT_KEY_NUM_PAD_PLUS: { return "NUMPAD +"; }
        case INPUT_KEY_NUM_PAD_1: { return "NUMPAD 1"; }
        case INPUT_KEY_NUM_PAD_2: { return "NUMPAD 2"; }
        case INPUT_KEY_NUM_PAD_3: { return "NUMPAD 3"; }
        case INPUT_KEY_NUM_PAD_0: { return "NUMPAD 0"; }
        case INPUT_KEY_NUM_PAD_DOT: { return "NUMPAD ."; }

        case INPUT_KEY_NUM_PAD_ASTERISK: { return "NUMPAD *"; }
        case INPUT_KEY_F11: { return "F11"; }
        case INPUT_KEY_F12: { return "F12"; }

        case INPUT_KEY_NUM_PAD_ENTER: { return "NUMPAD ENTER"; }
        case INPUT_KEY_NUM_RIGHT_CTRL: { return "RIGHT CONTROL"; }

        case INPUT_KEY_NUM_SLASH: { return "NUMPAD /"; }

        case INPUT_KEY_RIGHT_ALT: { return "RIGHT ALT"; }

        case INPUT_KEY_PAUSE: { return "PAUSE"; }
        case INPUT_KEY_HOME: { return "HOME"; }
        case INPUT_KEY_UP_ARROW: { return "UP ARROW"; }
        case INPUT_KEY_PAGE_UP: { return "PAGE UP"; }

        case INPUT_KEY_LEFT_ARROW: { return "LEFT ARROW"; }
        case INPUT_KEY_RIGHT_ARROW: { return "RIGHT ARROW"; }

        case INPUT_KEY_DOWN_ARROW: { return "DOWN ARROW"; }
        case INPUT_KEY_PAGE_DOWN: { return "PAGE DOWN"; }

        case INPUT_KEY_LEFT_MOUSE_BUTTON: { return "LEFT MOUSE BUTTON"; }
        case INPUT_KEY_RIGHT_MOUSE_BUTTON: { return "RIGHT MOUSE BUTTON"; }

        case INPUT_KEY_MIDDLE_MOUSE_BUTTON: { return "MIDDLE MOUSE BUTTON"; }
        case INPUT_KEY_X1_MOUSE_BUTTON: { return "X1 MOUSE BUTTON"; }
        case INPUT_KEY_X2_MOUSE_BUTTON: { return "X2 MOUSE BUTTON"; }
        case INPUT_KEY_FORWARD_MOUSE_WHEEL: { return "FORWARD MOUSE WHEEL"; }
        case INPUT_KEY_BACKWARD_MOUSE_WHEEL: { return "BACKWARD MOUSE WHEEL"; }
        }

        return "<INVALID KEY>";
    }

    const char* LocalizeKeyName2(const u32 code)
    {
        switch (code)
        {
        case INPUT_KEY_LEFT_MOUSE_BUTTON: { return "LEFT MOUSE BUTTON"; }
        case INPUT_KEY_RIGHT_MOUSE_BUTTON: { return "RIGHT MOUSE BUTTON"; }

        case INPUT_KEY_MIDDLE_MOUSE_BUTTON: { return "MIDDLE MOUSE BUTTON"; }

        case INPUT_KEY_X_AXIS_MOUSE: { return "Mouse X-Axis"; }
        case INPUT_KEY_Y_AXIS_MOUSE: { return "Mouse Y-Axis"; }
        case INPUT_KEY_NONE: { return "<NONE>"; }

        }

        return LocalizeKeyName1(code);
    }
}