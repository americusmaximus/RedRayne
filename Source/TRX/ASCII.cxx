#include "ASCII.hxx"

#define ASCII_CODE_TABLE_SIZE 260

namespace ASCII
{
    // A lookup table of character significance within the ASCII table.
    const Code CodeTable[ASCII_CODE_TABLE_SIZE] =
    {
        // NULL
        Code::Control,
        //SOH
        Code::Control,
        // STX
        Code::Control,
        // ETX
        Code::Control,
        // EOT
        Code::Control,
        // ENQ
        Code::Control,
        // ACK
        Code::Control,
        // BEL BS TAB LF
        Code::Control, Code::Control, Code::Control | Code::Space, Code::Control | Code::Space,
        // VT FF CR S0
        Code::Control | Code::Space, Code::Control | Code::Space, Code::Control | Code::Space, Code::Control,
        // S1 DLE DC1 DC2
        Code::Control, Code::Control, Code::Control, Code::Control,
        // DC3 DC4 NAK SYN
        Code::Control, Code::Control, Code::Control, Code::Control,
        // ETB CAN EM SUB
        Code::Control, Code::Control, Code::Control, Code::Control,
        // ESC FS GS RS
        Code::Control, Code::Control, Code::Control, Code::Control,
        // US SPACE ! "
        Code::Control, Code::Space | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol,
        // # $ % &
        Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol,
        // ' ( ) *
        Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol,
        // + , - .
        Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol,
        // / 0 1 2
        Code::Special | Code::Symbol, Code::Symbol | Code::Digit | Code::Number, Code::Symbol | Code::Digit | Code::Number, Code::Symbol | Code::Digit | Code::Number,
        // 3 4 5 6
        Code::Symbol | Code::Digit | Code::Number, Code::Symbol | Code::Digit | Code::Number, Code::Symbol | Code::Digit | Code::Number, Code::Symbol | Code::Digit | Code::Number,
        // 7 8 9 :
        Code::Symbol | Code::Digit | Code::Number, Code::Symbol | Code::Digit | Code::Number, Code::Symbol | Code::Digit | Code::Number, Code::Special | Code::Symbol,
        // ; < = >
        Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol,
        // ? @ A B
        Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Symbol | Code::Digit | Code::UpperCaseLetter, Code::Symbol | Code::Digit | Code::UpperCaseLetter,
        // C D E F
        Code::Symbol | Code::Digit | Code::UpperCaseLetter, Code::Symbol | Code::Digit | Code::UpperCaseLetter, Code::Symbol | Code::Digit | Code::UpperCaseLetter, Code::Symbol | Code::Digit | Code::UpperCaseLetter,
        // G H I J
        Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter,
        // K L M N
        Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter,
        // O P Q R
        Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter,
        // S T U V
        Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter,
        // W X Y Z
        Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter, Code::Symbol | Code::UpperCaseLetter,
        // [ \ ] ^
        Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol,
        // _ ` a b
        Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Symbol | Code::Digit | Code::LowerCaseLetter, Code::Symbol | Code::Digit | Code::LowerCaseLetter,
        // c d e f
        Code::Symbol | Code::Digit | Code::LowerCaseLetter, Code::Symbol | Code::Digit | Code::LowerCaseLetter, Code::Symbol | Code::Digit | Code::LowerCaseLetter, Code::Symbol | Code::Digit | Code::LowerCaseLetter,
        //g h i j
        Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter,
        // k l m n
        Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter,
        // o p q r
        Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter,
        // s t u v
        Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter,
        // w x y z
        Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter, Code::Symbol | Code::LowerCaseLetter,
        // { | } ~
        Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol, Code::Special | Code::Symbol,
        // DEL
        Code::Control,
        // Values 128+ not supported.
        Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None,
        Code::None, Code::None, Code::None, Code::None
    };

    BOOL IsSpace(const char value)
    {
        return (CodeTable[value] & Code::Space) != Code::None;
    }

    BOOL IsNotSpace(const char value)
    {
        return (CodeTable[value] & Code::Space) == Code::None;
    }

    BOOL IsDigit(const char value)
    {
        return (CodeTable[value] & Code::Number) != Code::None;
    }

    BOOL IsNotDigit(const char value)
    {
        return (CodeTable[value] & Code::Number) == Code::None;
    }
}