#pragma once

#include "Basic.hxx"
#include "Direct.Sound.hxx"

#define INVALID_SOUND_BUFFER_POOL_INDEX 0
#define MIN_SOUND_BUFFER_POOL_INDEX 1
#define MAX_SOUND_BUFFER_POOL_SIZE 256

#define INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX 0
#define MIN_ACTIVE_SOUND_BUFFER_POOL_INDEX 1
#define MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE 31

namespace Sounds
{
    enum class SoundMixMode : u32
    {
        None = 0,
        Simple = 1, // Mixing
        Advanced = 2 // Mixing & Spatialization
    };

    struct SoundBuffer
    {
        IDirectSoundBuffer* Buffer;

        u32 ActiveIndex; // An index to an Active Sound Buffer.

        s32 Offset;
        u32 Length;

        void* Audio;
        u32 AudioSize;
    };

    struct SoundUnk0x24//TODO
    {
        IDirectSoundBuffer* Buffer;
        IDirectSound3DBuffer* AdvancedBuffer;

        s32 Unk3;//TODO
        s32 Unk4;//TODO
        s32 Unk5;//TODO
        s32 Volume;
        s32 Pan;
        s32 HZ;
        s32 Unk9;//TODO
    };

    LRESULT DSC(const LRESULT code, const char* message);
}