#pragma once

#include "Basic.hxx"
#include "IO.hxx"

#define SOUND_RECORD_COLLECTION_VERSION 1

#define MAX_SOUND_SAMPLE_NAME_LENGTH 64

#define INVALID_SOUND_SAMPLE_LENGTH (-1)

#define SOUND_BITS_PER_SAMPLE_4 4
#define SOUND_BITS_PER_SAMPLE_8 8
#define SOUND_BITS_PER_SAMPLE_16 16

#define SOUND_FILE_WAVE_EXTENSION "WAV"
#define SOUND_FILE_MP3_EXTENSION "MP3"

#define SOUND_DIRECTORY_NAME "SOUND"

#define INVALID_SOUND_ASSET_OPERATION_INDEX 0

#define MIN_SOUND_ASSET_OPERATION_INDEX 1
#define MAX_SOUND_ASSET_OPERATION_COUNT 32

namespace Assets::Sounds
{
    enum class SoundSampleType : u32
    {
        Unknown = 0, // WAVE_FORMAT_UNKNOWN
        WAVMSPCM = 1, // WAVE_FORMAT_PCM
        MP3 = 2,
        WAVYADPCM = 3, // WAVE_FORMAT_YAMAHA_ADPCM
        Unknown4 = 4,
        WAVVWBA = 5, // WAVE_FORMAT_VOXWARE_BYTE_ALIGNED
        Unknown6 = 6
    };

    enum class SoundCacheMode : u32
    {
        Normal = 0,
        Static = 1,
        Stream = 2
    };

    struct SoundSampleDefinition
    {
        char Name[MAX_SOUND_SAMPLE_NAME_LENGTH];

        SoundSampleType Type;

        u32 BitsPerSample;
        u32 Channels;
        u32 HZ;
        u32 Length;
    };

    struct SoundSampleRecordDescriptor
    {
        char Name[MAX_SOUND_SAMPLE_NAME_LENGTH];

        u32 Length;
        u32 Offset;

        f32 ReferenceDistance;

        f32 MinimumDistance;
        f32 MaximumDistance;

        s32 Priority;

        u32 HZ;

        SoundSampleType Type;

        u32 BitsPerSample;
        u32 Channels;

        SoundCacheMode CacheControl;

        u32 Unknown1; // TODO 
        u32 Unknown2; // TODO 
    };

    enum class SoundAssetOperationState : s32
    {
        Available = -1,
        None = 0,
        Pending = 1,
        Processing = 2,
        Completed = 3
    };

    struct SoundAssetOperation
    {
        SoundAssetOperationState State;

        char Name[STANDARD_IO_FILE_NAME_LENGTH];

        s32 Offset;
        s32 Length;

        void* Data;
    };

    enum class SoundAssetThreadState : u32
    {
        None = 0,
        Initialization = 1,
        Active = 2
    };

    struct SoundAssetContainer
    {
        SoundAssetThreadState State;// = (u32*)0x00900f08;//TODO

        HANDLE Thread; // = (HANDLE*)0x00900f04;// TODO
        HANDLE Mutex;// = (HANDLE*)0x00900f00;//TODO

        SoundAssetOperation Operations[MAX_SOUND_ASSET_OPERATION_COUNT];// = (UnknownSoundStruct0x110*)0x008fecfc;

    };

    extern SoundAssetContainer SoundAssetState;

    void InitializeSoundRecordDescriptorCollection(const char* file);
    void ReleaseSoundRecordDescriptorCollection(void);

    void InitializeSoundAssets(void);
    DWORD WINAPI HandleSoundAssets(LPVOID);

    void AwaitSoundAssetAvailability(const s32 indx);

    SoundAssetOperationState AcquireSoundAssetOperationState(const s32 indx);
    s32 InitializeSoundAssetOperation(IO::FileDescriptor* desc, const s32 offset, const s32 length, void* data);
}