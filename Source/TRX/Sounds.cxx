#include "ASCII.hxx"
#include "Assets.Sounds.hxx"
#include "Logger.hxx"
#include "Mathematics.Basic.hxx"
#include "Native.hxx"
#include "Sounds.Devices.hxx"
#include "Sounds.Effects.hxx"
#include "Sounds.hxx"
#include "Strings.hxx"
#include "Time.hxx"

#include <math.h>
#include <string.h>

using namespace ASCII;
using namespace IO::Path;
using namespace Strings;
using namespace IO::Streams;
using namespace Time;
using namespace Logger;
using namespace Native;
using namespace Memory;
using namespace Mathematics;
using namespace Assets::Sounds;

namespace Sounds
{
    SoundContainer SoundState;

    void AcquireSoundStatistics(AbstractOutStream* stream)
    {
        AbstractOutStreamFormat(stream, "*** Sound Sample Cache Memory Report ***\n\n");

        auto count = 0;
        auto total = 0.0f;

        for (u32 x = 0; x < 256; x++) // TODO constants
        {
            const auto sample = &SoundState.Samples._Cache[x];

            if (IsNullOrEmpty(sample->Descriptor.Definition.Name)) { continue; }

            count = count + 1;

            const auto length = sample->Descriptor.Definition.Length * 0.0009765625f;// TODO constant

            total = total + length;

            AbstractOutStreamFormat(stream,
                "Name: %-32s Type: %d  Stream: %d  Cache: %d  Hz: %d  Length: %5.1fk  Memory Size: %5.1fk\n",
                sample->Descriptor.Definition.Name, sample->Descriptor.Definition.Type,
                sample->StreamIndex, sample->Descriptor.CacheControl, sample->Descriptor.Definition.HZ,
                length, AcquireSoundSampleDescriptorValue203(&sample->Descriptor, sample->Length, 1) * 0.0009765625f); // TODO constant
        }

        AbstractOutStreamFormat(stream, "\nSound Sample Cache Memory Report Summary: %d samples, %1.fk\n", count, total);
    }

    void LockSounds(void)
    {
        if (SoundState.Lock.Mutex == NULL)
        {
            SoundState.Lock.Mutex = InitializeMutex();

            if (SoundState.Lock.Mutex == NULL)
            {
                LogError("Unable to create sound mutex.");
            }
        }

        WaitMutex(SoundState.Lock.Mutex);

        *SoundState.Lock._Count = *SoundState.Lock._Count + 1;

        if (MAX_SOUND_LOCK_COUNT <= *SoundState.Lock._Count)
        {
            LogError("Sound lock state is imbalanced.");
        }
    }

    void UnlockSound1(void)
    {
        if (*SoundState.Lock._Count < 1) // TODO
        {
            LogError("Unable to unlock sound that was not locked.");

            UnlockSound3();
        }

        if (*SoundState.Lock._Count != 1) // TODO
        {
            *SoundState.Lock._Count = *SoundState.Lock._Count + -1;

            ReleaseMutex1(SoundState.Lock.Mutex);

            return;
        }

        SelectSoundModeValues(FALSE);

        UnlockSound2();
    }

    void UnlockSound2(void)
    {
        *SoundState.Lock._Count = *SoundState.Lock._Count + -1;

        ReleaseMutex1(SoundState.Lock.Mutex);
    }

    void UnlockSound3(void)
    {
        if (*SoundState.Lock._Count != 1) // TODO
        {
            *SoundState.Lock._Count = *SoundState.Lock._Count + -1;

            ReleaseMutex1(SoundState.Lock.Mutex);

            return;
        }

        SelectSoundModeValues(FALSE);

        UnlockSound2();
    }

    BOOL AcquireSoundAvailableMode(void)
    {
        return *SoundState._SoundControllerPointer != NULL;
    }

    BOOL AcquireSoundActiveMode(void)
    {
        return SoundState.ActiveMode;
    }

    BOOL StopSoundThread(void)
    {
        SoundState.Thread.IsQuit = TRUE;

        if (!SoundState.Thread.IsActive) { return TRUE; }

        for (u32 x = 0; x < MAX_SOUND_THREAD_ACTION_ITERATION_COUNT; x++)
        {
            Wait(0.01); // TODO

            if (!SoundState.Thread.IsActive) { return TRUE; }
        }

        if (!SoundState.Thread.IsActive) { return TRUE; }

        LogMessage("[ERROR] [SOUND] Unable to stop sound thread.\n");

        return !SoundState.Thread.IsActive;
    }

    SoundMixMode AcquireSoundMixMode2(void)
    {
        return SoundState.MixMode;
    }

    void SelectSoundMixMode(const SoundMixMode mode)
    {
        if (AcquireSoundAvailableMode())
        {
            LogError("Unable to set sound mixing mode while sound device is active.");
        }

        SoundState.MixMode = mode;
    }

    BOOL IsSoundRecordingPresent(void)
    {
        return *SoundState._SoundRecordingControllerPointer != NULL;
    }

    BOOL AcquireSoundMuteMode(void)
    {
        return SoundState.MuteMode;
    }

    BOOL SelectSoundMuteMode(const BOOL mode)
    {
        SoundState.MuteMode = mode;

        if (!mode) { return FALSE; }

        LogMessage("[SOUND] Stopping sound output...\n");

        ResetSoundEffectCache1();

        if (*SoundState._SoundControllerPointer != NULL)
        {
            if (!(*SoundState._SoundControllerPointer)->Self->StopBuffers(*SoundState._SoundControllerPointer))
            {
                return FALSE;
            }
        }

        SoundState.ActiveMode = FALSE;

        ReleaseUnknownSoundMemory();

        return TRUE;
    }

    f32 AcquireMaximumSoftWareSoundLatency(void)
    {
        return SoundState.Options.MaximumSoftWareLatency;
    }

    void SelectMaximumSoftWareSoundLatency(const f32 value)
    {
        if (AcquireSoundActiveMode())
        {
            LogError("Unable to set maximum software sound latency while sound is active.");
        }

        SoundState.Options.MaximumSoftWareLatency = Clamp(value, 0.05f, 2.0f); // TODO min, max constants
    }

    void AcquireSoundOutputOptions(u32* bits, u32* channels, u32* hz)
    {
        if (bits != NULL) { *bits = *SoundState.Options._Bits; }

        if (channels != NULL) { *channels = *SoundState.Options._ChannelCount; }

        if (hz != NULL) { *hz = *SoundState.Options._HZ; }
    }

    BOOL SelectSoundOutputOptions(const u32 bits, const u32 channels, const u32 hz)
    {
        if (AcquireSoundActiveMode())
        {
            LogError("Unable to set sound output mode while device is active.");
        }

        *SoundState.Options._Bits = bits;
        *SoundState.Options._ChannelCount = channels;
        *SoundState.Options._HZ = hz;

        if (*SoundState._SoundControllerPointer != NULL)
        {
            u32 count = 0;

            if (!(*SoundState._SoundControllerPointer)->Self->CreateBuffers(
                *SoundState._SoundControllerPointer, bits, channels, hz, &count))
            {
                return FALSE;
            }

            if (count != 0)
            {
                AllocateSoundMixBuffers(count, 4);// TODO why 4 ?
            }
        }

        return TRUE;
    }

    SoundMixMode AcquireSoundMixMode1(void)
    {
        return SoundState.DirectSound.MixMode;
    }

