#include "ASCII.hxx"
#include "Assets.Sounds.Wave.hxx"
#include "Logger.hxx"
#include "Native.hxx"
#include "Sounds.hxx"
#include "Sounds.Samples.hxx"
#include "Strings.hxx"

#include <stdio.h>
#include <math.h>

using namespace Mathematics;

using namespace Strings;
using namespace IO;
using namespace IO::Streams;
using namespace Native;
using namespace Logger;
using namespace Memory;
using namespace Assets::Sounds;
using namespace Assets::Sounds::Wave;

namespace Sounds
{
    SoundSampleContainer SoundSampleState;

    SoundSample* SoundSampleInitialize(SoundSample* self)
    {
        SoundSampleDescriptorInitialize1(&self->Descriptor);

        self->Descriptor.AllocatedMemory1 = NULL;

        self->Index = 0;
        self->BufferIndex = 0;

        self->StreamIndex = 0;
        self->Unk7 = -1; // TODO
        self->Unk5 = 0;
        self->Unk6 = 0;

        self->Decoder = NULL;

        self->LockLength = 0;
        self->Length = 0;
        self->AllocatedMemorySize = 0;
        self->Unk13 = 0;
        self->Unk16 = 0;

        return self;
    }

    void SoundSampleDescriptorInitialize1(SoundSampleDescriptor* self)
    {
        ZeroMemory(self, sizeof(SoundSampleDescriptor));

        self->CacheControl = SoundCacheMode::Normal;

        self->Offset = -1; // TODO
        self->Definition.Length = INVALID_SOUND_SAMPLE_LENGTH;
        self->LoopMode = 1; // TODO
        self->ChannelLength[0] = -1; // TODO
        self->Unk6 = -1; // TODO
        self->Priority = 1000; // TODO

        self->ReferenceDistance = 20.0f * SoundState.Listener.InverseDistanceFactor; // TODO constant
        self->MinimumDistance = SoundState.UnknownValue101 * SoundState.Listener.InverseDistanceFactor;
        self->MaximumDistance = 10000.0f * SoundState.Listener.InverseDistanceFactor; // TODO constant
    }

    void* SoundSampleDispose(SoundSample* self, const DisposeMode mode)
    {
        SoundSampleRelease(self);

        return self;
    }

    void UpdateSoundSample(SoundSample* self)
    {
        if (self->Unk13 == 0) { return; } // TODO constant

        if (self->Unk13 == 2) // TODO constant
        {
            UnlockSoundSampleBuffer1(self);

            if (self->Unk13 != 1) { return; } // TODO constant
        }
        else if (self->Unk13 != 1) { return; }  // TODO constant

        do
        {
            AssignUnknownSoundSample1(self);
        } while (self->Unk13 == 1); // TODO constant
    }

    void SoundSampleRelease(SoundSample* self)
    {
        if (SoundState.Samples.UnknownSample == self)
        {
            if (SoundState.Samples.AssetOperationIndex != INVALID_SOUND_ASSET_OPERATION_INDEX)
            {
                AwaitSoundAssetAvailability(SoundState.Samples.AssetOperationIndex);

                SoundState.Samples.AssetOperationIndex = INVALID_SOUND_ASSET_OPERATION_INDEX;
            }

            *SoundState.Samples._UnknownSampleValue1 = -1; // TODO
            *SoundState.Samples._UnknownSampleValue2 = -1; // TODO

            SoundState.Samples.UnknownSample = NULL;
        }

        UpdateSoundSample(self);
        ReleaseSoundSample(self);

        if (self->Index != 0)
        {
            LogError("Unable to release sound sample. The sample is in use.");
        }

        ReleaseSoundSampleDescriptor(&self->Descriptor);
        SoundSampleRelease1(self);

        if (self->Decoder != NULL)
        {
            ReleaseMemory2(SoundDecoderDispose(self->Decoder, DisposeMode::None));
        }

        self->Decoder = NULL;

        SoundSampleDescriptorInitialize1(&self->Descriptor);

        self->Length = 0;
        self->AllocatedMemorySize = 0;
        self->StreamIndex = 0;
        self->Unk7 = -1; // TODO
        self->Unk5 = 0;
        self->Unk6 = 0;
        self->Unk16 = 0;
    }

