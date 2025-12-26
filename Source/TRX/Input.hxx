#pragma once

#include "Basic.hxx"

#define MAX_INPUT_ELEMENT_COUNT 600

#define INPUT_KEY_ERROR 0
#define INPUT_KEY_ESC 1
#define INPUT_KEY_1 2
#define INPUT_KEY_2 3
#define INPUT_KEY_3 4
#define INPUT_KEY_4 5
#define INPUT_KEY_5 6
#define INPUT_KEY_6 7
#define INPUT_KEY_7 8
#define INPUT_KEY_8 9
#define INPUT_KEY_9 10
#define INPUT_KEY_0 11
#define INPUT_KEY_MINUS 12
#define INPUT_KEY_EQUAL 13
#define INPUT_KEY_BACKSPACE 14
#define INPUT_KEY_TAB 15
#define INPUT_KEY_Q 16
#define INPUT_KEY_W 17
#define INPUT_KEY_E 18
#define INPUT_KEY_R 19
#define INPUT_KEY_T 20
#define INPUT_KEY_Y 21
#define INPUT_KEY_U 22
#define INPUT_KEY_I 23
#define INPUT_KEY_O 24
#define INPUT_KEY_P 25
#define INPUT_KEY_OPEN_BRACKET 26
#define INPUT_KEY_CLOSE_BRACKET 27
#define INPUT_KEY_ENTER 28
#define INPUT_KEY_LEFT_CTRL 29
#define INPUT_KEY_A 30
#define INPUT_KEY_S 31
#define INPUT_KEY_D 32
#define INPUT_KEY_F 33
#define INPUT_KEY_G 34
#define INPUT_KEY_H 35
#define INPUT_KEY_J 36
#define INPUT_KEY_K 37
#define INPUT_KEY_L 38
#define INPUT_KEY_SEMI_COLON 39
#define INPUT_KEY_SINGLE_QUOTE 40
#define INPUT_KEY_BACK_TICK 41
#define INPUT_KEY_BACK_TILDE 41
#define INPUT_KEY_LEFT_SHIFT 42
#define INPUT_KEY_BACK_SLASH 43
#define INPUT_KEY_Z 44
#define INPUT_KEY_X 45
#define INPUT_KEY_C 46
#define INPUT_KEY_V 47
#define INPUT_KEY_B 48
#define INPUT_KEY_N 49
#define INPUT_KEY_M 50
#define INPUT_KEY_COMA 51
#define INPUT_KEY_DOT 52
#define INPUT_KEY_FORWARD_SLASH 53
#define INPUT_KEY_RIGHT_SHIFT 54
#define INPUT_KEY_PRINT_SCREEN 55
#define INPUT_KEY_LEFT_ALT 56
#define INPUT_KEY_SPACE 57
#define INPUT_KEY_CAPS_LOCK 58
#define INPUT_KEY_F1 59
#define INPUT_KEY_F2 60
#define INPUT_KEY_F3 61
#define INPUT_KEY_F4 62
#define INPUT_KEY_F5 63
#define INPUT_KEY_F6 64
#define INPUT_KEY_F7 65
#define INPUT_KEY_F8 66
#define INPUT_KEY_F9 67
#define INPUT_KEY_F10 68
#define INPUT_KEY_NUM_LOCK 69
#define INPUT_KEY_SCROLL_LOCK 70
#define INPUT_KEY_NUM_PAD_7 71
#define INPUT_KEY_NUM_PAD_8 72
#define INPUT_KEY_NUM_PAD_9 73
#define INPUT_KEY_NUM_PAD_MINUS 74
#define INPUT_KEY_NUM_PAD_4 75
#define INPUT_KEY_NUM_PAD_5 76
#define INPUT_KEY_NUM_PAD_6 77
#define INPUT_KEY_NUM_PAD_PLUS 78
#define INPUT_KEY_NUM_PAD_1 79
#define INPUT_KEY_NUM_PAD_2 80
#define INPUT_KEY_NUM_PAD_3 81
#define INPUT_KEY_NUM_PAD_0 82
#define INPUT_KEY_NUM_PAD_DOT 83

#define INPUT_KEY_NUM_PAD_ASTERISK 86
#define INPUT_KEY_F11 87
#define INPUT_KEY_F12 88

#define INPUT_KEY_NUM_PAD_ENTER 284
#define INPUT_KEY_NUM_RIGHT_CTRL 285

#define INPUT_KEY_NUM_SLASH 310

#define INPUT_KEY_RIGHT_ALT 312

#define INPUT_KEY_PAUSE 325
#define INPUT_KEY_HOME 327
#define INPUT_KEY_UP_ARROW 328
#define INPUT_KEY_PAGE_UP 329

#define INPUT_KEY_LEFT_ARROW 331

#define INPUT_KEY_RIGHT_ARROW 333

#define INPUT_KEY_END 335
#define INPUT_KEY_DOWN_ARROW 336
#define INPUT_KEY_PAGE_DOWN 337

#define INPUT_KEY_DEL 339

#define INPUT_KEY_LEFT_MOUSE_BUTTON 512
#define INPUT_KEY_RIGHT_MOUSE_BUTTON 513

#define INPUT_KEY_MIDDLE_MOUSE_BUTTON 524
#define INPUT_KEY_X1_MOUSE_BUTTON 525
#define INPUT_KEY_X2_MOUSE_BUTTON 526
#define INPUT_KEY_FORWARD_MOUSE_WHEEL 527
#define INPUT_KEY_BACKWARD_MOUSE_WHEEL 528

#define INPUT_KEY_X_AXIS_MOUSE 597
#define INPUT_KEY_Y_AXIS_MOUSE 598
#define INPUT_KEY_NONE 599

// TODO: Find better place for this.
#define MAX_INPUT_TEXT_LENGTH 20

namespace Input
{
    struct InputContainer
    {
        struct
        {
            s32* _X = (s32*)0x0067f028;//TODO
            s32* _Y = (s32*)0x0067f02c;//TODO

            s32* _WheelDelta = (s32*)0x0067f03c;//TODO
        } Mouse;

        s32* _Unk1 = (s32*)0x006707d4;//TODO
        s32* _Unk2 = (s32*)0x0067f030;//TODO some sort of flags
        u32* _KeyInputMask = (u32*)0x0061e9c4;//tODO, default value;

        u8* _InputPressed = (u8*)0x0068caa8;//TODO byte[MAX_INPUT_ELEMENT_COUNT], normalize data types
        u32* _InputReleased = (u32*)0x00903380;//TODO int[MAX_INPUT_ELEMENT_COUNT], normalize data types

        u8* _InputPressedCopy = (u8*)0x006c6480;//TODO., normalize data types

        s32* _KeyUnk1 = (s32*)0x006707d8;//TODO
        bool* _KeyUnk2 = (bool*)0x00903ce0;//TODO

        s32* _Unknown103 = (s32*)0x00901b20;//TODO

        struct
        {
            u32* _InputIndex1 = (u32*)0x00901b1c;//TODO
            u32* _InputIndex2 = (u32*)0x00901b18;//TODO
            u32* _Text = (u32*)0x00901ac8;//TODO [MAX_INPUT_TEXT_LENGTH]
        } Text;
    };

    extern InputContainer InputState;

    void ReleaseUnknownInput(void);
    void ReleaseMouseInput(void);
    void ReleaseInput(void);
    void HandleInputText(const u32 code);

    void InputSelectMouse(const s32 x, const s32 y);
}