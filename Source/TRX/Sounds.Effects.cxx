#include "Sounds.Effects.hxx"
#include "Sounds.hxx"
#include "Logger.hxx"
#include "Strings.hxx"

#include <math.h>

using namespace Mathematics;
using namespace IO;
using namespace IO::Streams;
using namespace Logger;
using namespace Memory;
using namespace Strings;
using namespace Assets::Sounds;

namespace Sounds
{
    SoundEffectContainer SoundEffectState;

    SoundEffectDescriptor* SoundEffectDescriptorInitialize(SoundEffectDescriptor* self)
    {
        SoundEffectDescriptorInitialize1(self);

        return self;
    }

    void SoundEffectDescriptorInitialize1(SoundEffectDescriptor* self)
    {
        self->NextChannelIndex = 0;

        self->Location.X = 0.0f;
        self->Location.Y = 0.0f;
        self->Location.Z = 0.0f;

        self->Velocity.X = 0.0f;
        self->Velocity.Y = 0.0f;
        self->Velocity.Z = 0.0f;

        self->Unk3 = 0;
        self->Unk4 = 0;
        self->Unk6 = 0;
        self->Unk7 = 0;
        self->Volume = 1.0f; // TODO constant
        self->HZ = 1.0f; // TODO

        ZeroMemory(&self->Tags, sizeof(char*) * 2); // TODO constants

        self->MinimumDistance = -1.0f; // TODO
        self->Unk16 = -1.0f; // TODO
        self->MaximumDistance = -1.0f; // TODO
        self->Unk18 = 0;
        self->Unk19 = 0;
        self->Unk20 = -1; // TODO
        self->RemainingDelay = -1.0f; // TODO constant
    }

    void* SoundEffectDescriptorDispose(SoundEffectDescriptor* self, const DisposeMode mode)
    {
        return self;
    }

    SoundEffect* SoundEffectInitialize(SoundEffect* self)
    {
        SoundEffectDescriptorInitialize1(&self->Descriptor);

        return self;
    }

    void* SoundEffectDispose(SoundEffect* self, const DisposeMode mode)
    {
        return self;
    }

    void ReleaseSoundEffect(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < 1)
        {
            LogError("Unable to release sound effect, sound effects must be locked.");
        }

        if (self->Unknown101 != 0)
        {
            LogMessage("[SOUND] Releasing sound effect %s...\n", self->Sample == NULL ? "<NULL>" : self->Sample->Descriptor.Definition.Name);
        }

        if (self->UnknownIndex != 0)
        {
            if (*SoundState._SoundControllerPointer != NULL)
            {
                (*SoundState._SoundControllerPointer)->Self->ReleaseSoundEffect(*SoundState._SoundControllerPointer, self);
            }

            self->UnknownIndex = 0;
        }

        self->Unknown101 = 0;
        self->Unk32 = 0;

        if (self->Sample != NULL)
        {
            if (self->Sample->Index < 1)
            {
                LogError("Unable to release sound effect. Sound effect is in invalid state.");
            }

            self->Sample->Index = self->Sample->Index - 1;

            auto sample = self->Sample;

            self->Sample = NULL;

            SelectUnknownSoundSampleValue(sample);

            if (-1 < sample->Unk7) // TODO
            {
                if (sample->StreamIndex == 2) // TODO
                {
                    SoundSampleRelease(sample);
                }
                else if (self == &SoundState.Effects._Cache[sample->Unk7])
                {
                    sample->Unk7 = -1;
                }
            }

            SelectSoundSampleCacheSize(0);
        }