    BOOL RestartSoundThread(const f32 value)
    {
        if (!StopSoundThread()) { return FALSE; }

        SoundState.Thread.TimeValue = Max(0.002f, value);//TODO constants

        SoundState.Thread.IsActive = FALSE;
        SoundState.Thread.IsQuit = FALSE;

        const auto thread = InitializeThread(StartSoundThread, NULL);

        if (thread != NULL)
        {
            if (SelectThreadWeight(thread, ThreadWeight::Max))
            {
                for (u32 x = 0; x < MAX_SOUND_THREAD_ACTION_ITERATION_COUNT; x++)
                {
                    if (SoundState.Thread.IsActive) { return TRUE; }

                    Wait(0.01);//TODO
                }

                return SoundState.Thread.IsActive;
            }
        }

        return FALSE;
    }

    DWORD WINAPI StartSoundThread(LPVOID)
    {
        SoundState.Thread.IsActive = TRUE;

        LogMessage("[SOUND] Sound thread starting...\n");

        while (!SoundState.Thread.IsQuit)
        {
            if (*SoundState._SoundControllerPointer != NULL && AcquireSoundActiveMode()
                && WaitSoundMutex(SoundState.Lock.Mutex, SoundState.Thread.TimeValue * 3.0f)) // TODO constant
            {
                const auto time = AcquireTime();

                if ((time - SoundState.UnknownTime) < 0 || 0x13332 < (time - SoundState.UnknownTime)) // TODO
                {
                    *SoundState.Lock._Count = *SoundState.Lock._Count + 1;
                    SoundState.UnknownTime = time;

                    (*SoundState._SoundControllerPointer)->Self->AcquireSecondaryBufferPosition(*SoundState._SoundControllerPointer);

                    UnknownSound101();
                    SelectSoundModeValues(TRUE);
                    UnlockSound1();
                }
                else
                {
                    ReleaseMutex1(SoundState.Lock.Mutex);
                }
            }

            if (SoundState.Thread.IsQuit) { break; }

            Wait(SoundState.Thread.TimeValue);
        }

        LogMessage("[SOUND] Sound thread exiting...\n");

        SoundState.Thread.IsActive = FALSE;

        return EXIT_SUCCESS;
    }

    BOOL WaitSoundMutex(const HANDLE mutex, const f64 time)
    {
        if (mutex == NULL) { return FALSE; }

        return WaitForSingleObject(mutex, (u32)round(time * 1000.0f)) == WAIT_OBJECT_0; // TODO
    }

    void ResetSoundSampleCache1(void)
    {
        LogMessage("[SOUND] Resetting sound sample cache...\n");

        LockSounds();

        ResetSoundEffectCache1();

        for (u32 x = 0; x < 256; x++) // TODO constant
        {
            SoundSampleRelease(&SoundState.Samples._Cache[x]);
        }

        UnlockSound1();

        LogMessage("[SOUND] Resetting sound sample cache completed.\n");
    }

    void ResetSoundEffectCache1(void)
    {
        LockSounds();

        for (u32 x = 0; x < 64; x++) // TODO constant
        {
            ReleaseSoundEffect(&SoundState.Effects._Cache[x]);
        }

        UnlockSound1();
    }

    BOOL StopSound1(void)
    {
        LogMessage("[SOUND] Stopping sound output...\n");

        ResetSoundEffectCache1();

        if (*SoundState._SoundControllerPointer != NULL)
        {
            if (!(*SoundState._SoundControllerPointer)->Self->StopBuffers(*SoundState._SoundControllerPointer)) { return FALSE; }
        }

        SoundState.ActiveMode = FALSE;

        ReleaseUnknownSoundMemory();

        return TRUE;
    }

    BOOL StopSound2(void)
    {
        if (!StopSound1()) { return FALSE; }

        for (u32 x = 0; x < 256; x++) // TODO constant
        {
            SoundSampleRelease(&SoundState.Samples._Cache[x]);
        }

        if (*SoundState._SoundControllerPointer == NULL) { return TRUE; }

        if (!(*SoundState._SoundControllerPointer)->Self->Initialize(*SoundState._SoundControllerPointer)) { return FALSE; }

        SoundState.DirectSound.MixMode = SoundMixMode::None;

        *SoundState._SoundControllerPointer = NULL;

        return TRUE;
    }

    void ReleaseUnknownSoundMemory(void)
    {
        if (*SoundState._UnknownMemory1 != NULL)
        {
            ReleaseMemory1(*SoundState._UnknownMemory1);

            *SoundState._UnknownMemory1 = NULL;
        }

        if (*SoundState._UnknownMemory2 != NULL)
        {
            ReleaseMemory1(*SoundState._UnknownMemory2);

            *SoundState._UnknownMemory2 = NULL;
        }

        if (SoundState.UnknownMemory3 != NULL)
        {
            ReleaseMemory1(SoundState.UnknownMemory3);

            SoundState.UnknownMemory3 = NULL;
        }

        for (u32 x = 0; x < 8; x++) // TODO constant
        {
            SoundState._UnknownArray101[x] = NULL;
            SoundState._UnknownArray102[x] = NULL;
            SoundState._UnknownArray103[x] = NULL;
            SoundState._UnknownArray104[x] = NULL;
        }

        *SoundState._UnknownValue201 = 0;
        *SoundState._UnknownValue202 = 0;
        *SoundState._UnknownValue203 = 0;
        *SoundState._UnknownValue204 = 0;
    }

    BOOL StopSoundRecording1(void)
    {
        if (!StopSoundRecording2()) { return FALSE; }

        if (*SoundState._SoundRecordingControllerPointer == NULL) { return TRUE; }

        if ((*SoundState._SoundRecordingControllerPointer)->Self->Initialize(*SoundState._SoundRecordingControllerPointer)) { return FALSE; }

        *SoundState._SoundRecordingControllerPointer = NULL;

        return TRUE;
    }

    BOOL StopSoundRecording2(void)
    {
        LockSounds();

        if (*SoundState._SoundRecordingControllerPointer != NULL)
        {
            if (!(*SoundState._SoundRecordingControllerPointer)->Self->Stop(*SoundState._SoundRecordingControllerPointer))
            {
                UnlockSound1();

                return FALSE;
            }
        }

        *SoundState._SoundRecordingActive = FALSE;

        UnlockSound1();

        return TRUE;
    }

    f32 AcquireUnknownSoundValue101(const f32 value)
    {
        auto result = 0.0f;

        if (modff(value, &result) < 0.0f)
        {
            result = result - 1.0f;
        }

        return result;
    }

    s32 AcquireSoundSampleDescriptorValue201(const s32 offset, const s32 v2)
    {
        return offset - offset % v2;
    }

    s32 AcquireSoundSampleDescriptorValue202(const s32 offset, const s32 v2)
    {
        if (offset % v2 == 0) { return offset; }

        return (offset + v2) - offset % v2;
    }

    s32 AcquireSoundSampleDescriptorValue203(SoundSampleDescriptor* self, const s32 offset, const s32 v2) // TODO
    {
        u32 size = 0;
        u32 packet = 0;

        AcquireSoundSampleDescriptorDetails(self, &size, &packet);

        auto value = offset;

        if (v2 < 0)
        {
            value = AcquireSoundSampleDescriptorValue201(offset, size);
        }
        else if (0 < v2)
        {
            value = AcquireSoundSampleDescriptorValue202(offset, size);
        }

        return packet * value / size;
    }

    s32 AcquireSoundSampleDescriptorValue204(SoundSampleDescriptor* self, const s32 offset, const s32 v2) // TODO
    {
        u32 size = 0;
        u32 packet = 0;

        AcquireSoundSampleDescriptorDetails(self, &size, &packet);

        auto value = offset;

        if (v2 < 0)
        {
            value = AcquireSoundSampleDescriptorValue201(offset, size);
        }
        else if (0 < v2)
        {
            value = AcquireSoundSampleDescriptorValue202(offset, size);
        }

        return size * value / packet;
    }

