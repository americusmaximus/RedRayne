#include "ASCII.hxx"
#include "IO.Streams.hxx"
#include "Strings.hxx"

#include <stdio.h>

// TODO DEBUG ONLY
#include "Native.Basic.hxx"
// TODO END DEBUG ONLY

#define MAX_STREAM_FORMAT_BUFFER_SIZE 2048

using namespace ASCII;
using namespace Strings;

namespace IO::Streams
{
    s32 AbstractOutStreamFormat(AbstractOutStream* self, const char* format, ...)
    {
        //// TODO DEBUG ONLY
        //OutputDebugStringA("\r\nFormat: ");
        //OutputDebugStringA(format);
        //OutputDebugStringA("\r\n");
        //// TODO END DEBUG ONLY

        char buffer[MAX_STREAM_FORMAT_BUFFER_SIZE];

        va_list vargs;
        va_start(vargs, format);

        s32 result = vsnprintf_s(buffer, MAX_STREAM_FORMAT_BUFFER_SIZE, format, vargs);

        if (result >= 0)
        {
            //// TODO DEBUG ONLY
            //OutputDebugStringA("\r\Result: ");
            //OutputDebugStringA(buffer);
            //OutputDebugStringA("\r\n");
            //// TODO END DEBUG ONLY

            const auto length = strlen(buffer);

            if (length != 0)
            {
                result = self->Self->WriteBlock(self, (u8*)buffer, length);
            }
        }

        va_end(vargs);

        return result;
    }
}