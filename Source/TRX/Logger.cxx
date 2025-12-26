#include "Logger.hxx"
#include "Native.Basic.hxx"
#include "Native.hxx"
#include "Strings.hxx"
#include "Memory.hxx"
#include "IO.Streams.hxx"
#include "Renderer.hxx"
#include "App.Windows.hxx"
#include "Sounds.hxx"
#include "Sounds.Unknown.hxx"
#include "Time.hxx"

#include <stdio.h>
#include <time.h>

#define MAX_LOG_MESSAGE_BUFFER_SIZE 1024
#define LOG_FILE_NAME "errorlog.txt"

using namespace Renderer;
using namespace Sounds;
using namespace App;
using namespace Native;
using namespace Strings;
using namespace Memory;
using namespace IO::Streams;
using namespace App::Windows;
using namespace Time;

namespace Logger
{
    BOOL IsLogFileOpen;

    void LogMessage(const char* format, ...)
    {
        char buffer[MAX_LOG_MESSAGE_BUFFER_SIZE];

        va_list vargs;
        va_start(vargs, format);

        vsnprintf_s(buffer, MAX_LOG_MESSAGE_BUFFER_SIZE, format, vargs);

        va_end(vargs);

        OutputDebugStringA(buffer);
    }

    void __declspec(noreturn) LogError(const char* format, ...)
    {
        if (IsLogFileOpen)
        {
            const auto file = fopen(LOG_FILE_NAME, "at");

            if (file != NULL)
            {
                fprintf(file, "Recursive Terminal Error!\n");
                fclose(file);
            }

            Halt();
        }

        IsLogFileOpen = TRUE;

        char buffer[MAX_LOG_MESSAGE_BUFFER_SIZE];

        va_list vargs;
        va_start(vargs, format);

        vsnprintf_s(buffer, MAX_LOG_MESSAGE_BUFFER_SIZE, format, vargs);

        va_end(vargs);

        OutStreamFile outStream;
        OutStreamFileInitialize(&outStream);

        if (OutStreamFileOpenFile1(&outStream, LOG_FILE_NAME, "at"))
        {
            AbstractOutStreamFormat((AbstractOutStream*)&outStream, "----------------------------------------------------------------\n");
            AbstractOutStreamFormat((AbstractOutStream*)&outStream, "Message: %s\n", buffer);

            time_t t;

            time(&t);

            AbstractOutStreamFormat((AbstractOutStream*)&outStream, "Time: %s\n", asctime(localtime(&t)));

            OutStreamFileRelease(&outStream);
        }
        
        if (IsDebuggerActive())
        {
            OutputDebugStringA("GTFO:\n");

            OutputDebugStringA(buffer);

            Halt();
        }
        else
        {
            StopSound3(*UnknownSoundState._UnknownSoundObjectPointer);

            RestoreRendererVideoMode2(TRUE);

            ReleaseAppWindow1();

            ReleaseTime();
        }

        MessageBoxA(*AppWindowState._HWND, buffer, AppWindowState.WindowName, MB_OK | MB_ICONERROR);

        IsLogFileOpen = FALSE;

        OutStreamFileDispose(&outStream, DisposeMode::None);

        if (IsDebuggerActive()) { Halt(); }

        ExitProcess(EXIT_FAILURE);
    }
}