    void UnknownPlay101(void)
    {
        if (*SoundState.Lock._Count < 1)
        {
            LogError("Unable to poll sound effect, sound sample is not locked.");
        }

        auto time1 = AcquireTime(); // TODO type
        auto time2 = time1 - SoundState.SoundTime1; // TODO type

        if (time2 < 0 || 0x1cccb < time2) // TODO constants
        {
            SoundState.SoundTime1 = time1;

            if (SoundState.SoundTimeUnknown1 < 2) // TODO constants
            {
                SelectSoundLocations1();
            }

            f32 volume = 0.0f; // TODO constants

            if (AcquireSoundMixMode1() != SoundMixMode::None)
            {
                volume = Clamp(time2 * 0.0000008477105f, 0.0f, 0.25f); // TODO constants
            }

            for (u32 x = 0; x < 64; x++) // TODO: constants
            {
                auto effect = &SoundState.Effects._Cache[x];

                if (InitializeSoundEffectVolume(effect, volume))
                {
                    ReleaseSoundEffect1(effect);
                }
            }
        }
    }

    void SelectListenerDistanceFactor(const f32 value)
    {
        SoundState.Listener.InverseDistanceFactor = 1.0f / value;
        SoundState.Listener.DistanceFactor = value;

        if (*SoundState._SoundControllerPointer == NULL) { return; }

        (*SoundState._SoundControllerPointer)->Self->SetListenerDistanceFactor(*SoundState._SoundControllerPointer, value);
    }

    BOOL InitializeSoundSampleDescriptor1(SoundSampleDescriptor* self, char* outMessage)
    {
        char name[STANDARD_IO_FILE_NAME_LENGTH];
        InStreamFile stream;

        sprintf(name, SOUND_SFX_FILE_NAME_TEMPLATE, self->Definition.Name);// TODO constant

        InStreamFileInitialize(&stream);

        if (InStreamFileOwn(&stream, SOUND_DIRECTORY_NAME, name, TRUE))
        {
            auto result = InitializeSoundSampleDescriptor2(self, &stream, outMessage);

            InStreamFileDispose(&stream, DisposeMode::None);

            return result;
        }

        InStreamFileDispose(&stream, DisposeMode::None);

        return TRUE;
    }

    s32 AcquireSoundFrequency(const f32 value)
    {
        return Clamp((s32)roundf(value), DSBFREQUENCY_MIN, DSBFREQUENCY_MAX);
    }

    s32 AcquireUnknownSoundValue102(const f32 value)
    {
        if (value <= 0.0f) { return DSBVOLUME_MIN; }// TODO constant

        if (1.0f <= value) { return DSBVOLUME_MAX; }// TODO constant

        const auto result = (s32)roundf(AcquireUnknownSoundValue101(log2f(value) * 868.589f));// TODO constant

        return Clamp(result, DSBVOLUME_MIN, DSBVOLUME_MAX);
    }

    u32 AcquireSoundChannelCount(void)
    {
        AcquireSoundOutputOptions(NULL, SoundState.Options._ChannelCount, NULL);

        return *SoundState.Options._ChannelCount;
    }

    void InitializeSoundRecordDescriptor(SoundSampleRecordDescriptor* self, const SoundSampleDescriptor* desc)
    {
        strcpy(self->Name, desc->Definition.Name);

        self->Length = desc->Definition.Length;
        self->Offset = desc->Offset;

        self->ReferenceDistance = desc->ReferenceDistance;
        self->MinimumDistance = desc->MinimumDistance;
        self->MaximumDistance = desc->MaximumDistance;

        self->Priority = desc->Priority;

        self->HZ = desc->Definition.HZ;
        self->Type = desc->Definition.Type;
        self->BitsPerSample = desc->Definition.BitsPerSample;
        self->Channels = desc->Definition.Channels;
        self->CacheControl = desc->CacheControl;

        self->Unknown1 = AcquireUnknownSoundSampleDescriptorValue1(desc);;
    }

    void InitializeSoundSampleDescriptor3(const SoundSampleRecordDescriptor* self, SoundSampleDescriptor* desc)
    {
        strcpy(desc->Definition.Name, self->Name);

        desc->Definition.Length = self->Length;
        desc->Offset = self->Offset;

        desc->ReferenceDistance = self->ReferenceDistance;
        desc->MinimumDistance = self->MinimumDistance;
        desc->MaximumDistance = self->MaximumDistance;

        desc->Priority = self->Priority;

        desc->Definition.HZ = self->HZ;
        desc->Definition.Type = self->Type;
        desc->Definition.BitsPerSample = self->BitsPerSample;
        desc->Definition.Channels = self->Channels;
        desc->LoopMode = 1; // TODO
        desc->CacheControl = self->CacheControl;
        desc->ChannelLength[0] = self->Length;

        desc->Unk6 = (self->Unknown1 != 0) - 1;
    }

