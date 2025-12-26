#include "Native.hxx"

#include <math.h>

#define CREATE_PROCESS_FAILURE 0
#define CREATE_PROCESS_OPTION_NONE 0
#define CREATE_THREAD_OPTION_NONE 0

#define EXECUTE_PROCESS_FAILURE -1

namespace Native
{
    void __declspec(noreturn) Halt()
    {
        __debugbreak();
    }

    BOOL IsDebuggerActive(void)
    {
        return IsDebuggerPresent();
    }

    HMODULE InitializeModule(const char* name)
    {
        return LoadLibraryA(name);
    }

    BOOL ReleaseModule(const HMODULE module)
    {
        return FreeLibrary(module);
    }

    FARPROC AcquireLambda(const HMODULE module, const char* name)
    {
        return GetProcAddress(module, name);
    }

    // TODO: Change interface from u8 to Window mode
    DWORD ExecuteProcess(const char* command, const u8 mode)
    {
        DWORD pcf = mode ? CREATE_PROCESS_OPTION_NONE : CREATE_NO_WINDOW;

        PROCESS_INFORMATION info;

        ZeroMemory(&info, sizeof(PROCESS_INFORMATION));

        STARTUPINFOA startInfo = { .cb = sizeof(STARTUPINFOA) };

        auto result = CreateProcessA(command, NULL, NULL, NULL, FALSE, pcf, NULL, NULL, &startInfo, &info);

        if (result != CREATE_PROCESS_FAILURE)
        {
            DWORD exitCode = 0;

            do
            {
                result = GetExitCodeProcess(info.hProcess, &exitCode);

                if (exitCode != STILL_ACTIVE) { return exitCode; }

                Sleep(0);

            } while (result);
        }

        return EXECUTE_PROCESS_FAILURE;
    }

    HANDLE InitializeMutex(void)
    {
        return CreateMutexA(NULL, FALSE, NULL);
    }

    BOOL WaitMutex(const HANDLE mutex)
    {
        if (mutex == NULL) { return FALSE; }

        return WaitForSingleObject(mutex, MAXDWORD) == WAIT_OBJECT_0;
    }

    HANDLE InitializeThread(const void* lambda, void* params)
    {
        DWORD id;

        return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)lambda, params, CREATE_THREAD_OPTION_NONE, &id);
    }

    BOOL ReleaseMutex1(const HANDLE mutex) // TODO name
    {
        if (mutex == NULL) { return FALSE; }

        return ReleaseMutex(mutex);
    }

    void Wait(const f64 time)
    {
        Sleep(time <= 0.0 ? 0 : (DWORD)round(time * 1000.0)); // TODO constant
    }

    BOOL SelectThreadWeight(const HANDLE thread, const ThreadWeight weight)
    {
        auto priority = THREAD_PRIORITY_NORMAL;

        switch (weight)
        {
        case ThreadWeight::Low:
        {
            priority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        }
        case ThreadWeight::Normal:
        {
            priority = THREAD_PRIORITY_NORMAL;
            break;
        }
        case ThreadWeight::High:
        {
            priority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        }
        case ThreadWeight::Max:
        {
            priority = THREAD_PRIORITY_HIGHEST;
            break;
        }
        }

        return SetThreadPriority(thread, priority);
    }
}