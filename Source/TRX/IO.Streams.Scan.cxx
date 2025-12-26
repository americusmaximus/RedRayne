#include "ASCII.hxx"
#include "IO.Streams.hxx"
#include "Strings.hxx"

#include <stdlib.h>

// TODO DEBUG ONLY
#include "Native.hxx"
#include "Native.Basic.hxx"
#include <stdio.h>
// TODO END DEBUG ONLY

#define NUMERAL_SYSTEM_OCTAL 8
#define NUMERAL_SYSTEM_DECIMAL 10
#define NUMERAL_SYSTEM_HEXADECIMAL 16

#define MAX_PATTERN_STRING_ARGUMENT_COUNT 127
#define MAX_PATTERN_STRING_ARGUMENT_BUFFER_SIZE (MAX_PATTERN_STRING_ARGUMENT_COUNT + 1)

#define MAX_REAL_VALUE_ARGUMENT_LENGTH 99
#define MAX_REAL_VALUE_ARGUMENT_BUFFER_SIZE (MAX_REAL_VALUE_ARGUMENT_LENGTH + 1)

using namespace ASCII;
using namespace Strings;

namespace IO::Streams
{
    enum class TypeLength : s32
    {
        Quarter = -2,
        Half = -1,
        Default = 0,
        Double = 1,
        Quadruple = 2
    };

    enum class Sign : s32
    {
        None = -1,
        Plus = 0,
        Minus = 1
    };

    enum class PatternMode : u32
    {
        Exclusion = 0,
        Inclusion = 1
    };

    struct ParseIntegerRequest
    {
        const u32 ValueLength;
        const Sign Sign;
        const u32 NumeralSystem;
        const BOOL Suppressed;
        const TypeLength TypeLength;
        va_list* VarArgs;
    };

    struct ParsePatternRequest
    {
        const char* Format;
        u32* FormatIndex;
        const u32 ValueLength;
        const BOOL Suppressed;
        va_list* VarArgs;
    };

    struct ParseStringRequest
    {
        const u32 ValueLength;
        const BOOL Suppressed;
        const BOOL AppendNull;
        const PatternMode PatterMode;
        const char* Pattern;
        va_list* VarArgs;
    };

    struct ParseRealRequest
    {
        const u32 ValueLength;
        const Sign Sign;
        const BOOL Suppressed;
        const TypeLength TypeLength;
        va_list* VarArgs;
    };

    TypeLength ParseTypeLength(const char* format, const int formatIndx)
    {
        auto modifier = TypeLength::Default;

        // Examples: %d %i %hhd %lu &lf
        if (formatIndx > 0)
        {
            auto v = format[formatIndx - 1];

            if (v == ASCII_CHARACTER_UPPER_CASE_H || v == ASCII_CHARACTER_LOWER_CASE_H)
            {
                modifier = TypeLength::Half;
            }
            else if (v == ASCII_CHARACTER_UPPER_CASE_L || v == ASCII_CHARACTER_LOWER_CASE_L)
            {
                modifier = TypeLength::Double;
            }

            if (formatIndx > 1 && modifier != TypeLength::Default)
            {
                v = format[formatIndx - 2];

                if (modifier == TypeLength::Half && (v == ASCII_CHARACTER_UPPER_CASE_H || v == ASCII_CHARACTER_LOWER_CASE_H))
                {
                    modifier = TypeLength::Quarter;
                }
                else if (modifier == TypeLength::Double && (v == ASCII_CHARACTER_UPPER_CASE_L || v == ASCII_CHARACTER_LOWER_CASE_L))
                {
                    modifier = TypeLength::Quadruple;
                }
            }
        }

        return modifier;
    }

    Sign ParseSign(AbstractInStream* stream, u32* valueLength)
    {
        auto sign = Sign::Plus;

        const auto value = stream->Self->Read(stream);

        if (value < STREAM_RESULT_SUCCESS) { return Sign::None; }

        if (value == ASCII_PLUS || value == ASCII_MINUS)
        {
            if (value == ASCII_MINUS)
            {
                sign = Sign::Minus;
            }

            *valueLength = *valueLength - 1; // Modify length for the sign.
        }
        else
        {
            if (!stream->Self->PutBack(stream, value)) { return Sign::None; }
        }

        return sign;
    }