        self->Options = 0;
    }

    BOOL SelectSoundEffectPosition1(SoundEffect* self)
    {
        if (*SoundState._SoundControllerPointer == NULL || self->Sample == NULL) { return FALSE; }

        if (self->Options & 0x40000000) { return TRUE; } // TODO

        const auto pos = (*SoundState._SoundControllerPointer)->Self->AcquireSoundEffectPosition(*SoundState._SoundControllerPointer, self);

        if (pos < 0.0f)
        {
            LogMessage("[ERROR] [SOUND] Error occurred while polling playback position of %s.\n", self->Sample->Descriptor.Definition.Name);

            return FALSE;
        }

        UpdateSoundEffectPosition2(self, pos);

        return TRUE;
    }

    void UpdateSoundEffectPosition2(SoundEffect* self, const f32 position)
    {
        if (self->Sample == NULL)
        {
            LogError("Unable to update playback position for missing sounds sample.");
        }

        if (position < 0.0f || self->Sample->Length < position)
        {
            LogError("Unable to update playback position. Buffer position is invalid.");
        }

        f32 pos = position - self->Position;

        if (pos < 0.0f)
        {
            pos = pos + self->Sample->Length;
        }

        if (pos < 0.0f || (self->Sample->Length + 0.001f < pos)) // TODO constants
        {
            LogError("Unable to update playback position for %s, attempted to step too much. %f - %f = %f, Sample = %d.",
                self->Sample->Descriptor.Definition.Name, position, self->Position, pos, self->Sample->Length);
        }

        if (self->Descriptor.Unk18 != self->Position || position < self->Descriptor.Unk18)
        {
            self->Descriptor.Unk18 = self->Descriptor.Unk18 + pos;
        }
        else
        {
            self->Descriptor.Unk18 = position;
        }

        if (self->Descriptor.Unk18 < 0.0f)
        {
            self->Descriptor.Unk18 = 0.0f;
        }

        self->Position = position;

        if (INVALID_SOUND_SAMPLE_LENGTH < self->Sample->Descriptor.Definition.Length
            && self->Sample->Descriptor.Definition.Length <= self->Descriptor.Unk18)
        {
            if (AcquireUnknownSoundSampleDescriptorValue1(&self->Sample->Descriptor) != 0) // TODO
            {
                const auto value = AcquireUnknownSoundValue101(self->Descriptor.Unk18 / self->Sample->Descriptor.Definition.Length);

                self->Descriptor.Unk18 = self->Descriptor.Unk18 - value * self->Sample->Descriptor.Definition.Length;
            }
            else
            {
                self->Descriptor.Unk18 = self->Sample->Descriptor.Definition.Length;
            }
        }
    }

    void ReleaseSoundEffect1(SoundEffect* self)
    {
        if (self->UnknownIndex == 0) { return; }

        if (*SoundState._SoundControllerPointer == NULL)
        {
            ReleaseSoundEffect(self);

            return;
        }

        if (self->Sample == NULL) { LogError("Unable to release missing sound effect."); }

        if (self->Options & 0x40000000) { return; } // TODO

        if (!(*SoundState._SoundControllerPointer)->Self->AcquireActiveSoundEffectBufferStatus(*SoundState._SoundControllerPointer, self))
        {
            if (AcquireUnknownSoundSampleDescriptorValue1(&self->Sample->Descriptor) != 0) // TODO
            {
                LogMessage("[ERROR] [SOUND] Releasing looping sound sample %s.\n", self->Sample->Descriptor.Definition.Name);
            }

            ReleaseSoundEffect(self);
        }
    }

    BOOL AcquireSoundEffectChannelState(const s32 indx)
    {
        if (indx < 0 || 31 < indx) // TODO constant
        {
            LogError("Unable to acquire sound effect channel state, invalid index.");
        }

        return SoundState.SFX.ChannelStatuses[indx];
    }
    
    f32 AcquireSoundEffectChannelVolume(const s32 indx)
    {
        if (indx < 0 || 31 < indx) // TODO constant
        {
            LogError("Unable to acquire sound effect channel volume, invalid index.");
        }

        return SoundState.SFX.ChannelVolumes1[indx];
    }

    void SelectSoundEffectChannelState(const s32 indx, const BOOL mode)
    {
        if (indx < 0 || 31 < indx) // TODO constant
        {
            LogError("Unable to select sound effect channel state, invalid index.");
        }

        SoundState.SFX.ChannelStatuses[indx] = mode;

        if (!mode)
        {
            LockSounds();

            for (u32 x = 0; x < 64; x++) // TODO constant
            {
                if (indx == SoundState.Effects._Cache[x].Descriptor.NextChannelIndex)
                {
                    ReleaseSoundEffect(&SoundState.Effects._Cache[x]);
                }
            }

            UnlockSound1();
        }
    }

    void SelectSoundEffectChannelCount(const u32 count)
    {
        if (count >= 32) // TODO constant
        {
            LogError("Unable to select number of sound effect channels to %d.", count);
        }

        SoundState.SFX.ChannelCount = count;
    }

    void SelectSoundEffectChannelVolume1(const s32 indx, const f32 value)
    {
        if (indx < 0 || 31 < indx)
        {
            LogError("Unable to select sound effect channel volume, invalid index.");
        }

        if (SoundState.SFX.ChannelVolumes1[indx] != value)
        {
            SoundState.SFX.ChannelVolumes1[indx] = value;

            if (*SoundState._SoundControllerPointer != NULL)
            {
                if (AcquireSoundMixMode1() != SoundMixMode::None)
                {
                    LockSounds();

                    for (u32 x = 0; x < 64; x++) // TODO constant
                    {
                        auto effect = &SoundState.Effects._Cache[x];

                        if (effect->Descriptor.NextChannelIndex == indx
                            && effect->Unk32 != 0 && effect->UnknownIndex != 0)
                        {
                            effect->Options = effect->Options | 8; // TODO
                        }
                    }

                    UnlockSound1();
                }
            }
        }
    }
    
    void SelectSoundEffectChannelVolume2(const s32 indx, const f32 value)
    {
        if (indx < 0 || 31 < indx) // TODO constant
        {
            LogError("Unable to set sound effect channel volume, invalid index %d.", indx);
        }

        if (SoundState.SFX.ChannelVolumes2[indx] != value)
        {
            SoundState.SFX.ChannelVolumes2[indx] = value;

            if (*SoundState._SoundControllerPointer != NULL)
            {
                if (AcquireSoundMixMode1() != SoundMixMode::None)
                {
                    LockSounds();

                    for (u32 x = 0; x < 64; x++) // TODO constant
                    {
                        auto effect = &SoundState.Effects._Cache[x];

                        if (effect->Descriptor.NextChannelIndex == indx
                            && effect->Unk32 != 0 && effect->UnknownIndex != 0)
                        {
                            effect->Options = effect->Options | 16; // TODO
                        }
                    }

                    UnlockSound1();
                }
            }
        }
    }

    void InitializeSoundEffectChannels(void)
    {
        if (!AcquireSoundEffectChannelState(0))
        {
            SelectSoundEffectChannelState(3, 0);
        }
        else
        {
            SelectSoundEffectChannelState(3, 1);
            SelectSoundEffectChannelVolume1(3, AcquireSoundEffectChannelVolume(0) * *SoundState._UnknownSoundVolume101);
        }

        SelectSoundEffectChannelVolume1(2, AcquireSoundEffectChannelVolume(0) * 7.0f); // TODO constant
        SelectSoundEffectChannelVolume1(4, AcquireSoundEffectChannelVolume(0));
    }

    u32 InitializeSoundEffect1(const char* file)
    {
        if (IsNullOrEmpty(file)) { return 0; } // TODO

        if (!AcquireSoundActiveMode()) { return 0; } // TODO

        SoundSampleDescriptor samdesc;
        SoundEffectDescriptor sefdesc;

        SoundSampleDescriptorInitialize1(&samdesc);
        SelectSoundSampleDescriptorName(&samdesc, file);

        SoundEffectDescriptorInitialize1(&sefdesc);

        CopyMemory(&sefdesc, &SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex], sizeof(SoundEffectDescriptor));

        if (!AcquireSoundEffectChannelState(sefdesc.NextChannelIndex)) { return 0; } // TODO

        LockSounds();

        if (*SoundState.Lock._Count < 1)
        {
            LogError("Unable to initialize sound effect, sound effect must be locked.");
        }

        s32 eindx = -1;

        for (u32 x = 0; x < 64; x++)// TODO: constants
        {
            if (SoundState.Effects._Cache[x].Unk32 == 0)
            {
                eindx = x;

                break;
            }
        }

        if (eindx < 0)
        {
            UnlockSound1();

            return 0; // TODO
        }

        auto effect = &SoundState.Effects._Cache[eindx];

        CopyMemory(&effect->Descriptor, &sefdesc, sizeof(SoundEffectDescriptor));

        s32 sindx = -1;

        for (u32 x = 0; x < 256; x++)// TODO: constants
        {
            if (EqualStrings(SoundState.Samples._Cache[x].Descriptor.Definition.Name, samdesc.Definition.Name))
            {
                sindx = x;

                break;
            }
        }

        SoundSample* sample = NULL;

        if (sindx == -1 || SoundState.Samples._Cache[sindx].Descriptor.CacheControl == SoundCacheMode::Stream)
        {
            sample = AcquireSoundSample1();

            if (sample == NULL)
            {
                LogMessage("Unable to play %s, no sound slots available.\n", samdesc.Definition.Name);

                if (sample != NULL) { SoundSampleRelease(sample); }

                if (effect != NULL) { ReleaseSoundEffect(effect); }

                UnlockSound1();

                return 0; // TODO
            }

            if (SoundState.Records.Cache.Count < 1)
            {
                char buffer[256];// TODO constant

                if (!InitializeSoundSampleDescriptor1(&samdesc, buffer))
                {
                    LogMessage("[ERROR] [SOUND] Unable to load file %s.sfx. %s.\n", samdesc.Definition.Name, buffer);

                    if (sample != NULL) { SoundSampleRelease(sample); }

                    if (effect != NULL) { ReleaseSoundEffect(effect); }

                    UnlockSound1();

                    return 0; // TODO
                }

                CopyMemory(&sample->Descriptor, &samdesc, sizeof(SoundSampleDescriptor));

                InStreamFile stream;

                InStreamFileInitialize(&stream);

                if (!LoadSoundSample1(sample, &stream))
                {
                    InStreamFileDispose(&stream, DisposeMode::None);

                    if (sample != NULL) { SoundSampleRelease(sample); }

                    if (effect != NULL) { ReleaseSoundEffect(effect); }

                    UnlockSound1();

                    return 0; // TODO
                }

                InStreamFileRelease(&stream);
                InStreamFileDispose(&stream, DisposeMode::None);
            }
            else
            {
                if (!AcquireSoundSampleDescriptor1(&samdesc))
                {
                    LogMessage("[ERROR] [SOUND] Unable to find sound sample %s in sound sample collection.\n", samdesc.Definition.Name);

                    if (sample != NULL) { SoundSampleRelease(sample); }

                    if (effect != NULL) { ReleaseSoundEffect(effect); }

                    UnlockSound1();

                    return 0; // TODO
                }

                CopyMemory(&sample->Descriptor, &samdesc, sizeof(SoundSampleDescriptor));

                FileDescriptor desc;

                if (!AcquireSoundSampleFileDescriptor(&sample->Descriptor, &desc))
                {
                    if (sample != NULL) { SoundSampleRelease(sample); }

                    if (effect != NULL) { ReleaseSoundEffect(effect); }

                    UnlockSound1();

                    return 0; // TODO
                }

                if (sample->Descriptor.Definition.Type == SoundSampleType::MP3)
                {
                    if (sample->Decoder != NULL)
                    {
                        ReleaseMemory2(SoundDecoderDispose(sample->Decoder, DisposeMode::None));
                    }

                    sample->Decoder = (SoundDecoder*)AllocateMemory1(sizeof(SoundDecoder));

                    if (sample->Decoder == NULL)
                    {
                        LogMessage("[ERROR] [SOUND] Unable to allocate memory for sound decoder.\n");

                        if (sample != NULL) { SoundSampleRelease(sample); }

                        if (effect != NULL) { ReleaseSoundEffect(effect); }

                        UnlockSound1();

                        return 0; // TODO
                    }

                    sample->Decoder = FUN_00514be0(sample->Decoder);

                    if (!FUN_00514c30(sample->Decoder, desc.FileName, 0))
                    {
                        LogMessage("[ERROR] [SOUND] Unable to initialize sound decoder.\n");

                        if (sample != NULL) { SoundSampleRelease(sample); }

                        if (effect != NULL) { ReleaseSoundEffect(effect); }

                        UnlockSound1();

                        return 0; // TODO
                    }
                }
            }

            u32 size = 0;
            u32 packet = 0;

            AcquireSoundSampleDescriptorDetails(&sample->Descriptor, &size, &packet);

            if (sample->Descriptor.CacheControl == SoundCacheMode::Stream)
            {
                sample->StreamIndex = 2; // TODO
                sample->Length = AcquireSoundSampleDescriptorValue201(
                    (s32)roundf(sample->Descriptor.Definition.HZ * SoundState.UnknownValue210), size);
            }
            else
            {
                sample->StreamIndex = 1; // TODO
                sample->Length = sample->Descriptor.Definition.Length;
            }

            sample->Unk6 = 0;
            sample->Unk5 = 0;
            sample->Unk10 = 0;
            sample->Unk11 = 0;
            sample->Unk7 = eindx;

            if (!AllocateSoundsSample(sample))
            {
                if (sample != NULL) { SoundSampleRelease(sample); }

                if (effect != NULL) { ReleaseSoundEffect(effect); }

                UnlockSound1();

                return 0; // TODO
            }

            effect->Sample = sample;

            SeekSoundSample1(sample, 0, 0);

            if (sample->Descriptor.CacheControl == SoundCacheMode::Stream)
            {
                LogMessage("[SOUND] Starting continuous sound streaming '%s' using %.1f-second buffer.\n",
                    sample->Descriptor.Definition.Name, ((f32)sample->Length / (f32)sample->Descriptor.Definition.HZ));
            }
            else
            {
                LogMessage("[SOUND] Starting initial sound %s data fill.\n", sample->Descriptor.Definition.Name);

                if (sample->Descriptor.CacheControl == SoundCacheMode::Static)
                {
                    LogMessage("[WARNING] [SOUND] Sound sample %s was not cached while the desired cache mode is static.\n", sample->Descriptor.Definition.Name);
                }
            }
        }
        else
        {
            sample = &SoundState.Samples._Cache[sindx];

            effect->Sample = sample;

            if (sample->StreamIndex == 1 && sample->Unk7 < 0)
            {
                sample->Unk5 = 0;
                sample->Unk7 = sindx;
                sample->Unk6 = sample->Unk10;
            }
        }

        if ((effect->Descriptor.Unk30 & 2) != 0) // TODO
        {
            sample->Descriptor.LoopMode = 1; // TODO
            sample->Descriptor.ChannelLength[0] = sample->Descriptor.Definition.Length;
            sample->Descriptor.Unk6 = 0;
        }

        sample->Index = sample->Index + 1;

        SeekSoundEffect1(effect);

        sample->Unk5 = (s32)roundf(effect->Descriptor.Unk18);

        effect->Unknown101 = 0;
        effect->Volume = -1.0f; // TODO constant
        effect->Unknown202 = 0.0f; // TODO constant
        effect->Unknown203 = effect->Descriptor.Volume;

        for (u32 x = 0; x < 8; x++) // TODO constant
        {
            effect->Unknown204[x] = -1;
        }

        if (effect->Descriptor.MinimumDistance < 0.0f)
        {
            effect->Descriptor.MinimumDistance = sample->Descriptor.ReferenceDistance;
        }

        if (effect->Descriptor.Unk16 < 0.0f)
        {
            effect->Descriptor.Unk16 = sample->Descriptor.MinimumDistance;
        }

        if (effect->Descriptor.MaximumDistance < 0.0f)
        {
            effect->Descriptor.MaximumDistance = sample->Descriptor.MaximumDistance;
        }

        SoundState.UnknownValue211 = SoundState.UnknownValue211 + 1;

        effect->Options = U32_MAX;

        if (0xfffffe < SoundState.UnknownValue211) // TODO constant
        {
            SoundState.UnknownValue211 = 1;
        }

        effect->Unk32 = SoundState.UnknownValue211;

        InitializeSoundEffectVolume(effect, 0.0f); // TODO constant

        if (AcquireSoundMixMode1() != SoundMixMode::None)
        {
            effect->UnknownIndex =
                (*SoundState._SoundControllerPointer)->Self->AllocateSoundEffect(
                    *SoundState._SoundControllerPointer, sample->BufferIndex);

            if (effect->UnknownIndex == 0)
            {
                LogMessage("[ERROR] [SOUND] Unable to start sound %s, unable to allocate hardware buffer.\n", sample->Descriptor.Definition.Name);

                if (sample != NULL) { SoundSampleRelease(sample); }

                if (effect != NULL) { ReleaseSoundEffect(effect); }

                UnlockSound1();

                return 0; // TODO
            }
        }

        if ((effect->Descriptor.Unk30 & 1) != 0 || sample->Descriptor.Definition.Channels == 1)
        {
            SelectUnknownSoundSampleValue(sample);

            UnlockSound1();

            return SoundState.UnknownValue211 << 6 | eindx; // TODO!!!! WTF?
        }

        LogMessage("[ERROR] [SOUND] Unable to play stereo sound %s with spatialization.\n", sample->Descriptor.Definition.Name);

        if (sample != NULL) { SoundSampleRelease(sample); }

        if (effect != NULL) { ReleaseSoundEffect(effect); }

        UnlockSound1();

        return 0; // TODO
    }

    BOOL InitializeSoundEffectVolume(SoundEffect* effect, const f32 volume)
    {
        if (*SoundState.Lock._Count < 1)
        {
            LogError("Unable to compute sound effect values, sound effect must be locked.");
        }

        if (effect->Sample == NULL) { return FALSE; }

        if (effect->Unknown101 == 1)
        {
            LogMessage("[DEBUG] [SOUND] Sound sample %s.", effect->Sample->Descriptor.Definition.Name);
        }

        if (effect->Unk32 != 0 && 0.0f <= effect->Descriptor.Unk18)
        {
            if (0.0f < volume && 0.0f <= effect->Volume) // TODO constants
            {
                if (effect->Volume <= volume)
                {
                    if ((u32)effect->Unknown202 & 0x7fffffffU) // TODO ???
                    {
                        ReleaseSoundEffect(effect);

                        return FALSE;
                    }

                    effect->Descriptor.Volume = effect->Unknown203;
                }
                else
                {
                    effect->Descriptor.Volume = effect->Descriptor.Volume
                        + (volume / effect->Volume) * (effect->Unknown203 - effect->Descriptor.Volume);

                    effect->Volume = effect->Volume - volume;
                }

                effect->Options = effect->Options | 8; // TODO
            }

            SelectSoundEffectLocationVelocity(effect);

            SoundState.Listener.Index = AcquireSoundListenerIndex(
                effect->Descriptor.Location.X, effect->Descriptor.Location.Y, effect->Descriptor.Location.Z);

            if (1 < SoundState.SoundTimeUnknown1)
            {
                SelectSoundLocations1();
            }

            if (effect->Descriptor.RemainingDelay == -1.0f) // TODO constant
            {
                if (effect->Unknown101 == 1)
                {
                    LogMessage(" Auto computing delay...");
                }

                SelectSoundEffectRemainingDelay(effect);
            }

            if (effect->Unknown101 == 1)
            {
                LogMessage(" Remaining Delay = %7.2fs.", effect->Descriptor.RemainingDelay);
            }

            effect->HZ = effect->Descriptor.HZ * SoundState.SFX.ChannelVolumes2[(effect->Descriptor).NextChannelIndex];

            if (effect->Unknown101 == 1)
            {
                LogMessage(" Frequency = %5.2f.", effect->HZ);
            }

            if (((u32)volume & 0x7fffffff) == 0) // TODO constants
            {
                SelectSoundLocations1();
            }

            if ((effect->Descriptor.Unk30 & 1) == 0) // TODO constants
            {
                const auto x = effect->Descriptor.Location.X - SoundState.Listener.Positions.X[SoundState.Listener.Index];
                const auto y = effect->Descriptor.Location.Y - SoundState.Listener.Positions.Y[SoundState.Listener.Index];
                const auto z = effect->Descriptor.Location.Z - SoundState.Listener.Positions.Z[SoundState.Listener.Index];

                auto distance = sqrtf(x * x + y * y + z * z);

                if (effect->Unknown101 == 1)
                {
                    LogMessage(" Distance To Ear = %7.2fs.", distance);
                }

                const auto vx = SoundState.Listener.Velocity.X[SoundState.Listener.Index] - effect->Descriptor.Velocity.X;
                const auto vy = SoundState.Listener.Velocity.Y[SoundState.Listener.Index] - effect->Descriptor.Velocity.Y;
                const auto vz = SoundState.Listener.Velocity.Z[SoundState.Listener.Index] - effect->Descriptor.Velocity.Z;

                if (distance <= 0.0f) // TODO constant
                {
                    distance = sqrtf(vx * vx + vy * vy + vz * vz);
                }
                else
                {
                    distance = (x * vx + y * vy + z * vz) / distance;
                }

                auto doppler = (distance * SoundState.Listener.DistanceFactor + 1116.4f) / 1116.4f; // TODO constant

                if (effect->Unknown101 == 1)
                {
                    LogMessage(" Doppler Value = %5.2f.", doppler);
                }

                doppler = Clamp(doppler, 0.25f, 4.0f); // TODO constants

                if (effect->Unknown101 == 1)
                {
                    LogMessage(" Clamped Doppler Value = %5.2f.", doppler);
                }

                effect->HZ = effect->HZ * doppler;
            }
            else if (effect->Unknown101 == 1)
            {
                LogMessage(" Non-Spatialized.");
            }

            SelectSoundEffectDistances1(effect);
            SelectSoundEffectDistances2(effect);

            if (effect->Unknown101 == 1)
            {
                LogMessage(" Volume = %5.2f.", effect->Descriptor.Volume);
            }

            SelectSoundEffectVolume(effect);

            if (effect->Unknown101 == 1)
            {
                for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
                {
                    LogMessage(" Channel %d Volume = %5.2f.", x,
                        effect->Pans[x] * (1 << (effect->Sample->Descriptor.Definition.BitsPerSample - 1U & 0x1f))); // TODO ???
                }
            }

            effect->HZ = Clamp(effect->HZ, 0.05f, 20.0f); // TODO constant

            if (effect->Unknown101 == 1)
            {
                LogMessage(" Effective Frequency = %5.2f.", effect->HZ);
            }

            return TRUE;
        }

        LogMessage("[ERROR] [SOUND] Releasing sound sample %s.", effect->Sample->Descriptor.Definition.Name);

        ReleaseSoundEffect(effect);

        return FALSE;
    }

    void SelectSoundEffectRemainingDelay(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < 1)
        {
            LogError("Unable to compute sound effect remaining delay, sound effect must be locked.");
        }

        if (self->Descriptor.Unk30 & 1)
        {
            self->Descriptor.RemainingDelay = 0.0f;

            return;
        }

        const auto x = self->Descriptor.Location.X - SoundState.Listener.Positions.X[SoundState.Listener.Index];
        const auto y = self->Descriptor.Location.Y - SoundState.Listener.Positions.Y[SoundState.Listener.Index];
        const auto z = self->Descriptor.Location.Z - SoundState.Listener.Positions.Z[SoundState.Listener.Index];

        self->Descriptor.RemainingDelay = (1.0f / 1116.4f) * sqrtf(x * x + y * y + z * z) * SoundState.Listener.DistanceFactor; // TODO constants
    }

    void SelectSoundEffectDistances1(SoundEffect* self)
    {
        for (u32 xx = 0; xx < *SoundState.Options._ChannelCount; xx++)
        {
            const auto x = self->Descriptor.Location.X - SoundState.Locations.X[xx];
            const auto y = self->Descriptor.Location.Y - SoundState.Locations.Y[xx];
            const auto z = self->Descriptor.Location.Z - SoundState.Locations.Z[xx];

            self->Distances[xx] = sqrtf(x * x + y * y + z * z);
        }
    }

    void SelectSoundEffectLocationVelocity(SoundEffect* self)
    {
        if (self->Descriptor.Unk4 == 1) // TODO constant
        {
            if (self->Descriptor.Location.X != self->Descriptor.Unk3[0]
                || self->Descriptor.Location.Y != self->Descriptor.Unk3[1]
                || self->Descriptor.Location.Z != self->Descriptor.Unk3[2])
            {
                self->Descriptor.Location.X = self->Descriptor.Unk3[0];
                self->Descriptor.Location.Y = self->Descriptor.Unk3[1];
                self->Descriptor.Location.Z = self->Descriptor.Unk3[2];

                self->Options = self->Options | 2; // TODO constant
            }
        }
        else if (self->Descriptor.Unk4 == 2)
        {
            // TODO, is this correct?
            if (self->Descriptor.Location.X != self->Descriptor.Unk3[0]
                || self->Descriptor.Location.Y != self->Descriptor.Unk3[1]
                || self->Descriptor.Location.Z != self->Descriptor.Unk3[2])
            {
                self->Descriptor.Location.X = self->Descriptor.Unk3[0];
                self->Descriptor.Location.Y = self->Descriptor.Unk3[1];
                self->Descriptor.Location.Z = self->Descriptor.Unk3[2];

                self->Options = self->Options | 2; // TODO constant
            }
        }

        if (self->Descriptor.Unk7 == 1) // TODO constant
        {
            if (self->Descriptor.Velocity.X != self->Descriptor.Unk6[0]
                || self->Descriptor.Velocity.Y != self->Descriptor.Unk6[1]
                || self->Descriptor.Velocity.Z != self->Descriptor.Unk6[2])
            {
                self->Descriptor.Velocity.X = self->Descriptor.Unk6[0];
                self->Descriptor.Velocity.Y = self->Descriptor.Unk6[1];
                self->Descriptor.Velocity.Z = self->Descriptor.Unk6[2];

                self->Options = self->Options | 4; // TODO constant
            }
        }
        else if (self->Descriptor.Unk7 == 2)
        {
            // TODO, is this correct?
            if (self->Descriptor.Velocity.X != self->Descriptor.Unk6[0]
                || self->Descriptor.Velocity.Y != self->Descriptor.Unk6[1]
                || self->Descriptor.Velocity.Z != self->Descriptor.Unk6[2])
            {
                self->Descriptor.Velocity.X = self->Descriptor.Unk6[0];
                self->Descriptor.Velocity.Y = self->Descriptor.Unk6[1];
                self->Descriptor.Velocity.Z = self->Descriptor.Unk6[2];

                self->Options = self->Options | 4; // TODO constant
            }
        }
    }

    void SelectSoundEffectVolume(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < 1)
        {
            LogError("Unable to select sound effect volume, sound effect must be locked.");
        }

        const auto volume = self->Descriptor.Volume * AcquireSoundEffectChannelVolume(self->Descriptor.NextChannelIndex);

        if ((self->Descriptor.Unk30 & 1) == 0) // TODO constant
        {
            for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
            {
                auto value = self->Distances[x];

                if (self->Distances[x] < self->Descriptor.Unk16)
                {
                    value = self->Descriptor.Unk16;
                }

                if (self->Descriptor.MaximumDistance < value)
                {
                    value = self->Descriptor.MaximumDistance;
                }

                auto minDist = self->Descriptor.MinimumDistance;

                if (minDist * 0.05f < value) // TODO constant
                {
                    minDist = minDist / value;
                }
                else
                {
                    minDist = 20.0f; // TODO constant
                }

                self->Pans[x] = minDist;
            }
        }
        else
        {
            for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
            {
                self->Pans[x] = volume;
            }
        }
    }

    SoundEffect* AcquireSoundEffect(const u32 hash, const u32 mode)
    {
        auto unk32 = (hash >> 6); // TODO constant
        auto indx = hash & 0x3f; // TODO constant

        if (unk32 != 0 && unk32 < 0xffffff && indx < 64) // TODO Consants
        {
            auto effect = &SoundState.Effects._Cache[indx];

            if (unk32 == SoundState.Effects._Cache[indx].Unk32) // TODO Consants
            {
                if (mode != 0) // TODO Consants
                {
                    LockSounds();

                    if (unk32 != effect->Unk32 || effect->Sample == NULL)
                    {
                        UnlockSound1();

                        return NULL;
                    }

                    if (effect->UnknownIndex != 0)
                    {
                        if (*SoundState._SoundControllerPointer == NULL)
                        {
                            ReleaseSoundEffect(effect);
                            UnlockSound1();

                            return NULL;
                        }

                        if (effect->Options & 0x40000000) { return effect; } // TODO Consants

                        if (!(*SoundState._SoundControllerPointer)->Self->AcquireActiveSoundEffectBufferStatus(*SoundState._SoundControllerPointer, effect))
                        {
                            ReleaseSoundEffect(effect);
                            UnlockSound1();

                            return NULL;
                        }
                    }
                }

                return effect;
            }
        }

        return NULL;
    }

    void SeekSoundEffect1(SoundEffect* self)
    {
        if (self->Sample == NULL)
        {
            LogError("Unable to seek sound effect sound sample, the sound sample is missing.");
        }

        self->Descriptor.Unk18 = SelectSoundSamplePlaybackPosition1(&self->Sample->Descriptor, self->Descriptor.Unk18, self->Descriptor.Unk19, 0);
        self->Descriptor.Unk19 = 0;
        self->Descriptor.Unk18 = SelectSoundSamplePlaybackPosition3(&self->Sample->Descriptor, self->Descriptor.Unk18, self->Descriptor.Unk19);

        if (self->Sample->StreamIndex != 2)
        {
            self->Position = self->Descriptor.Unk18;
        }
        else
        {
            SeekSoundSample1(self->Sample, (s32)roundf(self->Descriptor.Unk18), 0);
            self->Position = self->Sample->Unk11;
        }
    }

    void SelectSoundEffectDistances2(SoundEffect* self)
    {
        if (*SoundState.Lock._Count < 1)
        {
            LogError("Unable to TODO, sound effect must be locked.");
        }

        if (*SoundState.Options._ChannelCount < 1)
        {
            self->Unknown1[0] = 0;

            return;
        }

        if ((self->Descriptor.Unk30 & 1) == 0 && self->UnknownIndex == 0 && AcquireSoundMixMode1() == SoundMixMode::None)
        {
            auto minimumDistance = S32_MAX;

            for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
            {
                auto distance = (s32)roundf(self->Distances[x] * (f32)*SoundState.Options._HZ
                    * (1.0f / 1116.4f) * SoundState.Listener.DistanceFactor); // TODO constants

                if (distance < 0) { distance = 0; }

                self->Unknown1[x] = distance;

                if (distance < minimumDistance)
                {
                    minimumDistance = distance;
                }
            }

            for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
            {
                const auto distance = self->Unknown1[0] - minimumDistance;

                if (distance < *SoundState._UnknownValue204)
                {
                    self->Unknown1[0] = distance;
                }
                else
                {
                    self->Unknown1[0] = *SoundState._UnknownValue204;
                }
            }
        }
        else
        {
            for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
            {
                self->Unknown1[x] = 0;
            }
        }
    }
    
    void SelectSoundEffectLocation(const f32 x, const f32 y, const f32 z)
    {
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Location.X = x;
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Location.Y = y;
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Location.Z = z;

        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk3 = NULL;

        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk4 = 0;
    }

    u32 InitializeSoundEffect2(const char* file, const f32 x, const f32 y, const f32 z)
    {
        IncrementSountEffectOptionsIndex();

        SelectUnknownSoundMode2(1);
        SelectSoundEffectLocation(x, y, z);

        const auto result = InitializeSoundEffect1(file);

        DecrementSountEffectOptionsIndex();

        return result;
    }

    u32 InitializeSoundEffect3(const char* file)
    {
        IncrementSountEffectOptionsIndex();

        SelectUnknownSoundMode1(1);

        const auto result = InitializeSoundEffect1(file);

        DecrementSountEffectOptionsIndex();

        return result;
    }

    BOOL ReleaseSoundEffect2(const u32 hash)
    {
        auto self = AcquireSoundEffect(hash, 1);

        if (self == NULL) { return FALSE; }

        ReleaseSoundEffect(self);

        UnlockSound1();

        return TRUE;
    }

    BOOL AcquireSoundEffect1(const u32 hash)
    {
        if (AcquireSoundMixMode1() == SoundMixMode::None)
        {
            return AcquireSoundEffect(hash, 0) != NULL;
        }

        if (AcquireSoundEffect(hash, 1) == NULL) { return FALSE; }

        UnlockSound1();

        return TRUE;
    }

    void SelectSoundEffectUnknown9(const f32 value) // TODO better name
    {
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].HZ = value;
    }

    void SelectSoundEffectVolume1(const f32 value)
    {
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Volume = value;
    }

    void SelectSoundEffectUnknown30(const s32 value) // TODO better name
    {
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk30 = value;
    }

    void SelectNextSoundEffectTag(const s32 indx, char* value)
    {
        if (indx < 0 || 1 < indx) // TODO constants
        {
            LogError("Unable to select next sound effect tag, invalid index %d.", indx);
        }

        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Tags[indx] = value;
    }

    void SelectSoundEffectUnknown3And4(f32* value) // TODO better name
    {
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk3 = value;
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk4 = 1; // TODO constant
    }

    void SelectSoundEffectVelocity(const f32 x, const f32 y, const f32 z)
    {
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Velocity.X = x;
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Velocity.Y = y;
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Velocity.Z = z;

        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk6 = NULL;

        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk7 = 0;
    }

    void SelectSoundEffectUnknown18And19(const f32 unk18, const s32 unk19) // TODO better name
    {
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk18 = unk18;
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk19 = unk19;
    }

    u32 AcquireActiveSoundEffectCount(void) // TODO constants
    {
        u32 result = 0;

        for(u32 x = 0; x < 64; x++)
        {
            const auto effect = &SoundState.Effects._Cache[x];

            if (effect->Sample != NULL && effect->Unk32 != 0)
            {
                result = result + 1;
            }
        }

        return result;
    }

    u32 AcquireSoundEffectUnknownValue1(void)
    {
        u32 indx = 0;
        u32 unk32 = 0; // TODO name

        for (u32 x = 0; x < 64; x++) // TODO constants
        {
            const auto effect = &SoundState.Effects._Cache[x];

            unk32 = effect->Unk32;

            if (effect->Sample == NULL || effect->Unk32 == 0)
            {
                indx = indx + 1;
            }
        }

        return (unk32 << 6) | indx; // TODO constant, wtf?
    }

    u32 AcquireSoundEffectUnknownValue2(const u32 hash)
    {
        u32 indx = (hash & 0x3f) + 1; // TODO constants

        if (indx < 64) // TODO constant
        {
            for (u32 x = indx; x < 64; x++) // TODO constant
            {
                const auto effect = &SoundState.Effects._Cache[x];

                if (effect->Sample != NULL && effect->Unk32 != 0)
                {
                    return  (effect->Unk32 << 6) | indx;
                }

                indx = indx + 1;
            }
        }

        return 0;
    }
    
    BOOL SelectSoundEffectValues1(const u32 hash, const f32 unk203, const f32 volume, const s32 unk202)
    {
        auto effect = AcquireSoundEffect(hash, 1);

        if (effect == NULL) { return FALSE; }

        effect->Unknown203 = unk203;
        effect->Volume = volume;
        effect->Unknown202 = unk202;
        
        UnlockSound1();

        return TRUE;
    }

    BOOL AcquireSoundEffectDescriptor(const u32 hash, SoundEffectDescriptor* desc)
    {
        auto effect = AcquireSoundEffect(hash, 1);

        if (effect == NULL) { return FALSE; }

        InitializeSoundEffectVolume(effect, 0.0f);

        CopyMemory(desc, &effect->Descriptor, sizeof(SoundEffectDescriptor));

        UnlockSound1();

        return TRUE;
    }
}