#include "Unknown.hxx"
#include "Sounds.hxx"
#include "ASCII.hxx"
#include "Logger.hxx"
#include "Strings.hxx"
#include "Sounds.Unknown.hxx"
#include "Sounds.Devices.hxx"

using namespace Assets::Sounds;
using namespace Strings;
using namespace Sounds;
using namespace ASCII;
using namespace Logger;
using namespace Mathematics;

namespace Sounds
{
    UnknownSoundContainer UnknownSoundState;

    void StopSound3(UnknownSoundObject* self)
    {
        if (0 < *UnknownSoundState._UnknownValue401) { return; }

        *UnknownSoundState._UnknownValue401 = *UnknownSoundState._UnknownValue401 + 1;

        StopSound1();
        StopSound2();
        StopSoundRecording2();
        StopSoundRecording1();
        ResetSoundSampleCache1();

        ReleaseSoundRecordDescriptorCollection();

        *UnknownSoundState._UnknownValue401 = *UnknownSoundState._UnknownValue401 + -1;
    }

    f32 AcquireSoundUnknownValue1(UnknownSoundObject* self, const char* name)
    {
        SoundSampleDescriptor desc;

        SoundSampleDescriptorInitialize1(&desc);

        u32 length = 0;

        while (name[length] != NULL && name[length] != ASCII_AT
            && name[length] != ASCII_ASTERISK && IsNotSpace(name[length]))
        {
            length = length + 1;
        }

        CopyMemory(desc.Definition.Name, name, length);

        desc.Definition.Name[length + 1] = NULL;

        if (AcquireSoundSampleDescriptor2(&desc) == 0) { return -1.0f; } // TODO constant

        if (0 < desc.Definition.HZ)
        {
            return (f32)desc.Definition.Length / (f32)desc.Definition.HZ;
        }

        return 0.0f; // TODO constant
    }

    void AcquireSoundSample3(UnknownSoundObject* self, char* name, const char* value)
    {
        name[0] = NULL;

        AcquireSoundSample2(value);

        if (SoundState.Records.Names.Count < 1)
        {
            LogMessage("[WARNING] [SOUND] Unable to find sound sample matching %s.\n", value);

            return;
        }

        // Pick the one and only name if one is available.
        if (SoundState.Records.Names.Count == 1)
        {
            strcpy(name, SoundState.Records.Names.Items[0]);

            return;
        }

        // Pick a random name from the available, in case more than one is available.
        strcpy(name, SoundState.Records.Names.Items[rand() % SoundState.Records.Names.Count]);
    }

    void AcquireSoundSample4(UnknownSoundObject* self, const char* name)
    {
        if (IsNullOrEmpty(name)) { return; }

        AcquireSoundSample2(name);

        for (u32 x = 0; x < SoundState.Records.Names.Count; x++)
        {
            InitializeUnknownSound1(SoundState.Records.Names.Items[x], 2); // TODO
        }
    }

    void SelectSoundEffectChannelStates1(UnknownSoundObject* self)
    {
        SelectSoundEffectChannelStates2(self);
    }

    void SelectSoundEffectChannelStates2(UnknownSoundObject* self)
    {
        *UnknownSoundState._UnknownValue1 = 0;

        SelectSoundEffectChannelState(0, 0);
        SelectSoundEffectChannelState(0, 1);
        SelectSoundEffectChannelState(1, 0);
        SelectSoundEffectChannelState(1, 1);
        SelectSoundEffectChannelState(2, 0);
        SelectSoundEffectChannelState(2, 1);
    }