    void AssignInteger(const s64 value, TypeLength length, va_list* varargs)
    {
        // NOTE: No need to handle unsigned types. It will be handler as data beind reinterpreted by consumer.
        switch (length)
        {
        case TypeLength::Quarter:
        {
            s8* result = va_arg(*varargs, s8*);
            *result = (s8)value;
            break;
        }
        case TypeLength::Half:
        {
            s16* result = va_arg(*varargs, s16*);
            *result = (s16)value;
            break;
        }
        case TypeLength::Default:
        case TypeLength::Double:
        {
            s32* result = va_arg(*varargs, s32*);
            *result = (s32)value;
            break;
        }
        case TypeLength::Quadruple:
        {
            s64* result = va_arg(*varargs, s64*);
            *result = (s64)value;
            break;
        }
        }
    }

    void AssignReal(const double value, TypeLength length, va_list* varargs)
    {
        switch (length)
        {
        case TypeLength::Quarter:
        case TypeLength::Half:
        case TypeLength::Default:
        {
            float* result = va_arg(*varargs, float*);
            *result = (float)value;
            break;
        }
        case TypeLength::Double:
        case TypeLength::Quadruple:
        {
            double* result = va_arg(*varargs, double*);
            *result = value;
            break;
        }
        }
    }

    s32 ParseInteger(AbstractInStream* stream, ParseIntegerRequest* request)
    {
        s64 result = 0;
        u32 digits = 0;

        for (u32 x = 0; x < request->ValueLength; x++)
        {
            const auto value = stream->Self->Read(stream);

            if (STREAM_RESULT_END_OF_DATA < value)
            {
                u32 digit = 99; // Assign value larger then the number base.

                switch (value)
                {
                case ASCII_NUMBER_ZERO: { digit = 0; break; }
                case ASCII_NUMBER_ONE: { digit = 1; break; }
                case ASCII_NUMBER_TWO: { digit = 2; break; }
                case ASCII_NUMBER_THREE: { digit = 3; break; }
                case ASCII_NUMBER_FOUR: { digit = 4; break; }
                case ASCII_NUMBER_FIVE: { digit = 5; break; }
                case ASCII_NUMBER_SIX: { digit = 6; break; }
                case ASCII_NUMBER_SEVEN: { digit = 7; break; }
                case ASCII_NUMBER_EIGHT: { digit = 8; break; }
                case ASCII_NUMBER_NINE: { digit = 9; break; }
                case ASCII_CHARACTER_UPPER_CASE_A:
                case ASCII_CHARACTER_LOWER_CASE_A: { digit = 10; break; }
                case ASCII_CHARACTER_UPPER_CASE_B:
                case ASCII_CHARACTER_LOWER_CASE_B: { digit = 11; break; }
                case ASCII_CHARACTER_UPPER_CASE_C:
                case ASCII_CHARACTER_LOWER_CASE_C: { digit = 12; break; }
                case ASCII_CHARACTER_UPPER_CASE_D:
                case ASCII_CHARACTER_LOWER_CASE_D: { digit = 13; break; }
                case ASCII_CHARACTER_UPPER_CASE_E:
                case ASCII_CHARACTER_LOWER_CASE_E: { digit = 14; break; }
                case ASCII_CHARACTER_UPPER_CASE_F:
                case ASCII_CHARACTER_LOWER_CASE_F: { digit = 15; break; }
                }

                // Check if the value was a number within the numeral system.
                if (request->NumeralSystem <= digit)
                {
                    if (!stream->Self->PutBack(stream, value)) { return STREAM_RESULT_FAILURE; }

                    // Unable to find a single digit.
                    if (digits == 0) { return STREAM_RESULT_FAILURE; }

                    break;
                }

                digits = digits + 1;

                result = result * request->NumeralSystem + digit;
            }
        }

        if (request->Sign == Sign::Minus)
        {
            result = -result;
        }

        //// TODO DEBUG ONLY
        //char db[32] = { 0 };
        //sprintf_s(db, 32, "%lld", result);
        //OutputDebugStringA(db);
        //OutputDebugStringA("\r\n");
        //// TODO END DEBUG ONLY

        if (!request->Suppressed)
        {
            AssignInteger(result, request->TypeLength, request->VarArgs);
            return TRUE;
        }

        return FALSE;
    }

