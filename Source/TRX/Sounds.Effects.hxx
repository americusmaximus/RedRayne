#pragma once

#include "Sounds.Samples.hxx"

namespace Sounds
{
    struct SoundEffectDescriptor
    {
        s32 NextChannelIndex; // TODO, better name?

        f32x3 Location;

        f32* Unk3;  // TODO typem name
        s32 Unk4;  // TODO

        f32x3 Velocity;

        f32* Unk6;  // TODO type, name
        s32 Unk7;  // TODO
        f32 Volume;
        f32 HZ;
        f32 RemainingDelay;

        char* Tags[2]; // TODO constant

        s32 Unk30; // TODO
        s32 Unk20; // TODO
        f32 Unk18; // TODO
        s32 Unk19; // TODO
        f32 MinimumDistance;
        f32 Unk16; // TODO
        f32 MaximumDistance;
    };

    SoundEffectDescriptor* SoundEffectDescriptorInitialize(SoundEffectDescriptor* self);
    void SoundEffectDescriptorInitialize1(SoundEffectDescriptor* self);
    void* SoundEffectDescriptorDispose(SoundEffectDescriptor* self, const Memory::DisposeMode mode);

    struct SoundEffectDescriptorContainer
    {
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&SoundEffectDescriptorInitialize,
            .Dispose = (Objects::ABSTRACTOBJECTINITIALIZERDISPOSE)&SoundEffectDescriptorDispose,
            .Size = sizeof(SoundEffectDescriptor),
            .Name = "$SfxOptions$$"
        }; // 0x00613df0;
    };

    struct SoundEffect
    {
        SoundEffectDescriptor Descriptor;

        s32 UnknownIndex; // TODO

        s32 Unk32; // TODO check the possible values. So far it looks either as Index or an Active/Inactive flag.

        SoundSample* Sample;

        f32 HZ;

        f32 Distances[8]; // TODO constant

        f32 Pans[8]; // TODO constant

        s32 Unknown1[8]; // TODO constant

        s32 Unknown204[8]; // TODO constant

        f32 Unknown203; // TODO
        f32 Volume;
        f32 Unknown202; // TODO

        s32 Unknown101; // TODO IsDebug ?
        f32 Position;
        s32 Unknown104; // TODO

        u32 Options; // TODO: flags
    };

    SoundEffect* SoundEffectInitialize(SoundEffect* self);
    void* SoundEffectDispose(SoundEffect* self, const Memory::DisposeMode mode);
    void ReleaseSoundEffect(SoundEffect* self);
    void ReleaseSoundEffect1(SoundEffect* self);

    struct SoundEffectContainer
    {
        Objects::AbstractObjectInitializer Initializer =
        {
            .Options = 0x20100, // TODO
            .Initialize = (Objects::ABSTRACTOBJECTINITIALIZERINITIALIZE)&SoundEffectInitialize,
            .Dispose = (Objects::ABSTRACTOBJECTINITIALIZERDISPOSE)&SoundEffectDispose,
            .Size = sizeof(SoundEffect),
            .Name = "$SfxSlot$$"
        }; // 0x00613dd0;
    };

    extern SoundEffectContainer SoundEffectState;
}