    void SoundSampleRelease1(SoundSample* self)
    {
        if (self->BufferIndex != 0)
        {
            if (*SoundState._SoundControllerPointer != NULL)
            {
                (*SoundState._SoundControllerPointer)->Self->ReleaseSample(*SoundState._SoundControllerPointer, self->BufferIndex);
            }

            self->BufferIndex = 0;
        }
    }

    void ReleaseSoundSampleDescriptor(SoundSampleDescriptor* self)
    {
        if (self->AllocatedMemory1 != NULL)
        {
            ReleaseMemory1(self->AllocatedMemory1);

            self->AllocatedMemory1 = NULL;
        }
    }

    void SelectSoundSampleDescriptorName(SoundSampleDescriptor* self, const char* name)
    {
        auto indx = 0;

        while (name[indx] != NULL && name[indx] != ASCII_DOT)
        {
            self->Definition.Name[indx] = name[indx];

            indx = indx + 1;
        }

        self->Definition.Name[indx] = NULL;
    }

    void SelectUnknownSoundSampleValue(SoundSample* self)
    {
        SoundState.UnknownValue102 = SoundState.UnknownValue102 + 1;

        self->Unk16 = SoundState.UnknownValue102;
    }

    void ReleaseSoundSample(SoundSample* self)
    {
        for (u32 x = 0; x < 64; x++) // TODO
        {
            const auto effect = &SoundState.Effects._Cache[x];

            if (self == effect->Sample)
            {
                ReleaseSoundEffect(effect);
            }
        }
    }

    void SoundSampleInitialize1(SoundSample* self, SoundSampleDescriptor* desc)
    {
        CopyMemory(desc, &self->Descriptor, sizeof(SoundSampleDescriptor));

        if (self->Length == desc->Definition.Length) { return; }

        desc->LoopMode = 1; // TODO
        desc->Definition.Length = self->Length;
        desc->Unk6 = 0;
        desc->ChannelLength[0] = self->Length;
    }

    BOOL AcquireSoundSampleFileDescriptor(SoundSampleDescriptor* self, FileDescriptor* desc)
    {
        char ext[STANDARD_IO_EXTENSION_NAME_LENGTH];

        ZeroMemory(ext, STANDARD_IO_EXTENSION_NAME_LENGTH);

        switch (self->Definition.Type)
        {
        case SoundSampleType::WAVMSPCM:
        case SoundSampleType::WAVYADPCM:
        case SoundSampleType::WAVVWBA:
        {
            strcpy(ext, SOUND_FILE_WAVE_EXTENSION);

            break;
        }
        case SoundSampleType::MP3:
        {
            strcpy(ext, SOUND_FILE_MP3_EXTENSION);

            break;
        }
        case SoundSampleType::Unknown4:
        {
            LogError("NOT IMPLEMENTED");//TODO
            //ext = (char*)0x00766177;
            break;
        }
        case SoundSampleType::Unknown6:
        {
            LogError("NOT IMPLEMENTED");//TODO
            //ext = (char*)0x00766177;
            break;
        }
        default:
        {
            LogMessage("[ERROR] [SOUND] Sound sample %s has unknown type %d.\n", self->Definition.Name, self->Definition.Type);

            return FALSE;
        }
        }

        sprintf(desc->FileName, "sound\\%s.%s", self->Definition.Name, ext);// TODO constants

        if (!HandleFile(desc))
        {
            LogMessage("[ERROR] [SOUND] Unable to find file %s.\n", desc->FileName);

            return FALSE;
        }

        return TRUE;
    }