    inline BOOL IsMatch(const u8 v1, const u8 v2, const PatternMode mode)
    {
        if (mode == PatternMode::Inclusion)
        {
            return v1 != v2;
        }

        return v1 == v2;
    }

    s32 ParseString(AbstractInStream* stream, ParseStringRequest* request)
    {
        u32 resultIndx = 0;
        char* result = request->Suppressed ? NULL : va_arg(*request->VarArgs, char*);

        for (u32 x = 0; x < request->ValueLength; x++)
        {
            const auto value = stream->Self->Read(stream);

            if (value < STREAM_RESULT_SUCCESS) { return value; }

            // Check inclusion/exclusion list.
            if (request->Pattern != NULL)
            {
                u32 patternIndx = 0;
                BOOL match = FALSE;

                while (request->Pattern[patternIndx] != NULL)
                {
                    if (IsMatch((u8)value, request->Pattern[patternIndx], request->PatterMode))
                    {
                        match = TRUE;

                        if (!stream->Self->PutBack(stream, value)) { return STREAM_RESULT_FAILURE; }

                        break;
                    }

                    patternIndx = patternIndx + 1;
                }

                if (match)
                {
                    if (result != NULL && request->AppendNull)
                    {
                        result[resultIndx] = NULL;
                    }

                    return request->Suppressed ? FALSE : TRUE;
                }
            }
            else if (IsSpace(value))
            {
                if (!stream->Self->PutBack(stream, value)) { return STREAM_RESULT_FAILURE; }

                break;
            }

            //// TODO DEBUG ONLY
            //char db[2] = { (char)value, 0 };
            //OutputDebugStringA(db);
            //// TODO END DEBUG ONLY

            if (result != NULL)
            {
                result[resultIndx] = value;
            }

            resultIndx = resultIndx + 1;
        }

        //// TODO DEBUG ONLY
        //OutputDebugStringA("\r\n");
        //// TODO END DEBUG ONLY

        if (result != NULL && request->AppendNull)
        {
            result[resultIndx] = NULL;
        }

        return request->Suppressed ? FALSE : TRUE;
    }

    s32 ParsePattern(AbstractInStream* stream, ParsePatternRequest* request)
    {
        u32 patternIndx = 0;
        PatternMode mode = PatternMode::Exclusion;

        char pattern[MAX_PATTERN_STRING_ARGUMENT_BUFFER_SIZE];

        ZeroMemory(pattern, MAX_PATTERN_STRING_ARGUMENT_BUFFER_SIZE);

        *request->FormatIndex = *request->FormatIndex + 1;

        if (request->Format[*request->FormatIndex] != ASCII_CARET)
        {
            mode = PatternMode::Inclusion;
        }
        else
        {
            *request->FormatIndex = *request->FormatIndex + 1;
        }

        if (request->Format[*request->FormatIndex] == ASCII_SQUARE_BRACKET_CLOSE)
        {
            pattern[patternIndx] = ASCII_SQUARE_BRACKET_CLOSE;
            patternIndx = patternIndx + 1;
            *request->FormatIndex = *request->FormatIndex + 1;
        }

        while (request->Format[*request->FormatIndex] != NULL
            && request->Format[*request->FormatIndex] != ASCII_SQUARE_BRACKET_CLOSE
            && patternIndx < MAX_PATTERN_STRING_ARGUMENT_COUNT)
        {
            if (request->Format[*request->FormatIndex] == ASCII_MINUS)
            {
                *request->FormatIndex = *request->FormatIndex + 1;

                if (request->Format[*request->FormatIndex] == ASCII_SQUARE_BRACKET_CLOSE)
                {
                    pattern[patternIndx] = request->Format[*request->FormatIndex];
                    patternIndx = patternIndx + 1;
                }
                else if (request->Format[*request->FormatIndex + 1] != NULL)
                {
                    const u8 start = pattern[patternIndx - 1];
                    const u8 end = request->Format[*request->FormatIndex];

                    for (u8 x = start + 1; x < end + 1; x++)
                    {
                        pattern[patternIndx] = x;
                        patternIndx = patternIndx + 1;
                    }
                }
            }
            else
            {
                pattern[patternIndx] = request->Format[*request->FormatIndex];
                patternIndx = patternIndx + 1;
                *request->FormatIndex = *request->FormatIndex + 1;
            }
        }

        while (request->Format[*request->FormatIndex] != NULL
            && request->Format[*request->FormatIndex] != ASCII_SQUARE_BRACKET_CLOSE)
        {
            *request->FormatIndex = *request->FormatIndex + 1;
        }

        ParseStringRequest parseRequest =
        {
            .ValueLength = request->ValueLength,
            .Suppressed = request->Suppressed,
            .AppendNull = TRUE,
            .PatterMode = mode,
            .Pattern = pattern,
            .VarArgs = request->VarArgs
        };

        return ParseString(stream, &parseRequest);
    }

