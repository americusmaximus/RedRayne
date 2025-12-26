#pragma once

#include "Basic.hxx"

#define MAX_LOCALIZATION_KEY_NAME_BUFFER_LENGTH 200

namespace Localization
{
    struct LocalizationContainer
    {
        char KeyNameBuffer[MAX_LOCALIZATION_KEY_NAME_BUFFER_LENGTH];
    };

    extern LocalizationContainer LocalizationState;

    const char* LocalizeKeyName1(const u32 code);
    const char* LocalizeKeyName2(const u32 code);
}