    BOOL LoadSoundSample1(SoundSample* self, InStreamFile* stream)
    {
        FileDescriptor desc;

        sprintf(desc.FileName, "sound\\%s.wav", self->Descriptor.Definition.Name);// TODO constants

        if (!HandleFile(&desc))
        {
            sprintf(desc.FileName, "sound\\%s.mp3", self->Descriptor.Definition.Name);// TODO constants

            if (!HandleFile(&desc))
            {
                LogMessage("[ERROR] [SOUND] Unable to find sound sample %s.\n", self->Descriptor.Definition.Name);

                return FALSE;
            }

            self->Decoder = (SoundDecoder*)AllocateMemory1(sizeof(SoundDecoder));

            if (self->Decoder == NULL)
            {
                LogMessage("[ERROR] [SOUND] Unable to allocate memory for sound decoder.\n");

                return FALSE;
            }

            FUN_00514be0(self->Decoder);

            FUN_00514c30(self->Decoder, desc.FileName, 1);

            self->Descriptor.Definition.Type = SoundSampleType::MP3;
            self->Descriptor.Definition.BitsPerSample = SOUND_BITS_PER_SAMPLE_16;
            self->Descriptor.Definition.Channels = self->Decoder->Channels;
            self->Descriptor.Definition.HZ = self->Decoder->HZ;

            if (self->Descriptor.Definition.Length == 0)
            {
                self->Descriptor.Definition.Length = INVALID_SOUND_SAMPLE_LENGTH;
            }

            return TRUE;
        }

        if (!stream->Self->OpenFile(stream, &desc, TRUE)) { return FALSE; }

        self->Descriptor.Offset = 0;

        u32 size = 0;
        u32 offset = 0;

        WaveDefinition definition;

        if (stream->Self->Seek(stream, 0, StreamSeek::Set))
        {
            WaveChunk chunk;

            if (stream->Self->ReadBlock(stream, &chunk, sizeof(WaveChunk)) == sizeof(WaveChunk))
            {
                if (memcmp(&chunk.Name, RIFF_HEADER_VALUE, RIFF_HEADER_VALUE_LENGTH) != 0)
                {
                    LogMessage("[ERROR] [SOUND] \"RIFF\" value not found at the offset 0 of %s.\n", desc.FileName);

                    return FALSE;
                }

                size = chunk.Size + 8; // Calculate full file size.

                if (stream->Self->ReadBlock(stream, &chunk, WAVE_HEADER_VALUE_LENGTH) != WAVE_HEADER_VALUE_LENGTH)
                {
                    LogMessage("[ERROR] [SOUND] Unknown error occurred while reading %s.\n", desc.FileName);

                    return FALSE;
                }

                if (memcmp(&chunk.Name, WAVE_HEADER_VALUE, WAVE_HEADER_VALUE_LENGTH) != 0)
                {
                    LogMessage("[ERROR] [SOUND] \"WAVE\" value not found at the offset 8 of %s.\n", desc.FileName);

                    return FALSE;
                }

                offset = sizeof(WaveChunk) + WAVE_HEADER_VALUE_LENGTH;

                if (!stream->Self->Seek(stream, self->Descriptor.Offset + offset, StreamSeek::Set))
                {
                    LogMessage("[ERROR] [SOUND] Unknown error occurred while reading %s.\n", desc.FileName);

                    return FALSE;
                }

                if (stream->Self->ReadBlock(stream, &chunk, sizeof(WaveChunk)) != sizeof(WaveChunk))
                {
                    LogMessage("[ERROR] [SOUND] Unknown error occurred while reading %s.\n", desc.FileName);

                    return FALSE;
                }

                if (memcmp(&chunk.Name, WAVE_DATA_HEADER_VALUE, WAVE_DATA_HEADER_VALUE_LENGTH) == 0)
                {
                    LogMessage("[ERROR] [SOUND] The \"data\" chunk precedes \"fmt\" chunk in %s.\n", desc.FileName);

                    return FALSE;
                }

                if (memcmp(&chunk.Name, WAVE_FORMAT_HEADER_VALUE, WAVE_FORMAT_HEADER_VALUE_LENGTH) != 0)
                {
                    LogMessage("[ERROR] [SOUND] Unable to find \"fmt\" chunk in %s.\n", desc.FileName);

                    return FALSE;
                }

                offset = offset + sizeof(WaveChunk);

                if (chunk.Size < sizeof(WaveDefinition))
                {
                    LogMessage("[ERROR] [SOUND] File %s contains invalid \"fmt\" chunk.\n", desc.FileName);

                    return FALSE;
                }

                if (stream->Self->ReadBlock(stream, &definition, sizeof(WaveDefinition)) != sizeof(WaveDefinition))
                {
                    LogMessage("[ERROR] [SOUND] Unknown error occurred while reading %s.\n", desc.FileName);

                    return FALSE;
                }

                offset = offset + sizeof(WaveDefinition);

                switch (definition.AudioFormat)
                {
                case WAVE_FORMAT_PCM:
                {
                    self->Descriptor.Definition.Type = SoundSampleType::WAVMSPCM;

                    break;
                }
                case WAVE_FORMAT_YAMAHA_ADPCM:
                {
                    self->Descriptor.Definition.Type = SoundSampleType::WAVYADPCM;

                    break;
                }
                case WAVE_FORMAT_VOXWARE_BYTE_ALIGNED:
                {
                    self->Descriptor.Definition.Type = SoundSampleType::WAVVWBA;

                    break;
                }
                default:
                {
                    LogMessage("[ERROR] [SOUND] File %s has unsupported audio format %d.\n", desc.FileName, definition.AudioFormat);

                    return FALSE;
                }
                }

                self->Descriptor.Definition.BitsPerSample = 0;

                if (definition.Channels != 1 && definition.Channels != 2)
                {
                    LogMessage("[ERROR] [SOUND] File %s has unsupported number of audio channels %d.\n", desc.FileName, definition.Channels);

                    return FALSE;
                }

                switch (self->Descriptor.Definition.Type)
                {
                case SoundSampleType::WAVMSPCM:
                {
                    if (definition.Channels * definition.SampleRate == definition.ByteRate)
                    {
                        self->Descriptor.Definition.BitsPerSample = SOUND_BITS_PER_SAMPLE_8;
                    }
                    else if (2 * definition.Channels * definition.SampleRate == definition.ByteRate) // TODO
                    {
                        self->Descriptor.Definition.BitsPerSample = SOUND_BITS_PER_SAMPLE_16;
                    }
                    else
                    {
                        LogMessage("[ERROR] [SOUND] File %s has invalid bytes per second %d.\n", desc.FileName, definition.ByteRate);

                        return FALSE;
                    }

                    break;
                }
                case SoundSampleType::MP3:
                case SoundSampleType::WAVYADPCM:
                case SoundSampleType::WAVVWBA:
                {
                    self->Descriptor.Definition.BitsPerSample = SOUND_BITS_PER_SAMPLE_4;

                    break;
                }
                default:
                {
                    LogError("Unable to load sound file %s, it seems the file is corrupted.", desc.FileName);
                }
                }
            }
            else
            {
                LogMessage("[ERROR] [SOUND] Unknown error occurred while reading %s.\n", desc.FileName);

                return FALSE;
            }
        }
        else
        {
            LogMessage("[ERROR] [SOUND] Unknown error occurred while reading %s.\n", desc.FileName);

            return FALSE;
        }

        {
            WaveChunk chunk;

            if (!stream->Self->Seek(stream, self->Descriptor.Offset + offset, StreamSeek::Set))
            {
                LogMessage("[ERROR] [SOUND] Unknown error occurred while reading %s.\n", desc.FileName);

                return FALSE;
            }

            if (stream->Self->ReadBlock(stream, &chunk, sizeof(WaveChunk)) != sizeof(WaveChunk))
            {
                LogMessage("[ERROR] [SOUND] Unknown error occurred while reading %s.\n", desc.FileName);

                return FALSE;
            }

            if (memcmp(&chunk.Name, WAVE_DATA_HEADER_VALUE, WAVE_DATA_HEADER_VALUE_LENGTH) != 0)
            {
                LogMessage("[ERROR] [SOUND] Unable to find \"data\" chunk in %s.\n", desc.FileName);

                return FALSE;
            }

            self->Descriptor.Offset = self->Descriptor.Offset + offset + sizeof(WaveChunk);
            self->Descriptor.Definition.Channels = definition.Channels;
            self->Descriptor.Definition.HZ = definition.SampleRate;

            u32 size = 0;
            u32 packet = 0;

            AcquireSoundSampleDescriptorDetails(&self->Descriptor, &size, &packet);

            if (definition.SampleRate % packet == 0)
            {
                self->Descriptor.Definition.Length = size * ((u64)(chunk.Size % packet) << 0x20) | ((u64)chunk.Size / (u64)packet); // TODO

                SelectSoundSampleDescriptor1(&self->Descriptor);
            }
            else
            {
                LogMessage("[ERROR] [SOUND] File %s data size %d does not match a multiple of packet size %d.\n", desc.FileName, chunk.Size, packet);

                return FALSE;
            }
        }

        return TRUE;
    }

