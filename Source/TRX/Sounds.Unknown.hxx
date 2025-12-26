#pragma once

#include "Basic.hxx"
#include "Mathematics.Basic.hxx"

namespace Sounds
{
    struct UnknownSoundObject
    {
        // TODO
    };


    struct UnknownSoundContainer
    {
        UnknownSoundObject** _UnknownSoundObjectPointer = (UnknownSoundObject**)0x00625e60;//TODO
        UnknownSoundObject* _UnknownSoundObject = (UnknownSoundObject*)0x008fca14;//TODO

        s32* _UnknownValue1 = (s32*)0x008fd4a4; // TODO

        s32* _UnknownValue401 = (s32*)0x008c5cf0;//TODO
    };

    extern UnknownSoundContainer UnknownSoundState;

    void InitializeSounds1(UnknownSoundObject* self);

    f32 AcquireSoundUnknownValue1(UnknownSoundObject* self, const char* name);

    void AcquireSoundSample3(UnknownSoundObject* self, char* name, const char* value);
    void AcquireSoundSample4(UnknownSoundObject* self, const char* name);

    void SelectSoundEffectChannelStates1(UnknownSoundObject* self);
    void SelectSoundEffectChannelStates2(UnknownSoundObject* self);

    void InitializeSound2(UnknownSoundObject* self, const char* file);

    void StopSound3(UnknownSoundObject* self);


    void InitializeSound3(UnknownSoundObject* self, char* tag, const char* file);
    void InitializeSound4(UnknownSoundObject* self, char* tag, const char* file, const f32x3* values);
    void InitializeSound5(UnknownSoundObject* self, char* tag, const char* file, f32* values);
    u32 InitializeSound6(UnknownSoundObject* self, char* tag, const char* file, const f32x3* values, const f32 delay);
    u32 InitializeSound7(UnknownSoundObject* self, char* tag, const char* file, f32* values, const f32 delay);

    u32 InitializeSoundEffect4(char* tag, const char* file, const f32 x, const f32 y, const f32 z, f32* values, const s32 unk30); // TODO names

    BOOL ReleaseSoundEffect3(UnknownSoundObject* self, const u32 hash);
    void ReleaseSoundEffect4(UnknownSoundObject* self, const char* tag);

}