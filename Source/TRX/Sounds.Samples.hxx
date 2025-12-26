#pragma once

#include "Assets.Sounds.hxx"
#include "Sounds.Basic.hxx"
#include "Sounds.Decoder.hxx"
#include "Objects.hxx"
#include "IO.Streams.hxx"

namespace Sounds
{
    struct SoundSampleDescriptor
    {
        Assets::Sounds::SoundSampleDefinition Definition;

        f32 ReferenceDistance;
        f32 MinimumDistance;
        f32 MaximumDistance;

        void* AllocatedMemory1;

        u32 LoopMode; // TODO Enum
        s32 ChannelLength[2]; // TODO

        s32 Unk106; // TODO
        s32 Unk107; // TODO
        s32 Unk108; // TODO
        s32 Unk6; // TODO
        s32 Unk109; // TODO
        s32 Unk110; // TODO
        s32 Unk111; // TODO
        s32 Unk112; // TODO

        Assets::Sounds::SoundCacheMode CacheControl;

        s32 Priority;
        u32 Offset;
    };

    struct SoundSample
    {
        SoundSampleDescriptor Descriptor;

        s32 Index;
        u32 BufferIndex;
        s32 StreamIndex;

        s32 Unk5; // TODO
        s32 Unk6; // TODO
        s32 Unk7; // TODO, some sort of index
        s32 Length; // TODO type
        s32 AllocatedMemorySize; // TODO better name!
        s32 Unk10; // TODO
        s32 Unk11; // TODO
        SoundDecoder* Decoder;
        s32 Unk13; // TODO
        s32 LockOffset; // TODO
        s32 LockLength; // TODO
        s32 Unk16; // TODO
    };

    SoundSample* SoundSampleInitialize(SoundSample* self);
    void* SoundSampleDispose(SoundSample* self, const Memory::DisposeMode mode);
    void SoundSampleRelease(SoundSample* self);
    void UpdateSoundSample(SoundSample* self);
    void SoundSampleRelease1(SoundSample* self);
    void SelectUnknownSoundSampleValue(SoundSample* self);
    void ReleaseSoundSample(SoundSample* self);
    void SoundSampleInitialize1(SoundSample* self, SoundSampleDescriptor* desc);
    BOOL LoadSoundSample1(SoundSample* self, IO::Streams::InStreamFile* stream);

    SoundSample* AcquireSoundSample1();

    void SoundSampleDescriptorInitialize1(SoundSampleDescriptor* self);
    void ReleaseSoundSampleDescriptor(SoundSampleDescriptor* self);
    void SelectSoundSampleDescriptorName(SoundSampleDescriptor* self, const char* name);

    BOOL AcquireSoundSampleFileDescriptor(SoundSampleDescriptor* self, IO::FileDescriptor* desc);
    void AcquireSoundSampleDescriptorDetails(const SoundSampleDescriptor* desc, u32* size, u32* packet);
    void SelectSoundSampleDescriptor1(SoundSampleDescriptor* self);
    s32 AcquireUnknownSoundSampleDescriptorValue1(const SoundSampleDescriptor* self);
    BOOL InitializeSoundSampleDescriptor1(SoundSampleDescriptor* self, char* outMessage);

    // NOTE: Loads .SFX file.
    BOOL InitializeSoundSampleDescriptor2(SoundSampleDescriptor* self, IO::Streams::InStreamFile* stream, char* outMessage);

    struct SoundSampleContainer
    {
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&SoundSampleInitialize,
            .Dispose = (Objects::ABSTRACTOBJECTINITIALIZERDISPOSE)&SoundSampleDispose,
            .Size = sizeof(SoundSample),
            .Name = "$SfxSample$$"
        }; // 0x00613e10;
    };

    extern SoundSampleContainer SoundsSampleState;
}