    // TODO: proper parameter names
    void AcquireSoundSampleDescriptorDetails(const SoundSampleDescriptor* self, u32* size, u32* packet)
    {
        switch (self->Definition.Type)
        {
        case SoundSampleType::Unknown4:
        {
            *size = 28; // TODO
            *packet = 16; // TODO

            break;
        }
        case SoundSampleType::WAVVWBA:
        {
            *size = 64; // TODO
            *packet = self->Definition.Channels * 36; // TODO

            break;
        }
        case SoundSampleType::Unknown6:
        {
            *size = 56; // TODO
            *packet = 32; // TODO

            break;
        }
        default:
        {
            *size = 1; // TODO
            *packet = self->Definition.Channels * (self->Definition.BitsPerSample >> 3); // TODO

            break;
        }
        }
    }

    void SelectSoundSampleDescriptor1(SoundSampleDescriptor* self)
    {
        for (u32 x = 0; x < self->LoopMode; x++) // TODO
        {
            if (self->ChannelLength[x] < 0)
            {
                self->ChannelLength[x] = self->Definition.Length;
            }
        }
    }

    // TODO: enumeration?
    s32 AcquireUnknownSoundSampleDescriptorValue1(const SoundSampleDescriptor* self)
    {
        if (self->LoopMode < 2
            && ((self->ChannelLength[0] < 0 || self->Definition.Length <= self->ChannelLength[0]) && self->Unk6 < 1)) // TODO
        {
            if (self->Unk6 == 0) { return 1; } // TODO

            return 0; // TODO
        }

        return 2; // TODO
    }