    void InitializeSound2(UnknownSoundObject* self, const char* file)
    {
        ResetSoundSampleCache1();

        if (0.3f < AcquireSoundEffectChannelVolume(1)) // TODO constant
        {
            SelectSoundEffectChannelVolume1(1, 0.3f); // TODO constant
        }

        if (0.5f < AcquireSoundEffectChannelVolume(0)) // TODO constant
        {
            SelectSoundEffectChannelVolume1(0, 0.5f); // TODO constant
        }

        if (!AcquireSoundMuteMode())
        {
            SelectSoundEffectChannelStates1(self);

            SelectEnvironmentalAudioExtensionsEnvironment(0, 0.0f, 0.0f);
            SelectListenerPosition(0.0f, 0.0f, 0.0f);
            SelectListenerOrientation(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
            SelectListenerVelocity(0.0f, 0.0f, 0.0f);

            if (!AcquireSoundActiveMode())
            {
                StartSound();
            }

            IncrementSountEffectOptionsIndex();
            SelectSoundEffectNextChannelIndex(1);
            SelectUnknownSoundMode1(1);

            InitializeSoundEffect1(file == NULL ? "music_splash-44.wav" : file); // TODO constant

            DecrementSountEffectOptionsIndex();
        }
    }

    void InitializeSounds1(UnknownSoundObject* self)
    {
        *SoundState._CacheSize = 0x2000; // TODO constants

        SelectSoundEffectChannelCount(3);

        SelectSoundSampleCacheSize1(0x2000 << 10, 0x2000 << 10); // TODO constants

        InitializeSound1();

        InitializeSoundRecordDescriptorCollection("samplelist.txt"); // TODO constants
        InitializeSoundRecordDescriptorCollection("voxsamplelist.txt"); // TODO constants

        SelectSoundDeviceMixMode();
        SelectSoundDeviceOutputOptions();

        SelectSoundEffectChannelVolume1(0, 0.5f * 0.67f); // TODO constants
        SelectSoundEffectChannelVolume1(1, 0.3f * 0.67f); // TODO constants

        InitializeSoundEffectChannels();
    }

    void InitializeSound3(UnknownSoundObject* self, char* tag, const char* file)
    {
        InitializeSoundEffect4(tag, file, 0.0f, 0.0f, 0.0f, NULL, 1);
    }

    void InitializeSound4(UnknownSoundObject* self, char* tag, const char* file, const f32x3* values) // TODO proper type
    {
        InitializeSoundEffect4(tag, file, values->X, values->Y, values->Z, NULL, 0);
    }

    void InitializeSound5(UnknownSoundObject* self, char* tag, const char* file, f32* values)
    {
        InitializeSoundEffect4(tag, file, 0.0, 0.0, 0.0, values, 0);
    }

    u32 InitializeSound6(UnknownSoundObject* self, char* tag, const char* file, const f32x3* values, const f32 delay)
    {
        IncrementSountEffectOptionsIndex();

        SelectRemainingDelay(delay);

        const auto result = InitializeSoundEffect4(tag, file, values->X, values->Y, values->Z, NULL, 0);

        DecrementSountEffectOptionsIndex();

        return result;
    }

    u32 InitializeSound7(UnknownSoundObject* self, char* tag, const char* file, f32* values, const f32 delay)
    {
        IncrementSountEffectOptionsIndex();

        SelectRemainingDelay(delay);

        const auto result = InitializeSoundEffect4(tag, file, 0.0, 0.0, 0.0, values, 0);

        DecrementSountEffectOptionsIndex();

        return result;
    }

    u32 InitializeSoundEffect4(char* tag, const char* file, const f32 x, const f32 y, const f32 z, f32* values, const s32 unk30)
    {
        if (AcquireSoundMuteMode()) { return 0; }

        if (IsNullOrEmpty(file)) { return 0; }

        auto options = -1;
        char name[100];// TODO constants

        {
            auto indx = 0;

            auto pi = 0;
            char props[100];// TODO constants

            while (file[indx] != NULL)
            {
                const auto c = file[indx];

                if (IsSpace(c) || c == ASCII_AT || c == ASCII_ASTERISK)
                {
                    options = indx;

                    break;
                }

                props[pi] = c;

                pi = pi + 1;

                if (c != ASCII_SQUARE_BRACKET_OPEN)
                {
                    indx = indx + 1;

                    continue;
                }

                auto v1 = 0;
                auto v2 = 0;
                auto count = -1;

                sscanf(&file[indx], "[%d,%d]%n", &v1, &v2, &count);

                if (count < 5 || v2 < v1)// TODO constants
                {
                    LogError("Invalid sound effect options %s.", file);
                }

                props[pi] = ASCII_QUESTION_MARK;

                pi = pi + v2;

                if (9 < v2)// TODO constants
                {
                    props[pi] = ASCII_QUESTION_MARK;

                    pi = pi + v2;
                }

                indx = indx + count;
            }

            props[pi] = NULL;

            pi = 0;

            while (props[pi] != NULL)
            {
                if (props[pi] == ASCII_DOT)
                {
                    props[pi] = NULL;

                    break;
                }

                pi = pi + 1;
            }

            strcat(props, ".wav");// TODO constants

            // TODO:
            // name,
            // pattern

            //handling ! characters

            AcquireSoundSample3(*UnknownSoundState._UnknownSoundObjectPointer, name, props);
            // TODO incomplete ^^^
        }

        if (IsNullOrEmpty(name)) { return 0; }

        auto unk9 = 1.0f;//TODO better name
        auto volume = 1.0f;

        if (options >= 0)
        {
            while (file[options] != NULL)
            {
                const auto c = file[options];

                if (IsSpace(c)) { options = options + 1; continue; }

                switch (c)
                {
                case ASCII_AT:
                {
                    auto count = -1;

                    sscanf(&file[options], "@%f%n", &volume, &count);

                    if (count < 2 || volume < 0.0f)
                    {
                        LogError("Invalid sound effect options %s.", file);
                    }

                    options = options + count;

                    break;
                }
                case ASCII_ASTERISK:
                {
                    auto count = -1;

                    sscanf(&file[options], "*%f%n", &unk9, &count);

                    if (count < 2 || unk9 < 0.0f)
                    {
                        LogError("Invalid sound effect options %s.", file);
                    }

                    options = options + count;

                    break;
                }
                default:
                {
                    LogError("Invalid sound effect options %s.", file);
                }
                }
            }
        }

        IncrementSountEffectOptionsIndex();

        if (values == NULL)
        {
            SelectSoundEffectLocation(x, y, z);
        }
        else
        {
            SelectSoundEffectUnknown3And4(values);
        }

        SelectSoundEffectUnknown30(unk30);
        SelectNextSoundEffectTag(0, tag);
        SelectSoundEffectVolume1(volume);
        SelectSoundEffectUnknown9(unk9);

        const auto result = InitializeSoundEffect1(name);

        DecrementSountEffectOptionsIndex();

        return result;
    }

    void ReleaseSoundEffect4(UnknownSoundObject* self, const char* tag)
    {
        LockSounds();

        for (auto hash = AcquireSoundEffectUnknownValue1(); hash != 0; hash = AcquireSoundEffectUnknownValue2(hash))
        {
            SoundEffectDescriptor desc;

            SoundEffectDescriptorInitialize1(&desc);

            if (AcquireSoundEffectDescriptor(hash, &desc) && tag == desc.Tags[0])
            {
                ReleaseSoundEffect3(self, hash);
            }
        }

        UnlockSound1();
    }

    BOOL ReleaseSoundEffect3(UnknownSoundObject* self, const u32 hash)
    {
        return ReleaseSoundEffect2(hash);
    }
}