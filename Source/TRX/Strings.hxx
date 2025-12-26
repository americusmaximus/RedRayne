#pragma once

#include "Native.Basic.hxx"

namespace Strings
{
    inline BOOL IsNull(const char* str)
    {
        return (str == NULL);
    }

    inline BOOL IsNotNull(const char* str)
    {
        return (str != NULL);
    }

    inline BOOL IsNullOrEmpty(const char* str)
    {
        if (str == NULL) { return TRUE; }

        if (str[0] == NULL) { return TRUE; }

        return FALSE;
    }

    inline BOOL IsNotNullOrEmpty(const char* str)
    {
        return (str != NULL && str[0] != NULL);
    }

    BOOL EqualStrings(const char* s1, const char* s2);
    BOOL StartsWith(const char* str, const char* val);
}