    BOOL AllocateSoundsSample(SoundSample* self)
    {
        SoundSampleRelease1(self);
        ReleaseSoundSampleDescriptor(&self->Descriptor);

        self->AllocatedMemorySize = AcquireSoundSampleDescriptorValue203(&self->Descriptor, self->Length, 0);

        switch (self->Descriptor.Definition.Type)
        {
        case SoundSampleType::WAVMSPCM:
        case SoundSampleType::MP3:
        {
            if (AcquireSoundMixMode1() == SoundMixMode::None)
            {
                SelectSoundSampleCacheSize(self->AllocatedMemorySize);

                const auto memory = ReallocateMemory(self->Descriptor.AllocatedMemory1, self->AllocatedMemorySize); // TODO

                if (memory != NULL)
                {
                    self->Descriptor.AllocatedMemory1 = memory;

                    return TRUE;
                }
            }
            else
            {
                SoundSampleDescriptor desc;

                SoundSampleDescriptorInitialize1(&desc);
                SoundSampleInitialize1(self, &desc);

                SoundSampleDescriptor sbd;

                CopyMemory(&sbd, &desc, sizeof(SoundSampleDescriptor));

                if (*SoundState._SoundControllerPointer != NULL)
                {
                    auto const size = AcquireSoundSampleDescriptorValue203(&sbd, sbd.Definition.Length, 0);

                    if (size < 0x100001) // TODO constant
                    {
                        for (u32 x = 0; x < 256; x++) // TODO constant
                        {
                            const auto indx = (*SoundState._SoundControllerPointer)->Self->CreateSecondaryBuffer(*SoundState._SoundControllerPointer, &sbd);

                            if (indx != INVALID_SOUND_BUFFER_POOL_INDEX)
                            {
                                self->BufferIndex = indx;

                                return TRUE;
                            }

                            auto ci = SoundState.Samples.CacheIndex;

                            for (u32 xx = 0; xx < 256; xx++) // TODO constant
                            {
                                ci = ci + 1;

                                if ((256 - 1) < ci) { ci = 0; } // TODO constant

                                if (SoundState.Samples._Cache[ci].BufferIndex != 0
                                    && 0 < SoundState.Samples._Cache[ci].AllocatedMemorySize
                                    && 0 < SoundState.Samples._Cache[ci].Length
                                    && SoundState.Samples._Cache[ci].LockLength == 0
                                    && SoundState.Samples._Cache[ci].Index == 0
                                    && SoundState.Samples._Cache[ci].Descriptor.CacheControl != SoundCacheMode::Static)
                                {
                                    LogMessage("[SOUND] Releasing sound %s from the sound sample cache.\n", SoundState.Samples._Cache[ci].Descriptor.Definition.Name);

                                    SoundSampleRelease(&SoundState.Samples._Cache[ci]);

                                    break;
                                }
                            }
                        }

                        LogMessage("[ERROR] [SOUND] Unable to allocate sound sample, the sound sample cache is full.\n");
                    }
                    else
                    {
                        LogMessage("[WARNING] [SOUND] Unable to allocate a buffer of size %d for sample %s.\n", size, sbd.Definition.Name);
                    }
                }
            }
        }
        }

        self->BufferIndex = 0;

        return FALSE;
    }

