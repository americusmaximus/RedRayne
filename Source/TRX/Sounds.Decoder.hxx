#pragma once

#include "Memory.hxx"

namespace Sounds
{
    struct SoundDecoder
    {
        u32 HZ;
        u32 Channels;

        char Unk[0x38748];//TODO
    };

    void* SoundDecoderDispose(SoundDecoder* self, const Memory::DisposeMode mode);
}