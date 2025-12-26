#pragma once

#include "Basic.hxx"

#define ASCII_NULL 0

#define ASCII_HORIZONTAL_TAB 9
#define ASCII_NEW_LINE 10
#define ASCII_VERTICAL_TAB 11
#define ASCII_NEW_PAGE 12
#define ASCII_CARRIAGE_RETURN 13

#define ASCII_ESCAPE 27

#define ASCII_SPACE 32

#define ASCII_PERCENTAGE 37

#define ASCII_ASTERISK 42
#define ASCII_PLUS 43
#define ASCII_MINUS 45
#define ASCII_DOT 46
#define ASCII_SLASH 47

#define ASCII_NUMBER_ZERO 48
#define ASCII_NUMBER_ONE 49
#define ASCII_NUMBER_TWO 50
#define ASCII_NUMBER_THREE 51
#define ASCII_NUMBER_FOUR 52
#define ASCII_NUMBER_FIVE 53
#define ASCII_NUMBER_SIX 54
#define ASCII_NUMBER_SEVEN 55
#define ASCII_NUMBER_EIGHT 56
#define ASCII_NUMBER_NINE 57

#define ASCII_EQUAL 61
#define ASCII_QUESTION_MARK 63
#define ASCII_AT 64

#define ASCII_CHARACTER_UPPER_CASE_A 65
#define ASCII_CHARACTER_UPPER_CASE_B 66
#define ASCII_CHARACTER_UPPER_CASE_C 67
#define ASCII_CHARACTER_UPPER_CASE_D 68
#define ASCII_CHARACTER_UPPER_CASE_E 69
#define ASCII_CHARACTER_UPPER_CASE_F 70
#define ASCII_CHARACTER_UPPER_CASE_G 71
#define ASCII_CHARACTER_UPPER_CASE_H 72
#define ASCII_CHARACTER_UPPER_CASE_I 73
#define ASCII_CHARACTER_UPPER_CASE_L 76
#define ASCII_CHARACTER_UPPER_CASE_N 78
#define ASCII_CHARACTER_UPPER_CASE_O 79
#define ASCII_CHARACTER_UPPER_CASE_P 80
#define ASCII_CHARACTER_UPPER_CASE_S 83
#define ASCII_CHARACTER_UPPER_CASE_U 85
#define ASCII_CHARACTER_UPPER_CASE_W 87
#define ASCII_CHARACTER_UPPER_CASE_X 88

#define ASCII_SQUARE_BRACKET_OPEN 91
#define ASCII_BACK_SLASH 92
#define ASCII_SQUARE_BRACKET_CLOSE 93
#define ASCII_CARET 94
#define ASCII_UNDERSCODE 95
#define ASCII_BACK_TICK 96

#define ASCII_CHARACTER_LOWER_CASE_A 97
#define ASCII_CHARACTER_LOWER_CASE_B 98
#define ASCII_CHARACTER_LOWER_CASE_C 99
#define ASCII_CHARACTER_LOWER_CASE_D 100
#define ASCII_CHARACTER_LOWER_CASE_E 101
#define ASCII_CHARACTER_LOWER_CASE_F 102
#define ASCII_CHARACTER_LOWER_CASE_G 103
#define ASCII_CHARACTER_LOWER_CASE_H 104
#define ASCII_CHARACTER_LOWER_CASE_I 105
#define ASCII_CHARACTER_LOWER_CASE_L 108
#define ASCII_CHARACTER_LOWER_CASE_N 110
#define ASCII_CHARACTER_LOWER_CASE_O 111
#define ASCII_CHARACTER_LOWER_CASE_P 112
#define ASCII_CHARACTER_LOWER_CASE_S 115
#define ASCII_CHARACTER_LOWER_CASE_U 117
#define ASCII_CHARACTER_LOWER_CASE_X 120

#define ASCII_CURLY_BRACKET_OPEN 123
#define ASCII_PIPE 124
#define ASCII_CURLY_BRACKET_CLOSE 125
#define ASCII_TILDE 126
#define ASCII_DEL 127

// Notes: A custom implementation of functionality from <ctype.h> that.
namespace ASCII
{
    OPTIONS(Code, u8)
    {
        None = 0x0, // No significance
        Control = 0x1, // Control values such as ACK (acknowledge).
        Space = 0x2, // Actionalbe values such as TAB (tab).
        Special = 0x4, // Symbol, special character such as %.
        Symbol = 0x8, // Symbol, including invisible such as SPACE.
        Digit = 0x10, // Symbol that represents a number such as 0 .. F.
        Number = 0x20, // Numeric symbol such as 0 .. 9.
        UpperCaseLetter = 0x40, // Upper case letter.
        LowerCaseLetter = 0x80 // Lowe case letter.
    };

    template <typename T>
    inline T ToUpperCase(const T value)
    {
        if (ASCII_AT < value && value < ASCII_CURLY_BRACKET_OPEN)
        {
            return  value - ASCII_SPACE;
        }

        return value;
    }

    BOOL IsSpace(const char value);
    BOOL IsNotSpace(const char value);

    BOOL IsDigit(const char value);
    BOOL IsNotDigit(const char value);
}