    SoundSample* AcquireSoundSample1()
    {
        for (u32 x = 0; x < 256; x++) // TODO constant
        {
            SoundState.Samples.CacheIndex = SoundState.Samples.CacheIndex + 1;

            if ((256 - 1) < SoundState.Samples.CacheIndex) // TODO constant
            {
                SoundState.Samples.CacheIndex = 0;
            }

            auto sample = &SoundState.Samples._Cache[SoundState.Samples.CacheIndex];

            if (IsNullOrEmpty(sample->Descriptor.Definition.Name))
            {
                SoundSampleRelease(sample);

                return sample;
            }
        }

        s32 indx = -1;
        s32 max = -1;

        for (u32 x = 0; x < 256; x++) // TODO constant
        {
            auto sample = &SoundState.Samples._Cache[x];

            if (sample->LockLength == 0 && sample->Index == 0
                && sample->Descriptor.CacheControl != SoundCacheMode::Static)
            {
                auto value = SoundState.UnknownValue102 - sample->Unk16;

                if (max < value)
                {
                    max = value;
                    indx = x;
                }
            }
        }

        if (-1 < indx)
        {
            auto sample = &SoundState.Samples._Cache[indx];

            SoundSampleRelease(sample);

            return sample;
        }

        LogMessage("[ERROR] [SOUND] Unable to allocate sound sample, no available cache items left.\n");

        return NULL;
    }

    BOOL LoadSoundSample2(SoundSample* self, InStreamFile* stream)
    {
        auto data = LockSoundSample(self, 0, self->Descriptor.Definition.Length);

        if (data == NULL)
        {
            LogMessage("[ERROR] [SOUND] Unable to lock sound sample %s.\n", self->Descriptor.Definition.Name);

            return FALSE;
        }

        if (stream->Self->ReadBlock(stream, data, self->AllocatedMemorySize) == self->AllocatedMemorySize)
        {
            UpdateSoundSample(self);

            return TRUE;
        }

        LogMessage("[ERROR] [SOUND] Unable to read sound sample data %s.\n", self->Descriptor.Definition.Name);

        return FALSE;
    }

    f32 SelectSoundSamplePlaybackPosition1(SoundSampleDescriptor* self, const f32 value, const u32 type1, const u32 type2) // TODO enums
    {
        if (type1 == type2) { return value; }

        auto result = 0.0f;

        if (type1 != 0)
        {
            switch (type1)
            {
            case 1:
            {
                result = self->Definition.HZ * value;
                break;
            }
            case 2:
            {
                if (self->Definition.Length < 1)
                {
                    LogError("Unable to update sound sample playback position, unable to use relative sample position when the length of %s is unknown.",
                        self->Definition.Name);
                }
                else
                {
                    result = self->Definition.Length * value;
                }

                break;
            }
            default:
            {
                LogError("Unable to update sound sample playback position, invalid sample position type.");
            }
            }
        }

        if (type2 != 0)
        {
            switch (type2)
            {
            case 1:
            {
                result = value / (f32)self->Definition.HZ;
                break;
            }
            case 2:
            {
                if (self->Definition.Length < 1)
                {
                    LogError("Unable to update sound sample playback position, unable to use relative sample position when the length of %s is unknown.",
                        self->Definition.Name);
                }
                else
                {
                    result = value / (f32)self->Definition.Length;
                }

                break;
            }
            default:
            {
                LogError("Unable to update sound sample playback position, invalid sample position type.");
            }
            }
        }

        return result;
    }

