#pragma once

#include "Basic.hxx"
#include "Native.Basic.hxx"

namespace Native
{
    enum class ThreadWeight : u32
    {
        Low = 0,
        Normal = 1,
        High = 2,
        Max = 3
    };

    void __declspec(noreturn) Halt();
    BOOL IsDebuggerActive(void);
    BOOL ReleaseModule(const HMODULE module);
    BOOL ReleaseMutex1(const HANDLE mutex);
    BOOL SelectThreadWeight(const HANDLE thread, const ThreadWeight weight);
    BOOL WaitMutex(const HANDLE mutex);
    DWORD ExecuteProcess(const char* command, const u8 mode);
    FARPROC AcquireLambda(const HMODULE module, const char* name);
    HANDLE InitializeMutex(void);
    HANDLE InitializeThread(const void* lambda, void* params);
    HMODULE InitializeModule(const char* name);
    void Wait(const f64 time);
}