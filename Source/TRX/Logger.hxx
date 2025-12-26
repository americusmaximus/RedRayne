#pragma once

#include "Basic.hxx"

namespace Logger
{
    extern BOOL IsLogFileOpen;

    void LogMessage(const char* format ...);
    void __declspec(noreturn) LogError(const char* format ...);
}