    BOOL AcquireSoundSampleDescriptor1(SoundSampleDescriptor* self)
    {
        for (u32 x = 0; x < SoundState.Records.Cache.Count; x++)
        {
            const auto item = &SoundState.Records.Cache.Items[x];

            if (EqualStrings(self->Definition.Name, item->Name))
            {
                InitializeSoundSampleDescriptor3(item, self);

                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL AcquireSoundSampleDescriptor2(SoundSampleDescriptor* self)
    {
        LockSounds();

        ChangeFileExtension(self->Definition.Name, NULL);

        for (u32 x = 0; x < 256; x++) // TODO constants
        {
            const auto item = &SoundState.Samples._Cache[x];

            if (EqualStrings(self->Definition.Name, item->Descriptor.Definition.Name))
            {
                CopyMemory(self, &item->Descriptor, sizeof(SoundSampleDescriptor));

                UnlockSound1();

                return TRUE;
            }
        }

        if (SoundState.Records.Cache.Count < 1)
        {
            UnlockSound1();

            char buffer[256]; // TODO

            if (!InitializeSoundSampleDescriptor1(self, buffer))
            {
                LogMessage("[ERROR] [SOUND] Unable to acquire sound sample %s options. %s.\n", self->Definition.Name, buffer);

                return FALSE;
            }

            SoundSample sample;

            SoundSampleInitialize(&sample);

            CopyMemory(&sample.Descriptor, self, sizeof(SoundSampleDescriptor));

            InStreamFile stream;

            InStreamFileInitialize(&stream);

            if (!LoadSoundSample1(&sample, &stream))
            {
                InStreamFileDispose(&stream, DisposeMode::None);
                SoundSampleDispose(&sample, DisposeMode::None);

                return FALSE;
            }

            InStreamFileRelease(&stream);

            CopyMemory(self, &sample.Descriptor, sizeof(SoundSampleDescriptor));

            InStreamFileDispose(&stream, DisposeMode::None);
            SoundSampleDispose(&sample, DisposeMode::None);

            return TRUE;
        }

        if (AcquireSoundSampleDescriptor1(self))
        {
            UnlockSound1();

            return TRUE;
        }

        UnlockSound1();

        LogMessage("[ERROR] [SOUND] Unable to find sound sample %s in the sound sample collection.\n", self->Definition.Name);

        return FALSE;
    }

    BOOL InitializeUnknownSound1(const char* name, const s32 type) // TODO enum?
    {
        LockSounds();

        SoundSampleDescriptor desc;

        SoundSampleDescriptorInitialize1(&desc);
        SelectSoundSampleDescriptorName(&desc, name);

        for (u32 x = 0; x < 256; x++) // TODO constant
        {
            const auto sample = &SoundState.Samples._Cache[x];

            if (EqualStrings(sample->Descriptor.Definition.Name, desc.Definition.Name))
            {
                UnlockSound1();

                return TRUE;
            }
        }

        if (!AcquireSoundSampleDescriptor1(&desc))
        {
            if (0 < SoundState.Records.Cache.Count)
            {
                LogMessage("[ERROR] [SOUND] Unable to find sound sample %s in the sound sample collection. Attempting to find the file on the disk.\n", desc.Definition.Name);
            }

            char buffer[256];// TODO

            if (!InitializeSoundSampleDescriptor1(&desc, buffer))
            {
                LogMessage("[ERROR] [SOUND] Unable to load %s.sfx file. %s.\n", desc.Definition.Name, buffer);

                UnlockSound1();

                return FALSE;
            }
        }

        if (desc.CacheControl == SoundCacheMode::Stream)
        {
            LogMessage("[ERROR] [SOUND] Ignoring request to cache streaming sound sample %s.\n", name);

            UnlockSound1();

            return FALSE;
        }

        InStreamFile stream;

        InStreamFileInitialize(&stream);

        auto sample = AcquireSoundSample1();

        if (sample == NULL)
        {
            UnlockSound1();

            InStreamFileDispose(&stream, DisposeMode::None);

            return FALSE;
        }

        CopyMemory(&sample->Descriptor, &desc, sizeof(SoundSampleDescriptor));

        if (LoadSoundSample1(sample, &stream))
        {
            sample->Unk7 = -1; // TODO Constant
            sample->Unk6 = 0;
            sample->Unk5 = 0;
            sample->StreamIndex = 0;
            sample->Length = sample->Descriptor.Definition.Length;
            sample->Unk10 = sample->Descriptor.Definition.Length;
            sample->Unk11 = sample->Descriptor.Definition.Length;

            if (type == 2) // TODO constant
            {
                sample->Descriptor.CacheControl = SoundCacheMode::Static;
            }

            if (AllocateSoundsSample(sample))
            {
                InStreamFileSeek(&stream, sample->Descriptor.Offset, StreamSeek::Set);

                if (LoadSoundSample2(sample, &stream))
                {
                    InStreamFileRelease(&stream);

                    LogMessage("[SOUND] Sound sample %s is cached (%d bytes).\n", sample->Descriptor.Definition.Name, sample->AllocatedMemorySize);

                    UnlockSound1();

                    InStreamFileDispose(&stream, DisposeMode::None);

                    return TRUE;
                }
            }
        }

        SoundSampleRelease(sample);

        UnlockSound1();

        InStreamFileDispose(&stream, DisposeMode::None);

        return FALSE;
    }

    void* LockSoundSample(SoundSample* self, const s32 offset, const u32 length)
    {
        if (self->LockLength != 0 || self->Unk13 != 0)
        {
            LogError("Unable to lock sound sample, it is already locked.");
        }

        if (offset < 0 || self->Length < (offset + length))
        {
            LogError("Unable to lock sound sample, invalid memory region selected.");
        }

        if (self->BufferIndex == 0)
        {
            if (self->Descriptor.AllocatedMemory1 == NULL) { return NULL; }

            auto result = (void*)((addr)(self->Descriptor.AllocatedMemory1)
                + AcquireSoundSampleDescriptorValue203(&self->Descriptor, offset, 0));

            self->Unk13 = 2;// TODO
            self->LockOffset = offset;
            self->LockLength = length;

            return result;
        }

        if (*SoundState._SoundControllerPointer == NULL) { return NULL; }

        SoundSampleDescriptor desc;

        SoundSampleDescriptorInitialize1(&desc);
        SoundSampleInitialize1(self, &desc);

        auto result = (*SoundState._SoundControllerPointer)->Self->LockSample(
            *SoundState._SoundControllerPointer, self->BufferIndex, offset, length, &desc);

        if (result != NULL)
        {
            self->Unk13 = 2;// TODO
            self->LockOffset = offset;
            self->LockLength = length;
        }

        return result;
    }

    BOOL InitializeSoundSampleDescriptor2(SoundSampleDescriptor* self, InStreamFile* stream, char* outMessage)
    {
        /*
        The SFX dile is a text file with the sound sample configuration, it looks something like this:

        loop
        refDist=50
        maxVol=1.5

        NOTES:
        1. It is a legacy format and not being used in the game.
        2. It may contain comments starting with //
        3. It also looks like leading spaces at the beginning of the line should be ignored.
        */

        u32 lineNumber = 0;
        char line[300];// TODO constant

        u32 referenceLineNumber = 0;
        u32 minimumLineNumber = 0;

        f32 maximumVolume = 0.0f;

        s32 length = 0;

        while (InStreamFileReadString(stream, line, 300)) // TODO constant
        {
            lineNumber = lineNumber + 1;

            // Skip lines that are comment.
            if (StartsWith(line, "//")) { continue; } // TODO constant

            u32 indx = 0;
            
            // Skip leading spaces.
            while (IsSpace(line[indx])) { indx = indx + 1; }

            char* str = &line[indx];

            if (StartsWith(str, "refDist")) // TODO constant
            {
                if (sscanf(str, "refDist =%f", &self->ReferenceDistance) == 1) // TODO constant
                {
                    if (referenceLineNumber == 0)
                    {
                        referenceLineNumber = lineNumber;

                        if (self->ReferenceDistance < 0.1f)
                        {
                            sprintf(outMessage, "Reference volume distance %f is too small, line %d",self->ReferenceDistance, lineNumber);
                            
                            return FALSE;
                        }

                        if (minimumLineNumber == 0)
                        {
                            self->MinimumDistance = self->ReferenceDistance / 4.0f; // TODO constant
                        }
                    }
                    else
                    {
                        sprintf(outMessage, "Reference distance specified on line %d, then again on line %d",
                            referenceLineNumber, lineNumber);
                        
                        return FALSE;
                    }
                }
            }
            else if (StartsWith(str, "minDist")) // TODO constant
            {
                if (sscanf(str, "minDist =%f", &self->MinimumDistance) == 1) // TODO constant
                {
                    if (minimumLineNumber == 0)
                    {
                        minimumLineNumber = lineNumber;

                        if (self->MinimumDistance < 0.1f)
                        {
                            sprintf(outMessage, "Minimum volume distance %f is too small, line %d", self->ReferenceDistance, lineNumber);

                            return FALSE;
                        }
                    }
                    else
                    {
                        sprintf(outMessage, "Minimum distance specified on line %d, then again on line %d",
                            minimumLineNumber, lineNumber);

                        return FALSE;
                    }
                }
            }
            else if (StartsWith(str, "maxDist")) // TODO constant
            {
                sscanf(str, "maxDist =%f", &self->MaximumDistance);
            }
            else if (StartsWith(str, "maxVol")) // TODO constant
            {
                if (sscanf(str, "maxVol =%f", &maximumVolume) == 1)
                {
                    if (referenceLineNumber == 0)
                    {
                        sprintf(outMessage, "Maximum volume specified on line %d without specifying reference distance first", lineNumber);

                        return FALSE;
                    }

                    if (minimumLineNumber == 0)
                    {
                        minimumLineNumber = lineNumber;

                        if (maximumVolume < 0.001f) // TODO constant
                        {
                            sprintf(outMessage, "Maximum volume %f is too small, line %d", maximumVolume, lineNumber);
                            
                            return FALSE;
                        }
                        self->MinimumDistance = self->ReferenceDistance / maximumVolume;
                    }
                    else
                    {
                        sprintf(outMessage, "Minimum distance specified on line %d, then again on line %d",
                            minimumLineNumber, lineNumber);

                        return FALSE;
                    }
                }
            }
            else if (StartsWith(str, "length")) // TODO constant
            {
                if (sscanf(str, "length =%d", &length) == 1)
                {
                    if (length < 1)
                    {
                        sprintf(outMessage, "Invalid length %d, line %d", length, lineNumber);

                        return FALSE;
                    }

                    if (0 < self->Definition.Length && self->Definition.Length != length)
                    {
                        sprintf(outMessage, "Length %d does not match previously known value of %d, line %d",
                            length, self->Definition.Length, lineNumber);

                        return FALSE;
                    }
                    
                    self->Definition.Length = length;
                }
            }
            else if (StartsWith(str, "priority")) // TODO constant
            {
                sscanf(str, "priority = %d", &self->Priority);
            }
            else if (StartsWith(str, "cacheControl")) // TODO constant
            {
                char cache[64];//TODO constant

                if (sscanf(str, "cacheControl = %s", cache) == 1)
                {
                    if (EqualStrings(cache, "normal")) // TODO constant
                    {
                        self->CacheControl = SoundCacheMode::Normal;
                    }
                    else if (EqualStrings(cache, "static")) // TODO constant
                    {
                        self->CacheControl = SoundCacheMode::Static;
                    }
                    else if (EqualStrings(cache, "stream")) // TODO constant
                    {
                        self->CacheControl = SoundCacheMode::Stream;
                    }
                    else
                    {
                        sprintf(outMessage, "Invalid cache mode %s on line %d", cache, lineNumber);

                        return FALSE;
                    }
                }
                else
                {
                    sprintf(outMessage, "Unable to parse cache mode, line %d, value %s", lineNumber, str);

                    return FALSE;
                }
            }
            else if (EqualStrings(str, "loop")) // TODO constant
            {
                self->LoopMode = 1; // TODO
                self->Unk6 = 0;
                self->ChannelLength[0] = self->Definition.Length;
            }
        }

        SelectSoundSampleDescriptor1(self);

        return TRUE;
    }

    void AcquireSoundSample2(const char* file)
    {
        SoundState.Records.Names.Count = 0;

        char name[STANDARD_IO_FILE_NAME_LENGTH];

        SplitPath(file, NULL, NULL, name, NULL);

        u32 length = 0;

        while (name[length] != NULL && name[length] != ASCII_QUESTION_MARK && name[length] != ASCII_ASTERISK)
        {
            length = length + 1;
        }

        if (length < 1)
        {
            LogMessage("[ERROR] [SOUND] Unable to acquire a sound sample, the name contains wildcard, value %s\n", name);

            return;
        }

        for (u32 x = 0; x < SoundState.Records.Cache.Count; x++)
        {
            if (strnicmp(name, SoundState.Records.Cache.Items[x].Name, length) == 0)
            {
                strcpy(SoundState.Records.Names.Items[SoundState.Records.Names.Count], SoundState.Records.Cache.Items[x].Name);

                SoundState.Records.Names.Count = SoundState.Records.Names.Count + 1;

                if (SoundState.Records.Names.Count == 40) { return; } // TODO constant
            }
        }
    }

    BOOL StartSound(void)
    {
        LogMessage("[SOUND] Starting sound output...\n");

        if (!AcquireSoundAvailableMode())
        {
            LogMessage("[ERROR] [SOUND] No sound devices available.\n");

            return FALSE;
        }

        if (AcquireSoundMuteMode())
        {
            LogMessage("[ERROR] [SOUND] Sound output is disabled (muted).\n");

            return StopSound1();
        }

        if (AcquireSoundActiveMode())
        {
            LogMessage("[ERROR] [SOUND] Sound output is already active.\n");

            return TRUE;
        }

        LockSounds();

        if (!SelectSoundOutputOptions(*SoundState.Options._Bits, *SoundState.Options._ChannelCount, *SoundState.Options._HZ))
        {
            LogMessage("[ERROR] [SOUND] Sound output is already active.\n");

            UnlockSound1();

            return FALSE;
        }

        if (0 < *SoundState._UnknownValue201 && 0 < *SoundState.Options._ChannelCount)
        {
            for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
            {
                ZeroMemory(&(SoundState._UnknownArray104[x]),
                    *SoundState._UnknownValue204 * *SoundState._UnknownValue201 * 4); // TODO
            }
        }

        *SoundState._UnknownValue202 = 0;
        *SoundState._UnknownValue203 = 0;

        (*SoundState._SoundControllerPointer)->Self->SelectListenerPosition(
            *SoundState._SoundControllerPointer,
            SoundState.Listener.Positions.X[SoundState.Listener.Index],
            SoundState.Listener.Positions.Y[SoundState.Listener.Index],
            SoundState.Listener.Positions.Z[SoundState.Listener.Index]);

        (*SoundState._SoundControllerPointer)->Self->SelectListenerOrientation(
            *SoundState._SoundControllerPointer,
            SoundState.Listener.Orientation.X1[SoundState.Listener.Index],
            SoundState.Listener.Orientation.Y1[SoundState.Listener.Index],
            SoundState.Listener.Orientation.Z1[SoundState.Listener.Index],
            SoundState.Listener.Orientation.XT[SoundState.Listener.Index],
            SoundState.Listener.Orientation.YT[SoundState.Listener.Index],
            SoundState.Listener.Orientation.ZT[SoundState.Listener.Index],
            SoundState.Listener.Orientation.XF[SoundState.Listener.Index],
            SoundState.Listener.Orientation.XF[SoundState.Listener.Index],
            SoundState.Listener.Orientation.ZF[SoundState.Listener.Index]);

        (*SoundState._SoundControllerPointer)->Self->SetListenerVelocity(
            *SoundState._SoundControllerPointer,
            SoundState.Listener.Velocity.X[SoundState.Listener.Index],
            SoundState.Listener.Velocity.Y[SoundState.Listener.Index],
            SoundState.Listener.Velocity.Z[SoundState.Listener.Index]);

        (*SoundState._SoundControllerPointer)->Self->SetListenerDistanceFactor(
            *SoundState._SoundControllerPointer, SoundState.Listener.DistanceFactor);

        (*SoundState._SoundControllerPointer)->Self->SelectEnvironmentalAudioExtensionsEnvironment(
            *SoundState._SoundControllerPointer,
            SoundState.Listener.EAX.Index,
            SoundState.Listener.EAX.UnknownValue1, SoundState.Listener.EAX.UnknownValue2);

        SoundState.SoundTime1 = AcquireTime();

        if ((*SoundState._SoundControllerPointer)->Self->StartSecondaryBuffer(*SoundState._SoundControllerPointer))
        {
            LogMessage("[SOUND] Sound output successfully started.\n");

            SoundState.ActiveMode = TRUE;

            UnlockSound1();

            return TRUE;
        }

        LogMessage("[ERROR] [SOUND] Unable to start sound output.\n");

        UnlockSound1();

        return FALSE;
    }

    void SelectListenerVelocity(const f32 x, const f32 y, const f32 z)
    {
        LockSounds();

        SoundState.Listener.Velocity.X[SoundState.Listener.Index] = x;
        SoundState.Listener.Velocity.Y[SoundState.Listener.Index] = y;
        SoundState.Listener.Velocity.Z[SoundState.Listener.Index] = z;

        SoundState.UnknownValue301 = SoundState.UnknownValue301 | 4; // TODO

        UnlockSound1();
    }

    void SelectListenerOrientation(const f32 x1, const f32 y1, const f32 z1, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf)
    {
        LockSounds();

        SoundState.Listener.Orientation.X1[SoundState.Listener.Index] = x1;
        SoundState.Listener.Orientation.Y1[SoundState.Listener.Index] = y1;
        SoundState.Listener.Orientation.Z1[SoundState.Listener.Index] = z1;
        SoundState.Listener.Orientation.XT[SoundState.Listener.Index] = xt;
        SoundState.Listener.Orientation.YT[SoundState.Listener.Index] = yt;
        SoundState.Listener.Orientation.ZT[SoundState.Listener.Index] = zt;
        SoundState.Listener.Orientation.XF[SoundState.Listener.Index] = xf;
        SoundState.Listener.Orientation.XF[SoundState.Listener.Index] = yf;
        SoundState.Listener.Orientation.ZF[SoundState.Listener.Index] = zf;

        SoundState.UnknownValue301 = SoundState.UnknownValue301 | 0x20FFFFFF; // TODO
        
        UnlockSound1();
    }

    void SelectListenerPosition(const f32 x, const f32 y, const f32 z)
    {
        LockSounds();

        SoundState.Listener.Positions.X[SoundState.Listener.Index] = x;
        SoundState.Listener.Positions.Y[SoundState.Listener.Index] = y;
        SoundState.Listener.Positions.Z[SoundState.Listener.Index] = z;

        SoundState.UnknownValue301 = SoundState.UnknownValue301 | 2; // TODO

        UnlockSound1();
    }

    u32 AcquireSoundListenerIndex(const f32 x, const f32 y, const f32 z)
    {
        if (SoundState.SoundTimeUnknown1 < 2) { return 0; }

        auto pos =
            (x - SoundState.Listener.Positions.X[0]) * (x - SoundState.Listener.Positions.X[0])
            + (y - SoundState.Listener.Positions.Y[0]) * (y - SoundState.Listener.Positions.Y[0])
            + (z - SoundState.Listener.Positions.Z[0]) * (z - SoundState.Listener.Positions.Z[0]);

        u32 result = 0;

        for (u32 xx = 1; xx < SoundState.SoundTimeUnknown1; xx++)
        {
            const auto dx = x - SoundState.Listener.Positions.X[xx];
            const auto dy = y - SoundState.Listener.Positions.Y[xx];
            const auto dz = z - SoundState.Listener.Positions.Z[xx];

            const auto position = dz * dz + dy * dy + dx * dx;

            if (position < pos)
            {
                result = xx;
                pos = position;
            }
        }

        return result;
    }

    BOOL UnknownListenerPositionCheck(const f32 x, const f32 y, const f32 z, const f32 value)
    {
        return
            (x - SoundState.Listener.Positions.X[SoundState.Listener.Index]) * (x - SoundState.Listener.Positions.X[SoundState.Listener.Index])
            + (y - SoundState.Listener.Positions.Y[SoundState.Listener.Index]) * (y - SoundState.Listener.Positions.Y[SoundState.Listener.Index])
            + (z - SoundState.Listener.Positions.Z[SoundState.Listener.Index]) * (z - SoundState.Listener.Positions.Z[SoundState.Listener.Index]) <= value * value;
    }

    void SelectSoundLocations1(void)
    {
        if (*SoundState.Options._ChannelCount == 2)
        {
            const auto value = SoundState.Listener.InverseDistanceFactor * 0.5f; // TODO constant

            SoundState.Locations.X[0] =
                SoundState.Listener.Positions.X[SoundState.Listener.Index] -
                SoundState.Listener.Orientation.X1[SoundState.Listener.Index] * value;

            SoundState.Locations.Y[0] =
                SoundState.Listener.Positions.Y[SoundState.Listener.Index] -
                SoundState.Listener.Orientation.Y1[SoundState.Listener.Index] * value;

            SoundState.Locations.Z[0] =
                SoundState.Listener.Positions.Z[SoundState.Listener.Index] -
                SoundState.Listener.Orientation.Z1[SoundState.Listener.Index] * value;

            SoundState.Locations.X[1] =
                SoundState.Listener.Positions.X[SoundState.Listener.Index] +
                SoundState.Listener.Orientation.X1[SoundState.Listener.Index] * value;

            SoundState.Locations.Y[1] =
                SoundState.Listener.Positions.Y[SoundState.Listener.Index] +
                SoundState.Listener.Orientation.Y1[SoundState.Listener.Index] * value;

            SoundState.Locations.Z[1] =
                SoundState.Listener.Positions.Z[SoundState.Listener.Index] +
                SoundState.Listener.Orientation.Z1[SoundState.Listener.Index] * value;

            return;
        }

        for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
        {
            SoundState.Locations.X[x] = SoundState.Listener.Positions.X[SoundState.Listener.Index];
            SoundState.Locations.Y[x] = SoundState.Listener.Positions.Y[SoundState.Listener.Index];
            SoundState.Locations.Z[x] = SoundState.Listener.Positions.Z[SoundState.Listener.Index];
        }
    }

    void SelectSoundModeValues(const BOOL mode) // TODO enum?
    {
        if (*SoundState._SoundControllerPointer == NULL) { return; }

        if (AcquireSoundMixMode1() == SoundMixMode::None) { return; }

        if (!AcquireSoundActiveMode()) { return; }

        const auto time = AcquireTime();

        if (mode || (time - *SoundState._UnknownSoundTime2) < 0
            || 0x9998 < (time - *SoundState._UnknownSoundTime2)) // TODO constant
        {
            *SoundState._UnknownSoundTime2 = time;

            for (u32 x = 0; x < 64; x++)// TODO constant
            {
                auto effect = &SoundState.Effects._Cache[x];

                if (effect->UnknownIndex != 0)
                {
                    if ((effect->Descriptor.Unk30 & 1) == 0) // TODO constant
                    {
                        if (SoundState.DirectSound.MixMode == SoundMixMode::Simple)
                        {
                            effect->Options = effect->Options | SoundState.UnknownValue301;
                        }
                    }
                    else
                    {
                        effect->Options = effect->Options & 0xdfffff79; // TODO constant
                    }

                    if (effect->Options != 0)
                    {
                        if ((effect->Options & 0xbfffffff) == 0) // TODO constant
                        {
                            if ((effect->Options & 0x40000000) == 0) { continue; } // TODO constant

                            if (effect->Sample != NULL)
                            {
                                if (effect->Sample->StreamIndex != 0)
                                {
                                    auto value = effect->Sample->Descriptor.Definition.HZ;

                                    if (effect->Sample->Length < value)
                                    {
                                        value = effect->Sample->Length;
                                    }

                                    if (effect->Sample->Unk6 < value) { continue; }
                                }

                                if ((*SoundState._SoundControllerPointer)->Self->StartSoundEffect(*SoundState._SoundControllerPointer, effect))
                                {
                                    effect->Options = effect->Options & 0xbfffffff; // TODO

                                    continue;
                                }
                            }
                        }
                        else
                        {
                            InitializeSoundEffectVolume(effect, 0.0f);

                            if ((*SoundState._SoundControllerPointer)->Self->SelectSoundEffectOptions(
                                *SoundState._SoundControllerPointer, effect, effect->Options & 0xbfffffff)) // TODO
                            {
                                if ((effect->Options & 0x40000000) == 0) { continue; } // TODO

                                if (effect->Sample != NULL)
                                {
                                    if (effect->Sample->StreamIndex != 0)
                                    {
                                        auto value = effect->Sample->Descriptor.Definition.HZ;

                                        if (effect->Sample->Length < value)
                                        {
                                            value = effect->Sample->Length;
                                        }

                                        if (effect->Sample->Unk6 < value) { continue; }
                                    }

                                    if ((*SoundState._SoundControllerPointer)->Self->StartSoundEffect(*SoundState._SoundControllerPointer, effect))
                                    {
                                        effect->Options = effect->Options & 0xbfffffff; // TODO

                                        continue;
                                    }
                                }
                            }
                        }

                        ReleaseSoundEffect(effect);
                    }
                }
            }

            if (SoundState.UnknownValue301 & 2) // TODO
            {
                (*SoundState._SoundControllerPointer)->Self->SelectListenerPosition(
                    *SoundState._SoundControllerPointer,
                    SoundState.Listener.Positions.X[0], SoundState.Listener.Positions.Y[0], SoundState.Listener.Positions.Z[0]);
            }

            if (SoundState.UnknownValue301 & 0x20000000) // TODO
            {
                (*SoundState._SoundControllerPointer)->Self->SelectListenerOrientation(
                    *SoundState._SoundControllerPointer,
                    SoundState.Listener.Orientation.X1[0],
                    SoundState.Listener.Orientation.Y1[0],
                    SoundState.Listener.Orientation.Z1[0],
                    SoundState.Listener.Orientation.XT[0],
                    SoundState.Listener.Orientation.YT[0],
                    SoundState.Listener.Orientation.ZT[0],
                    SoundState.Listener.Orientation.XF[0],
                    SoundState.Listener.Orientation.XF[0],
                    SoundState.Listener.Orientation.ZF[0]);
            }

            if (SoundState.UnknownValue301 & 4) // TODO
            {
                (*SoundState._SoundControllerPointer)->Self->SetListenerVelocity(*SoundState._SoundControllerPointer,
                    SoundState.Listener.Velocity.X[0], SoundState.Listener.Velocity.Y[0], SoundState.Listener.Velocity.Z[0]);
            }

            SoundState.UnknownValue301 = 0;

            (*SoundState._SoundControllerPointer)->Self->CommitSoundEffectSettings(*SoundState._SoundControllerPointer);
        }
    }

    void UnknownSound101(void)
    {
        LockSounds();

        if (*SoundState._UnknownSoundSample1 != NULL)
        {
            AssignUnknownSoundSample1(*SoundState._UnknownSoundSample1);
        }

        if (SoundState.Samples.AssetOperationIndex != INVALID_SOUND_ASSET_OPERATION_INDEX)
        {
            const auto state = AcquireSoundAssetOperationState(SoundState.Samples.AssetOperationIndex);

            if (state == SoundAssetOperationState::Available || state == SoundAssetOperationState::None)
            {
                ReleaseSoundSample(SoundState.Samples.UnknownSample);

                if (SoundState.Samples.AssetOperationIndex != INVALID_SOUND_ASSET_OPERATION_INDEX)
                {
                    AwaitSoundAssetAvailability(SoundState.Samples.AssetOperationIndex);
                    
                    SoundState.Samples.AssetOperationIndex = INVALID_SOUND_ASSET_OPERATION_INDEX;
                }

                *SoundState.Samples._UnknownSampleValue1 = -1;
                *SoundState.Samples._UnknownSampleValue2 = -1;

                SoundState.Samples.UnknownSample = NULL;
            }
            else if (state == SoundAssetOperationState::Completed)
            {
                if (*SoundState._UnknownSoundSample1 == NULL)
                {
                    SelectSoundSampleBuffer1(SoundState.Samples.UnknownSample);

                    if (SoundState.Samples.AssetOperationIndex != 0)
                    {
                        AwaitSoundAssetAvailability(SoundState.Samples.AssetOperationIndex);
                        
                        SoundState.Samples.AssetOperationIndex = INVALID_SOUND_ASSET_OPERATION_INDEX;
                    }

                    *SoundState.Samples._UnknownSampleValue1 = -1;
                    *SoundState.Samples._UnknownSampleValue2 = -1;

                    SoundState.Samples.UnknownSample = NULL;
                }
            }
        }

        auto minValue = 0;
        SoundSample* sample = NULL;

        {
            auto minPropirity = -999999;// TODO constant
            auto maxValue = 9999.9f;// TODO constant

            for (u32 x = 0; x < 256; x++)// TODO constant
            {
                auto smpl = &SoundState.Samples._Cache[x];
                auto val = FUN_00571e10(smpl);

                if (0 < val && SoundState.Samples.UnknownSample == NULL)
                {
                    auto value = 999.9f;// TODO constant

                    if (-1 < smpl->Unk7)
                    {
                        value = (f32)smpl->Unk6 / (f32)smpl->Descriptor.Definition.HZ;
                    }

                    if (minPropirity < smpl->Descriptor.Priority
                        || (smpl->Descriptor.Priority == minPropirity && value < maxValue))
                    {
                        sample = smpl;
                        minPropirity = smpl->Descriptor.Priority;

                        maxValue = value;
                        minValue = val;
                    }
                }
            }
        }

        if (sample != NULL)
        {
            SeekSoundSample2(sample, minValue);
        }
        
        UnlockSound1();
    }

    BOOL SelectSoundSampleCacheSize(const s32 size)
    {
        u32 total = 0;

        for (u32 x = 0; x < 256; x++) // TODO constant
        {
            const auto sample = &SoundState.Samples._Cache[x];

            if (sample->Descriptor.AllocatedMemory1 != NULL)
            {
                total = total + sample->AllocatedMemorySize;
            }
        }

        while (TRUE)
        {
            if ((total + size) <= *SoundState.Samples._MaximumCacheSize) { return TRUE; }

            auto indx = SoundState.Samples.CacheIndex;

            for (u32 x = 0; x < 256; x++) // TODO constant
            {
                indx = indx + 1;

                if ((256 - 1) < indx) { indx = 0; } // TODO constant

                auto sample = &SoundState.Samples._Cache[x];

                if (sample->Descriptor.AllocatedMemory1 != NULL
                    && 0 < sample->AllocatedMemorySize
                    && 0 < sample->Length
                    && sample->LockLength == 0
                    && sample->Index == 0
                    && sample->Descriptor.CacheControl != SoundCacheMode::Static)
                {
                    LogMessage("[SOUND] Releasing sound sample % from the sound sample cache.",
                        sample->Descriptor.Definition.Name);

                    total = total - sample->AllocatedMemorySize;

                    break;
                }

                if (x == (256 - 1)) { return FALSE; } // TODO constant
            }
        }

        return FALSE;
    }

    void SelectEnvironmentalAudioExtensionsEnvironment(const s32 indx, const f32 v1, const f32 v2)
    {
        LockSounds();

        SoundState.Listener.EAX.Index = indx;
        SoundState.Listener.EAX.UnknownValue1 = v1;
        SoundState.Listener.EAX.UnknownValue2 = v2;

        if (*SoundState._SoundControllerPointer != NULL)
        {
            (*SoundState._SoundControllerPointer)->Self->SelectEnvironmentalAudioExtensionsEnvironment(
                *SoundState._SoundControllerPointer, indx, v1, v2);

            UnlockSound1();

            return;
        }

        if (*SoundState.Lock._Count == 0)
        {
            LogError("Unable to unlock sound sample, it is not locked.");
        }

        if (*SoundState.Lock._Count != 1)
        {
            *SoundState.Lock._Count = *SoundState.Lock._Count + -1;

            ReleaseMutex1(SoundState.Lock.Mutex);
        }
        else
        {
            SelectSoundModeValues(0);

            UnlockSound2();
        }
    }

    void SelectSoundEffectNextChannelIndex(const s32 indx)
    {
        if (indx < 0 || 31 < indx) // TODO constant
        {
            LogError("Unable to set next sound effect channel index, invalid index %d.", indx);
        }

        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].NextChannelIndex = indx;
    }

    void SelectUnknownSoundMode1(const u32 mode) // TODO enum
    {
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk30 =
            SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk30 | mode;
    }

    void DecrementSountEffectOptionsIndex(void)
    {
        *SoundState._SoundEffectDescriptorArrayIndex = *SoundState._SoundEffectDescriptorArrayIndex - 1;

        if (*SoundState._SoundEffectDescriptorArrayIndex < 0) // TODO constant
        {
            LogError("Unable to decrement sound effect options index, index is out of bounds.");
        }
    }

    void IncrementSountEffectOptionsIndex(void)
    {
        *SoundState._SoundEffectDescriptorArrayIndex = *SoundState._SoundEffectDescriptorArrayIndex + 1;

        if (7 < *SoundState._SoundEffectDescriptorArrayIndex) // TODO Constant
        {
            LogError("Unable to increment sound effect options index, index is out of bounds.");
        }

        CopyMemory(&SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex],
            &SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex - 1],
            sizeof(SoundEffectDescriptor));
    }

