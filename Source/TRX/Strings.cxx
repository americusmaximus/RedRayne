#pragma once

#include "Strings.hxx"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

namespace Strings
{
    BOOL EqualStrings(const char* s1, const char* s2)
    {
        return strcmpi(s1, s2) == 0;
    }

    BOOL StartsWith(const char* str, const char* val)
    {
        const auto vl = strlen(val);
        const auto sl = strlen(str);

        return sl < vl ? FALSE : strnicmp(val, str, vl) == 0;
    }
}