    s32 ParseReal(AbstractInStream* stream, ParseRealRequest* request)
    {
        u32 indx = 0;
        char buffer[MAX_REAL_VALUE_ARGUMENT_BUFFER_SIZE];

        ZeroMemory(buffer, MAX_REAL_VALUE_ARGUMENT_BUFFER_SIZE);

        s32 dotCount = 0; // Count of '.' found during parsing.
        s32 eCount = 0; // Count of 'E' or 'e' found during parsing.
        s32 eIndex = -1; // Index of the last 'E' or 'e' found during parsing.
        s32 signCount = 0; // Count of '-' found during parsing. For '[Ee]-X' pattern.

        if (request->Sign == Sign::Minus)
        {
            buffer[indx] = ASCII_MINUS;
            indx = indx + 1;
        }

        for (u32 x = 0; x < request->ValueLength; x++)
        {
            const auto value = stream->Self->Read(stream);

            if (STREAM_RESULT_END_OF_DATA < value)
            {
                if (MAX_REAL_VALUE_ARGUMENT_LENGTH < indx)
                {
                    if (!stream->Self->PutBack(stream, value)) { return STREAM_RESULT_FAILURE; }
                    break;
                }

                if (IsDigit(value) || value == ASCII_DOT || value == ASCII_PLUS || value == ASCII_MINUS
                    || value == ASCII_CHARACTER_UPPER_CASE_E || value == ASCII_CHARACTER_LOWER_CASE_E)
                {
                    if (value == ASCII_DOT)
                    {
                        dotCount = dotCount + 1;

                        if (dotCount != 1)
                        {
                            if (!stream->Self->PutBack(stream, value)) { return STREAM_RESULT_FAILURE; }

                            break;
                        }
                    }
                    else if (value == ASCII_CHARACTER_UPPER_CASE_E || value == ASCII_CHARACTER_LOWER_CASE_E)
                    {
                        // Check for sequential 'E's or 'e's.
                        if (eCount != 0 && (eIndex + 1 == x)) { return STREAM_RESULT_FAILURE; }

                        eCount = eCount + 1;
                        eIndex = x;

                        if (eCount != 1)
                        {
                            if (!stream->Self->PutBack(stream, value)) { return STREAM_RESULT_FAILURE; }

                            break;
                        }
                    }
                    else if (value == ASCII_PLUS || value == ASCII_MINUS)
                    {
                        // No "E" or "e" found previously, therefore stop.
                        if (eCount != 1)
                        {
                            if (!stream->Self->PutBack(stream, value)) { return STREAM_RESULT_FAILURE; }

                            break;
                        }
                        // Check if index of 'E' or 'e' is the previous character.
                        else if (eIndex + 1 != x) { return STREAM_RESULT_FAILURE; }

                        signCount = signCount + 1;

                        if (signCount != 1) { return STREAM_RESULT_FAILURE; }
                    }

                    buffer[indx] = value;
                    indx = indx + 1;
                }
                else
                {
                    if (!stream->Self->PutBack(stream, value)) { return STREAM_RESULT_FAILURE; }
                    break;
                }
            }
        }

        //// TODO DEBUG ONLY
        //OutputDebugStringA(realValueBuffer);
        //OutputDebugStringA("\r\n");
        //// TODO END DEBUG ONLY

        const double result = strtod(buffer, NULL);

        if (!request->Suppressed)
        {
            AssignReal(result, request->TypeLength, request->VarArgs);
            return TRUE;
        }

        return FALSE;
    }