    void AllocateSoundMixBuffers(const s32 count, const s32 value)
    {
        *SoundState._UnknownValue204 = 1;
        *SoundState._UnknownValue201 = value;

        for (u32 x = 0; x < *SoundState._UnknownValue204; x++)
        {
            *SoundState._UnknownValue204 = *SoundState._UnknownValue204 * 2;
        }

        *SoundState._UnknownValue202 = 0;
        *SoundState._UnknownValue203 = 0;

        *SoundState._UnknownMemory1 = ReallocateMemory(*SoundState._UnknownMemory1, *SoundState._UnknownValue204 * 4 * value * *SoundState.Options._ChannelCount); // TODO constant
        *SoundState._UnknownMemory2 = ReallocateMemory(*SoundState._UnknownMemory2, *SoundState._UnknownValue204 * 4 * *SoundState.Options._ChannelCount * 2); // TODO constant
        SoundState.UnknownMemory3 = ReallocateMemory(SoundState.UnknownMemory3, *SoundState._UnknownValue204 * 4 * *SoundState.Options._ChannelCount); // TODO constant

        if (*SoundState._UnknownMemory1 == NULL || *SoundState._UnknownMemory2 == NULL || SoundState.UnknownMemory3 == NULL)
        {
            LogError("Unable to allocate memory for sound mix buffers .");
        }

        // TODO is this correct? I feel like 4s and 8s are for pointer sizes
        for (u32 x = 0; x < *SoundState.Options._ChannelCount; x++)
        {
            SoundState._UnknownArray104[x] = (UnknownArray104Item*)
                ((addr)SoundState._UnknownMemory1 + *SoundState._UnknownValue204 * x * *SoundState._UnknownValue201 * 4); // Constants

            SoundState._UnknownArray101[x] = (void*)((addr)SoundState.UnknownMemory3 + x * *SoundState._UnknownValue204 * 4); // Constants

            SoundState._UnknownArray102[x] = (void*)((addr)SoundState._UnknownMemory2 + *SoundState._UnknownValue204 * 8 * x); // Constants
            SoundState._UnknownArray103[x] = (void*)((addr)SoundState._UnknownMemory2 + (*SoundState._UnknownValue204 * 4 + *SoundState._UnknownValue204 * 8 * x)); // Constants
        }
    }