    f32 SelectSoundSamplePlaybackPosition2(const u32 hash, const u32 type)
    {
        auto effect = AcquireSoundEffect(hash, 1);

        if (effect == NULL) { return -1.0f; }// TODO constant

        if (effect->UnknownIndex != 0 && *SoundState._SoundControllerPointer)
        {
            if (!SelectSoundEffectPosition1(effect))
            {
                UnlockSound1();

                return -1.0f;// TODO constant
            }
        }

        auto result = SelectSoundSamplePlaybackPosition1(&effect->Sample->Descriptor,
            effect->Descriptor.Unk18, effect->Descriptor.Unk19, type);

        UnlockSound1();

        return result;;
    }

    void AssignUnknownSoundSample1(SoundSample* self)
    {
        if (self->Unk13 == 1 && 0 < self->LockLength && self == *SoundState._UnknownSoundSample1)
        {
            if ((*SoundState._UnknownSoundSample1)->BufferIndex == 0
                || *SoundState._SoundControllerPointer == NULL) {
                self->LockLength = 0;
                self->Unk13 = 0;
            }
            else
            {
                if ((*SoundState._SoundControllerPointer)->Self->UnknownValue(
                    *SoundState._SoundControllerPointer, (*SoundState._UnknownSoundSample1)->BufferIndex))
                {
                    self->LockLength = 0;
                    *SoundState._UnknownSoundSample1 = NULL;
                    self->Unk13 = 0;
                }
            }
        }
    }

    f32 SelectSoundSamplePlaybackPosition3(SoundSampleDescriptor* self, const f32 value, const u32 type)
    {
        auto v1 = SelectSoundSamplePlaybackPosition1(self, value, type, 2);

        if (AcquireUnknownSoundSampleDescriptorValue1(self) == 0)
        {
            v1 = Clamp(v1, 0.0f, 1.0f);// TODO constants
        }
        else
        {
            v1 = v1 - AcquireUnknownSoundValue101(v1);
        }

        return SelectSoundSamplePlaybackPosition1(self, v1, 2, type);;
    }

    void SeekSoundSample1(SoundSample* self, const s32 value, const s32 destination)
    {
        if (self->StreamIndex == 0)
        {
            LogError("Unable to seek sound sample stream, the sample is not streaming.");
        }

        auto pos = SelectSoundSamplePlaybackPosition3(&self->Descriptor, (f32)value, 0);

        self->Unk10 = (s32)roundf(pos);

        if (self->Unk10 < 0) { self->Unk10 = 0; } // TODO constants

        if (destination < 0 || self->Length <= destination)
        {
            LogError("Unable to seek sound sample stream, invalid destination position.");
        }

        self->Unk11 = destination;

        u32 size = 0;
        u32 packet = 0;

        AcquireSoundSampleDescriptorDetails(&self->Descriptor, &size, &packet);

        if (1 < size)
        {
            self->Unk10 = AcquireSoundSampleDescriptorValue201(self->Unk10, size);
            self->Unk11 = AcquireSoundSampleDescriptorValue201(self->Unk11, size);
        }

        if (self->Decoder != NULL)
        {
            if (!FUN_00515050a(self->Decoder, self->Unk10))
            {
                LogError("Unable to seek sound sample stream %s to playback position %d.",
                    self->Descriptor.Definition.Name, self->Unk10);
            }
        }
    }
    
    void SelectSoundSampleBuffer1(SoundSample* self)
    {
        if (self->StreamIndex != 0 && self == SoundState.Samples.UnknownSample)
        {
            auto sample = SoundState.Samples.UnknownSample;

            if (*SoundState.Samples._UnknownSampleValue1 == SoundState.Samples.UnknownSample->Unk10)
            {
                auto data = LockSoundSample(SoundState.Samples.UnknownSample,
                    SoundState.Samples.UnknownSample->Unk11, *SoundState.Samples._UnknownSampleValue2);

                sample = self;

                if (data != NULL)
                {
                    CopyMemory(data, &SoundState.Samples.UnknownBuffer101,
                        AcquireSoundSampleDescriptorValue203(&self->Descriptor, *SoundState.Samples._UnknownSampleValue2, 0));

                    UnlockSoundSampleBuffer1(self);
                    InitializeSoundSample5(self, *SoundState.Samples._UnknownSampleValue2);

                    return;
                }
            }

            ReleaseSoundSample(sample);
        }
    }