    s32 Scan(AbstractInStream* stream, const char* format, va_list varargs)
    {
        u32 parsedValueCount = 0;

        if (IsNullOrEmpty(format)) { return parsedValueCount; }

        u32 formatIndx = 0;

        while (format[formatIndx] != NULL)
        {
            // Skip the line of the data because the format is requesting for the line change.
            if (format[formatIndx] == ASCII_NEW_LINE)
            {
                const s32 value = AbstractStreamSkipLine(stream, 1);

                if (STREAM_RESULT_END_OF_DATA < value)
                {
                    formatIndx = formatIndx + 1;

                    if (format[formatIndx] == NULL) { return parsedValueCount; }

                    continue;
                }

                return STREAM_RESULT_END_OF_DATA;
            }

            // Skip actionable characters like TAB, SPACE, etc.
            if (IsSpace(format[formatIndx]))
            {
                const s32 value = AbstractInStreamIsSpace(stream);

                if (STREAM_RESULT_END_OF_DATA < value)
                {
                    formatIndx = formatIndx + 1;

                    if (format[formatIndx] == NULL) { return parsedValueCount; }

                    continue;
                }

                return STREAM_RESULT_END_OF_DATA;
            }

            // Skip normal characters before % character.
            if (format[formatIndx] != ASCII_PERCENTAGE || format[formatIndx + 1] == ASCII_PERCENTAGE)
            {
                // If a percent sign (%) is followed by a character that has no meaning as a format control character,
                // that character and following characters up to the next percent sign are treated as an ordinary sequence of characters;
                // that is, a sequence of characters that must match the input. For example, to specify a percent-sign character, use %%.

                const auto value = stream->Self->Read(stream);

                if (value == format[formatIndx])
                {
                    formatIndx = formatIndx + 1;

                    if (format[formatIndx] == NULL) { return parsedValueCount; }

                    // The % character isn't reached yet, skip expensive checks.
                    continue;
                }

                if (STREAM_RESULT_END_OF_DATA < value || !stream->Self->PutBack(stream, value)) { return STREAM_RESULT_END_OF_DATA; }

                break;
            }

            // Examples: %*d
            BOOL suppressed = FALSE;

            // Examples: %7d %19hd %10s
            u32 valueLength = U32_MAX;

            while (format[formatIndx] != NULL)
            {
                if (format[formatIndx] != ASCII_PERCENTAGE)
                {
                    if (format[formatIndx] == ASCII_ASTERISK)
                    {
                        suppressed = TRUE;
                    }
                    else if (format[formatIndx] != ASCII_CHARACTER_UPPER_CASE_L
                        && format[formatIndx] != ASCII_CHARACTER_LOWER_CASE_L
                        && format[formatIndx] != ASCII_CHARACTER_UPPER_CASE_H
                        && format[formatIndx] != ASCII_CHARACTER_LOWER_CASE_H)
                    {
                        // Check if not a number.
                        if (IsNotDigit(format[formatIndx])) { break; }

                        // Convert character to integer.
                        const auto digit = format[formatIndx] - ASCII_NUMBER_ZERO;

                        // Value length cannot be zero.
                        valueLength = (valueLength == U32_MAX && digit != 0) ? digit : (valueLength * 10 + digit);
                    }
                }

                formatIndx = formatIndx + 1;
            }

            switch (format[formatIndx])
            {
            case ASCII_CHARACTER_UPPER_CASE_D:
            case ASCII_CHARACTER_LOWER_CASE_D:
            {
                if (STREAM_RESULT_END_OF_DATA < AbstractInStreamIsSpace(stream))
                {
                    const auto sign = ParseSign(stream, &valueLength);

                    if (sign == Sign::None) { return STREAM_RESULT_FAILURE; }

                    const auto length = ParseTypeLength(format, formatIndx);

                    ParseIntegerRequest request =
                    {
                        .ValueLength = valueLength,
                        .Sign = sign,
                        .NumeralSystem = NUMERAL_SYSTEM_DECIMAL,
                        .Suppressed = suppressed,
                        .TypeLength = length,
                        .VarArgs = &varargs
                    };

                    const auto result = ParseInteger(stream, &request);

                    if (result < STREAM_RESULT_SUCCESS) { return STREAM_RESULT_FAILURE; }

                    parsedValueCount = parsedValueCount + result;
                }

                break;
            }
            /*case ASCII_CHARACTER_UPPER_CASE_O:
            case ASCII_CHARACTER_LOWER_CASE_O:
            {
                const auto nac = SpaceCount(buffer, bufferIndx);

                if (-1 < nac)
                {
                    bufferIndx = bufferIndx + nac;
                }

                const auto isNegative = ParseNumberSign(buffer, &bufferIndx, &valueLength);
                const auto modifier = ParseValueLengthModifier(format, formatIndx);

                ParseIntegerNumber(format, formatIndx, buffer, &bufferIndx, valueLength, isNegative, NUMERAL_SYSTEM_OCTAL, isSuppressed, modifier, &varargs);

                // TODO: Error handling

                break;
            }
            case ASCII_CHARACTER_UPPER_CASE_U:
            case ASCII_CHARACTER_LOWER_CASE_U:
            {
                const auto nac = SpaceCount(buffer, bufferIndx);

                if (-1 < nac)
                {
                    bufferIndx = bufferIndx + nac;
                }

                const auto modifier = ParseValueLengthModifier(format, formatIndx);

                ParseIntegerNumber(format, formatIndx, buffer, &bufferIndx, valueLength, FALSE, NUMERAL_SYSTEM_DECIMAL, isSuppressed, modifier, &varargs);

                // TODO: Error handling


                break;
            }
            case ASCII_CHARACTER_UPPER_CASE_P:
            case ASCII_CHARACTER_LOWER_CASE_P:
            {
                const auto nac = SpaceCount(buffer, bufferIndx);

                if (-1 < nac)
                {
                    bufferIndx = bufferIndx + nac;
                }

                const auto modifier = ValueLengthModifier::None; // TODO: 64bit system.

                ParseIntegerNumber(format, formatIndx, buffer, &bufferIndx, valueLength, FALSE, NUMERAL_SYSTEM_DECIMAL, isSuppressed, modifier, &varargs);

                // TODO: Error handling


                break;
            }
            case ASCII_CHARACTER_UPPER_CASE_I:
            case ASCII_CHARACTER_LOWER_CASE_I:
            {
                const auto nac = SpaceCount(buffer, bufferIndx);

                if (-1 < nac)
                {
                    bufferIndx = bufferIndx + nac;
                }

                const auto isNegative = ParseNumberSign(buffer, &bufferIndx, &valueLength);
                const auto base = ParseNumeralSystem(buffer, &bufferIndx);
                const auto modifier = ParseValueLengthModifier(format, formatIndx);

                ParseIntegerNumber(format, formatIndx, buffer, &bufferIndx, valueLength, isNegative, base, isSuppressed, modifier, &varargs);

                // TODO: Error handling


                break;
            }
            case ASCII_CHARACTER_UPPER_CASE_X:
            case ASCII_CHARACTER_LOWER_CASE_X:
            {
                const auto nac = SpaceCount(buffer, bufferIndx);

                if (-1 < nac)
                {
                    bufferIndx = bufferIndx + nac;
                }

                const auto isNegative = ParseNumberSign(buffer, &bufferIndx, &valueLength);
                const auto modifier = ParseValueLengthModifier(format, formatIndx);

                ParseIntegerNumber(format, formatIndx, buffer, &bufferIndx, valueLength, isNegative, NUMERAL_SYSTEM_HEXADECIMAL, isSuppressed, modifier, &varargs);

                // TODO: Error handling

                break;
            }
            case ASCII_CHARACTER_UPPER_CASE_N:
            case ASCII_CHARACTER_LOWER_CASE_N:
            {
                const auto modifier = ParseValueLengthModifier(format, formatIndx);

                if (isSuppressed == 0)
                {
                    AssignIntegerValue(bufferIndx, modifier, &varargs);
                }

                break;
            }*/
            case ASCII_CHARACTER_UPPER_CASE_S:
            case ASCII_CHARACTER_LOWER_CASE_S:
            {
                if (STREAM_RESULT_END_OF_DATA < AbstractInStreamIsSpace(stream))
                {
                    ParseStringRequest request =
                    {
                        .ValueLength = valueLength,
                        .Suppressed = suppressed,
                        .AppendNull = TRUE,
                        .PatterMode = PatternMode::Inclusion,
                        .Pattern = NULL,
                        .VarArgs = &varargs
                    };

                    const auto result = ParseString(stream, &request);

                    if (result < STREAM_RESULT_SUCCESS) { return STREAM_RESULT_FAILURE; }

                    parsedValueCount = parsedValueCount + result;
                }

                break;
            }
            /*case ASCII_CHARACTER_UPPER_CASE_C:
            case ASCII_CHARACTER_LOWER_CASE_C:
            {
                if (valueLength == INT_MAX)
                {
                    valueLength = 1;
                }

                const auto nac = SpaceCount(buffer, bufferIndx);

                if (-1 < nac)
                {
                    bufferIndx = bufferIndx + nac;
                }

                ParsePlainString(buffer, &bufferIndx, valueLength, isSuppressed, FALSE, &varargs);

                break;
            }*/
            case ASCII_SQUARE_BRACKET_OPEN:
            {
                if (STREAM_RESULT_END_OF_DATA < AbstractInStreamIsSpace(stream))
                {
                    ParsePatternRequest request =
                    {
                        .Format = format,
                        .FormatIndex = &formatIndx,
                        .ValueLength = valueLength,
                        .Suppressed = suppressed,
                        .VarArgs = &varargs
                    };

                    const auto result = ParsePattern(stream, &request);

                    if (result < STREAM_RESULT_SUCCESS) { return STREAM_RESULT_FAILURE; }

                    parsedValueCount = parsedValueCount + result;
                }

                break;
            }
            case ASCII_CHARACTER_UPPER_CASE_A:
            case ASCII_CHARACTER_LOWER_CASE_A:
            case ASCII_CHARACTER_UPPER_CASE_E:
            case ASCII_CHARACTER_LOWER_CASE_E:
            case ASCII_CHARACTER_UPPER_CASE_F:
            case ASCII_CHARACTER_LOWER_CASE_F:
            case ASCII_CHARACTER_UPPER_CASE_G:
            case ASCII_CHARACTER_LOWER_CASE_G:
            {
                if (STREAM_RESULT_END_OF_DATA < AbstractInStreamIsSpace(stream))
                {
                    const auto sign = ParseSign(stream, &valueLength);

                    if (sign == Sign::None) { return STREAM_RESULT_FAILURE; }

                    const auto length = ParseTypeLength(format, formatIndx);

                    ParseRealRequest request =
                    {
                        .ValueLength = valueLength,
                        .Sign = sign,
                        .Suppressed = suppressed,
                        .TypeLength = length,
                        .VarArgs = &varargs
                    };

                    const auto result = ParseReal(stream, &request);

                    if (result < STREAM_RESULT_SUCCESS) { return STREAM_RESULT_FAILURE; }

                    parsedValueCount = parsedValueCount + result;
                }

                break;
            }
            default:
            {
                // TODO
                OutputDebugStringA("TODO: NOT IMPLEMENTED!\r\n");
                Native::Halt();
                // END TODO
                break;
            }
            }

            formatIndx = formatIndx + 1;
        }

        return parsedValueCount;
    }

    s32 AbstractInStreamScan(AbstractInStream* self, const char* format, ...)
    {
        //// TODO DEBUG ONLY
        //OutputDebugStringA("\r\nFormat: ");
        //OutputDebugStringA(format);
        //OutputDebugStringA("\r\n");
        //// TODO END DEBUG ONLY

        va_list vargs;
        va_start(vargs, format);

        const auto result = Scan(self, format, vargs);

        va_end(vargs);

        return result;
    }
}