    /*
    void FUN_00579ad0(const u32* values, const u32 bps, const u32 channels, const u32 hz, const s32 count, const s32 size)
    {
        if (*SoundState.Lock._Count < 1)
        {
            LogError("Sound must be locked."); // TODO better message
        }

        for (u32 x = 0; x < channels; x++)
        {
            if (SoundState._UnknownArray104[x] == NULL) { return; }
        }

        if (bps == *SoundState.Options._Bits && channels == *SoundState.Options._ChannelCount && hz == *SoundState.Options._HZ)
        {
            SelectSoundLocations1();

            u32 vls[9];

            for (u32 x = 0; x < channels; x++)
            {
                vls[x + 1] = values[x];
            }
        }
    }*/

    void ConvertMixBufferToOutputBuffer(const f32* floats, u32* values, const u32 bps, const s32 count, const s32 size)
    {
        switch (bps)
        {
        case 8: // TODO constant
        {
            u8* vls = (u8*)((void*)values);

            for (u32 x = 0; x < count; x++)
            {
                const auto value = values[x];

                if (value <= -1.0f) // TODO constant
                {
                    vls[size * x] = 0; // TODO constant
                }
                else if (value < 1.0f) // TODO constant
                {
                    vls[size * x] = (u8)roundf(value * 127.0f + 128.0f); // TODO constants
                }
                else
                {
                    vls[size * x] = 255; // TODO constant
                }
            }

            break;
        }
        case 16: // TODO constant
        {
            u16* vls = (u16*)((void*)values);

            for (u32 x = 0; x < count; x++)
            {
                const auto value = values[x];

                if (value <= -1.0f) // TODO constant
                {
                    vls[size * x] = 32769; // TODO constant, is the value correct?
                }
                else if (value < 1.0f) // TODO constant
                {
                    vls[size * x] = (u16)roundf(value * 32766.0f); // TODO constants
                }
                else
                {
                    vls[size * x] = 32767; // TODO constant
                }
            }

            break;
        }
        default:
        {
            LogError("Unable to convert sound mix buffer to an output buffer, invalid bits per sample %d.", bps);
        }
        }
    }