    void UnlockSoundSampleBuffer1(SoundSample* self)
    {
        if (self->Unk13 == 2 && 0 < self->LockLength) // TODO constants
        {
            if (self->BufferIndex == 0 || *SoundState._SoundControllerPointer == NULL)
            {
                self->LockLength = 0;
                self->Unk13 = 0;
            }
            else
            {
                if (*SoundState._UnknownSoundSample1 != NULL)
                {
                    UpdateSoundSample(*SoundState._UnknownSoundSample1);
                }

                SoundSampleDescriptor desc;

                SoundSampleDescriptorInitialize1(&desc);
                SoundSampleInitialize1(self, &desc);

                auto result = (*SoundState._SoundControllerPointer)->Self->UnlockActiveSoundSampleBuffer(
                    *SoundState._SoundControllerPointer, self->BufferIndex, self->LockOffset, self->LockLength, &desc);

                switch (result)
                {
                case 0: // TODO constant
                {
                    self->Unk13 = 1;
                    *SoundState._UnknownSoundSample1 = self;

                    break;
                }
                case 1: // TODO constant
                {
                    self->Unk13 = 0;
                    self->LockLength = 0;

                    break;
                }
                default:
                {
                    LogError("Unable to unlock sound effect.");
                }
                }
            }
        }
    }

    void InitializeSoundSample5(SoundSample* self, const s32 value)
    {
        self->Unk11 = self->Unk11 + value;
        self->Unk6 = self->Unk6 + value;
        self->Unk10 = self->Unk10 + value;

        if (self->Length <= self->Unk11)
        {
            self->Unk11 = 0;
        }

        if (-1 < self->Descriptor.Definition.Length && self->Descriptor.Definition.Length <= self->Unk10)
        {
            if (AcquireUnknownSoundSampleDescriptorValue1(&self->Descriptor) == 1) // TODO constants
            {
                self->Unk10 = 0;
            }
            else
            {
                self->Unk10 = self->Descriptor.Definition.Length;
            }

            if (self->StreamIndex == 1) // TODO constants
            {
                LogMessage("[SOUND] Initial sound %s data fill completed.\n", self->Descriptor.Definition.Name);
                
                self->StreamIndex = 0;

                self->Unk7 = -1;
                self->Unk5 = 0;
                self->Unk6 = 0;
            }
            else if (self->Unk10 == 0)
            {
                SeekSoundSample1(self, 0, self->Unk11);
            }
        }
    }
    
    void SeekSoundSample2(SoundSample* self, const s32 value)
    {
        if (self->StreamIndex != 0 && SoundState.Samples.UnknownSample == NULL
            && SoundState.Samples.AssetOperationIndex == INVALID_SOUND_ASSET_OPERATION_INDEX)
        {
            const auto val = Min(value, AcquireSoundSampleDescriptorValue204(&self->Descriptor, 0x20000, -1)); // TODO constants

            const auto pos = AcquireSoundSampleDescriptorValue203(&self->Descriptor, self->Unk10, 0);
            const auto length = AcquireSoundSampleDescriptorValue203(&self->Descriptor, val, 0);

            FileDescriptor desc;

            if (AcquireSoundSampleFileDescriptor(&self->Descriptor, &desc))
            {
                SoundState.Samples.AssetOperationIndex = InitializeSoundAssetOperation(
                    &desc, self->Descriptor.Offset + pos, length, &SoundState.Samples.UnknownBuffer101);

                if (SoundState.Samples.AssetOperationIndex != INVALID_SOUND_ASSET_OPERATION_INDEX)
                {
                    SoundState.Samples.UnknownSample = self;

                    *SoundState.Samples._UnknownSampleValue2 = val;
                    *SoundState.Samples._UnknownSampleValue1 = self->Unk10;

                    return;
                }
            }

            ReleaseSoundSample(self);
        }
    }

    void SelectSoundSampleCacheSize1(const u32 min, const u32 max)
    {
        SoundState.Samples.MinimumCacheSize = min;
        *SoundState.Samples._MaximumCacheSize = max;
        
        SelectSoundSampleCacheSize(0);
    }

    BOOL AcquireSoundSampleDescriptor3(const u32 hash, SoundSampleDescriptor* desc)
    {
        const auto effect = AcquireSoundEffect(hash, 1);

        if (effect == NULL) { return FALSE; }

        CopyMemory(desc, &effect->Sample->Descriptor, sizeof(SoundSampleDescriptor));

        UnlockSound1();

        return TRUE;
    }
}