    void InitializeSound1(void)
    {
        for (u32 x = 0; x < 32; x++) // TODO constant
        {
            SelectSoundEffectChannelVolume2(x, 1.0f);
            SelectSoundEffectChannelVolume1(x, 1.0f);

            SelectSoundEffectChannelState(x, TRUE);
        }

        SelectListenerDistanceFactor(1.0f);
        SelectListenerPosition(0.0f, 0.0f, 0.0f);
        SelectListenerOrientation(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        SelectListenerVelocity(0.0f, 0.0f, 0.0f);

        SelectSoundDevice2(AcquireSoundDeviceIndex());

        SelectSoundRecordingDevice2(AcquireSoundRecordingDeviceIndex());
    }

    void SelectUnknownSoundMode2(const u32 mode) // TODO enum
    {
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk30 =
            SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].Unk30 & ~mode;
    }
    
    void SelectRemainingDelay(const f32 delay)
    {
        SoundState._SoundEffectDescriptorArray[*SoundState._SoundEffectDescriptorArrayIndex].RemainingDelay = delay;
    }

    void UnknownSound102(void)
    {
        if (*SoundState._SoundControllerPointer == NULL) { return; }

        if (!AcquireSoundActiveMode()) { return; }

        if (WaitSoundMutex(SoundState.Lock.Mutex, SoundState.Thread.TimeValue * 3.0f)) // TODO constant
        {
            SoundState.UnknownTime = AcquireTime();
            *SoundState.Lock._Count = *SoundState.Lock._Count + 1;

            (*SoundState._SoundControllerPointer)->Self->AcquireSecondaryBufferPosition(*SoundState._SoundControllerPointer);

            UnknownSound101();
            SelectSoundModeValues(1);
            UnlockSound1();
        }
    }
}