#pragma once

#include "Assets.Sounds.hxx"
#include "IO.hxx"
#include "IO.Streams.hxx"
#include "Mathematics.Basic.hxx"
#include "Memory.hxx"
#include "Objects.hxx"
#include "Sounds.Basic.hxx"
#include "Sounds.Samples.hxx"
#include "Sounds.Effects.hxx"
#include "Sounds.Controller.hxx"

#define MAX_SOUND_LOCK_COUNT 100
#define MAX_SOUND_LOCK_SAMPLE_COUNT 256

#define MAX_SOUND_THREAD_ACTION_ITERATION_COUNT 100

#define SOUND_SFX_FILE_NAME_TEMPLATE "%s.SFX"

#define MAX_SOUND_SAMPLE_CACHE_COUNT 256

namespace Sounds
{
    struct UnknownArray104Item
    {
        s32 Unk; // TODOUnknown size and type
    };

    struct SoundContainer
    {
        BOOL ActiveMode;// = (BOOL*)0x008d84f8;//todo
        BOOL MuteMode;// = (BOOL*)0x008d8500;//todo

        struct
        {
            // The buffer is a global sound buffer. With this flag set,
            // an application using DirectSound can continue to play its buffers if the user switches focus to another application,
            // even if the new application uses DirectSound.
            BOOL GlobalFocusMode = TRUE;// = (BOOL*)0x008c5d3c;//todo, always 0?
        } Debug;

        struct
        {
            s32 Index;// = (s32*)0x008f8e88;//todo
            IKsPropertySet* Instance;// = (IKsPropertySet**)0x008f9bec;//todo
        } EAX;

        struct
        {
            s32* _Count = (s32*)0x008d860c;//todo
            HANDLE Mutex;// = (HANDLE*)0x008d861c;//todo
        } Lock;

        struct
        {
            IDirectSound* Instance;// = (IDirectSound**)0x008f9bdc;//todo
            IDirectSound3DListener* Listener;// = (IDirectSound3DListener**)0x008f9be8;//todo, move to "Buffers"?

            struct
            {
                struct
                {
                    u32 BitsPerSample;// = (u32*)0x008f9bf0;//todo
                    u32 HZ;// = (u32*)0x008f9bf4;//todo
                    u32 Channels;// = (u32*)0x008f9bf8;//todo

                    IDirectSoundBuffer* Buffer;// = (IDirectSoundBuffer**)0x008f9be0;//todo
                } Primary;

                struct
                {
                    // TODO: these two do not go for only secondary buffer
                    u32 Unknown1;// = (u32*)0x008f9c04;// todo, some sort of count
                    u32 Unknown2;// = (u32*)0x008f9c08;//TODO some sort of count

                    u32 Count;// = (u32*)0x008f9bfc;//todo, type!
                    s32 ChannelBufferSize;// = (s32*)0x008f9c00;//todo, type!, name: BufferSize

                    IDirectSoundBuffer* Buffer;// = (IDirectSoundBuffer**)0x008f9be4;//todo
                } Secondary;

            } Buffers;

            SoundMixMode MixMode;// = (SoundMixMode*)0x008d84fc;//todo
        } DirectSound;

        struct
        {
            u32 ChannelCount = 32;// (u32*)0x006254e0;// TODO default value

            f32 ChannelVolumes1[32];// = (f32*)0x008d7ab4;// TODO MAX_SOUND_SFX_CHANNEL_COUNT

            BOOL ChannelStatuses[32];// = (BOOL*)0x008d7bb4;//todo BOOL [MAX_SOUND_SFX_CHANNEL_COUNT]

            f32 ChannelVolumes2[32];// = (f32*)0x008d7b34;//TODO 32 elements
        } SFX; // TODO: combine with Effects

        struct
        {
            u32* _Bits = (u32*)0x006254e4;//todo, default value
            u32* _ChannelCount = (u32*)0x006254e8;//todo, default value
            u32* _HZ = (u32*)0x006254ec;//todo, default value

            f32 MaximumSoftWareLatency = 0.5f;// (f32*)0x00625510;//todo, default value 0.5f;
        } Options;

        SoundMixMode MixMode = SoundMixMode::Simple;// 0x006254f8;

        SoundController** _SoundControllerPointer = (SoundController**)0x008d84f4;//TODO
        SoundController* _SoundController = (SoundController*)0x008fb880;//TODO

        struct
        {
            f32 TimeValue;// = (f32*)0x008d8610;//TODO
            BOOL IsActive;// = (BOOL*)0x008d8614;//todo
            BOOL IsQuit;// = (BOOL*)0x008d8618;//todo
        } Thread;

        struct
        {
            SoundSample* UnknownSample;// = (SoundSample**)0x008d8628;//TODO
            s32 AssetOperationIndex;// = (s32*)0x008d8624;//TODO

            s32* _UnknownSampleValue1 = (s32*)0x00625514;//TODO
            s32* _UnknownSampleValue2 = (s32*)0x00625518;//TODO

            SoundSample* _Cache = (SoundSample*)0x008ca234;// TODO array of256 elements

            s32 CacheIndex;// = (s32*)0x008ca230;//TODO

            // TODO, allocate this at the start of the game?
            u32 UnknownBuffer101[32768];//TODO proper type = (u32*)0x008d8630;// TODO 32768 elements

            u32 MinimumCacheSize = 0x200000;// (u32*)0x00625508;//tODO default value
            s32* _MaximumCacheSize = (s32*)0x0062550c;//tODO default value
        } Samples;

        struct
        {
            SoundEffect* _Cache = (SoundEffect*)0x008c5d4c;// TODO array of 64
        } Effects;

        s32 UnknownTime;// = (s32*)0x008c5cf8;//TODO
        f32 UnknownValue101;// = (f32*)0x008c5d40;//TODO

        s32 UnknownValue102;// = (s32*)0x008f8638;//TODO

        BOOL* _SoundRecordingActive = (BOOL*)0x008f8e80;//TODO
        SoundRecordingController** _SoundRecordingControllerPointer = (SoundRecordingController**)0x008f8e7c;//TODO

        void** _UnknownMemory1 = (void**)0x008d8538; //TODO
        void** _UnknownMemory2 = (void**)0x008d855c; //TODO
        void* UnknownMemory3;// = (void**)0x008d8514; //TODO

        void** _UnknownArray101 = (void**)0x008d8518; // TODO 8 elements
        void** _UnknownArray102 = (void**)0x008d8560; // TODO 8 elements
        void** _UnknownArray103 = (void**)0x008d8580; // TODO 8 elements

        UnknownArray104Item** _UnknownArray104 = (UnknownArray104Item**)0x008d853c; // TODO array of 8 pointers

        s32* _UnknownValue201 = (s32*)0x008d8510;//TODO
        s32* _UnknownValue202 = (s32*)0x008d8504;//TODO
        s32* _UnknownValue203 = (s32*)0x008d8508;//TODO
        s32* _UnknownValue204 = (s32*)0x008d850c;//TODO

        SoundBuffer SoundBufferPool[MAX_SOUND_BUFFER_POOL_SIZE];// = (SoundBuffer*)0x008f9c0c;//TODO [MAX_SOUND_BUFFER_POOL_SIZE]

        SoundUnk0x24 SoundUnk0x24Array[31];// = (SoundUnk0x24*)0x008fb424;// TODO 31 size

        s32 SoundTime1;// = (s32*)0x008d8620;//TODO
        s32 SoundTimeUnknown1 = 1;// (s32*)0x006254dc;//TODO, looks like SoundMixMode
        s32* _UnknownSoundTime2 = (s32*)0x008c5cf4;//TODO

        f32* _UnknownSoundVolume101 = (f32*)0x008fd4a8;//TODO

        s32* _SoundEffectDescriptorArrayIndex = (s32*)0x008c9f4c; //TODO
        SoundEffectDescriptor* _SoundEffectDescriptorArray = (SoundEffectDescriptor*)0x008c9f50;//TODO

        f32 UnknownValue210 = 4.0f;// = (f32*)0x006254d8;//TODO
        s32 UnknownValue211 = 1;// = (s32*)0x006254d0;//TODO

        u32 UnknownValue301;// = (u32*)0x008d7ab0;//TODO

        struct
        {
            struct
            {
                u32 Count;// = (u32*)0x008f8630;//TODO
                Assets::Sounds::SoundSampleRecordDescriptor* Items;// = (Assets::Sounds::SoundSampleRecordDescriptor**)0x008f8634;//TODO
            } Cache;

            struct
            {
                u32 Count;// = (u32*)0x008fca44; // TODO, is this even needed?
                char Items[40][64];// = (char*)0x008fca48;//TODO, is this even needed? size [40][64]
            } Names;
        } Records; // TODO rename

        SoundSample** _UnknownSoundSample1 = (SoundSample**)0x008d862c;

        struct
        {
            u32 Index;// = (u32*)0x008d7aac;//TODO

            struct
            {
                f32 X[2];// = (f32*)0x008d7a34;//TODO 2 items
                f32 Y[2];// = (f32*)0x008d7a3c;//TODO 2 items
                f32 Z[2];// = (f32*)0x008d7a44;//TODO 2 items
            } Positions;

            struct
            {
                f32 X1[2];// = (f32*)0x008d7a64;//TODO 2 items
                f32 Y1[2];// = (f32*)0x008d7a6c;//TODO 2 items
                f32 Z1[2];// = (f32*)0x008d7a74;//TODO 2 items

                f32 XT[2];// = (f32*)0x008d7a7c;//TODO 2 items
                f32 YT[2];// = (f32*)0x008d7a84;//TODO 2 items
                f32 ZT[2];// = (f32*)0x008d7a8c;//TODO 2 items

                f32 XF[2];// = (f32*)0x008d7a94;//TODO 2 items
                f32 YF[2];// = (f32*)0x008d7a9c;//TODO 2 items
                f32 ZF[2];// = (f32*)0x008d7aa4;//TODO 2 items
            } Orientation;

            struct
            {
                f32 X[2];// = (f32*)0x008d7a4c;//TODO 2 items
                f32 Y[2];// = (f32*)0x008d7a54;//TODO 2 items
                f32 Z[2];// = (f32*)0x008d7a5c;//TODO 2 items
            } Velocity;
            
            f32 InverseDistanceFactor = 1.0f;// (f32*)0x00625500;//TODO
            f32 DistanceFactor = 1.0f;// = (f32*)0x006254fc;//TODO

            struct
            {
                s32 Index;// = (s32*)0x008d8604;//TODO
                f32 UnknownValue1 = 1.0f;// = (f32*)0x00625504;//TODO default value
                f32 UnknownValue2;// = (f32*)0x008d8608;// TODO
            } EAX;
        } Listener;

        struct
        {
            f32 X[8];// = (f32*)0x008d85a4;//TODO 8 items
            f32 Y[8];// = (f32*)0x008d85c4;//TODO 8 items
            f32 Z[8];// = (f32*)0x008d85e4;//TODO 8 items
        } Locations;

        // Note : the first property of an UnknownSoundObject,
        // Which, since it is static its values in a static way are represented by this sturcture,
        // with a mix of device and DirectSound stuff.
        u32* _CacheSize = (u32*)0x008fca14;//TODO
    };

    extern SoundContainer SoundState;

    void AcquireSoundStatistics(IO::Streams::AbstractOutStream* stream);

    void LockSounds(void);
    void UnlockSound1(void);
    void UnlockSound2(void);
    void UnlockSound3(void);

    BOOL AcquireSoundAvailableMode(void);       // TODO name
    BOOL AcquireSoundActiveMode(void);        // TODO name
    BOOL AcquireSoundMuteMode(void);
    BOOL SelectSoundMuteMode(const BOOL mode);

    BOOL StopSoundThread(void);

    SoundMixMode AcquireSoundMixMode2(void);
    void SelectSoundMixMode(const SoundMixMode mode);

    BOOL IsSoundRecordingPresent(void);

    f32 AcquireMaximumSoftWareSoundLatency(void);
    void SelectMaximumSoftWareSoundLatency(const f32 value);

    void AcquireSoundOutputOptions(u32* bits, u32* channels, u32* hz);
    BOOL SelectSoundOutputOptions(const u32 bits, const u32 channels, const u32 hz);
    u32 AcquireSoundChannelCount(void);

    SoundMixMode AcquireSoundMixMode1(void);
    BOOL RestartSoundThread(const f32 value);
    DWORD WINAPI StartSoundThread(LPVOID);
    BOOL WaitSoundMutex(const HANDLE mutex, const f64 time);

    void ResetSoundSampleCache1(void);
    void ResetSoundEffectCache1(void);

    BOOL StopSound1(void);
    BOOL StopSound2(void);
    void ReleaseUnknownSoundMemory(void);

    BOOL StopSoundRecording1(void);
    BOOL StopSoundRecording2(void);

    BOOL SelectSoundEffectPosition1(SoundEffect* self);
    void UpdateSoundEffectPosition2(SoundEffect* self, const f32 position);

    f32 AcquireUnknownSoundValue101(const f32 value);

    s32 AcquireSoundSampleDescriptorValue201(const s32 offset, const s32 v2); // TODO
    s32 AcquireSoundSampleDescriptorValue202(const s32 offset, const s32 v2); // TODO
    s32 AcquireSoundSampleDescriptorValue203(SoundSampleDescriptor* self, const s32 offset, const s32 v2); // TODO
    s32 AcquireSoundSampleDescriptorValue204(SoundSampleDescriptor* self, const s32 offset, const s32 v2); // TODO

    BOOL AllocateSoundsSample(SoundSample* self);

    void SelectSoundEffectChannelVolume1(const s32 indx, const f32 value);
    void SelectSoundEffectChannelVolume2(const s32 indx, const f32 value);

    void SelectSoundEffectChannelCount(const u32 count);

    BOOL AcquireSoundEffectChannelState(const s32 indx);
    void SelectSoundEffectChannelState(const s32 indx, const BOOL mode);

    void InitializeSoundEffectChannels(void);

    void UnknownPlay101(void);

    void SelectListenerDistanceFactor(const f32 value);

    u32 InitializeSoundEffect1(const char* file);

    s32 AcquireSoundFrequency(const f32 value);
    s32 AcquireUnknownSoundValue102(const f32 value);

    void InitializeSoundRecordDescriptor(Assets::Sounds::SoundSampleRecordDescriptor* self, const SoundSampleDescriptor* desc);
    void InitializeSoundSampleDescriptor3(const Assets::Sounds::SoundSampleRecordDescriptor* self, SoundSampleDescriptor* desc);
    BOOL AcquireSoundSampleDescriptor1(SoundSampleDescriptor* self);
    BOOL AcquireSoundSampleDescriptor2(SoundSampleDescriptor* self);
    BOOL InitializeUnknownSound1(const char* name, const s32 type);

    void* LockSoundSample(SoundSample* self, const s32 offset, const u32 length);
    BOOL LoadSoundSample2(SoundSample* self, IO::Streams::InStreamFile* stream);

    void AcquireSoundSample2(const char* file);
    BOOL StartSound(void);
    void SelectListenerVelocity(const f32 x, const f32 y, const f32 z);
    void SelectListenerOrientation(const f32 x1, const f32 y1, const f32 z1, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf);
    void SelectListenerPosition(const f32 x, const f32 y, const f32 z);
    u32 AcquireSoundListenerIndex(const f32 x, const f32 y, const f32 z);
    BOOL UnknownListenerPositionCheck(const f32 x, const f32 y, const f32 z, const f32 value);
    BOOL InitializeSoundEffectVolume(SoundEffect* effect, const f32 volume);
    void SelectSoundEffectRemainingDelay(SoundEffect* self);
    void SelectSoundEffectDistances1(SoundEffect* self);
    void SelectSoundLocations1(void);
    void SelectSoundEffectLocationVelocity(SoundEffect* self);
    void SelectSoundEffectVolume(SoundEffect* self);
    void SelectSoundModeValues(const BOOL mode);
    f32 SelectSoundSamplePlaybackPosition1(SoundSampleDescriptor* self, const f32 value, const u32 type1, const u32 type2); // TODO enums
    SoundEffect* AcquireSoundEffect(const u32 hash, const u32 mode);
    f32 SelectSoundSamplePlaybackPosition2(const u32 hash, const u32 type);
    void AssignUnknownSoundSample1(SoundSample* self);
    f32 SelectSoundSamplePlaybackPosition3(SoundSampleDescriptor* self, const f32 value, const u32 type);
    void SeekSoundSample1(SoundSample* self, const s32 value, const s32 destination);
    void SeekSoundEffect1(SoundEffect* self);
    void UnknownSound101(void);
    void SelectSoundSampleBuffer1(SoundSample* self);
    void UnlockSoundSampleBuffer1(SoundSample* self);
    void InitializeSoundSample5(SoundSample* self, const s32 value);
    void SeekSoundSample2(SoundSample* self, const s32 value);
    BOOL SelectSoundSampleCacheSize(const s32 size);
    BOOL InitializeSecondarySoundBuffer1(void);
    void SelectEnvironmentalAudioExtensionsEnvironment(const s32 indx, const f32 v1, const f32 v2);
    void SelectSoundEffectNextChannelIndex(const s32 indx);
    void SelectUnknownSoundMode1(const u32 mode); // TODO enum
    void DecrementSountEffectOptionsIndex(void);
    void IncrementSountEffectOptionsIndex(void);
    void SelectSoundEffectDistances2(SoundEffect* self);
    void AllocateSoundMixBuffers(const s32 count, const s32 value);
    void ConvertMixBufferToOutputBuffer(const f32* floats, u32* values, const u32 bps, const s32 count, const s32 size);
    void InitializeSound1(void);
    void SelectSoundEffectLocation(const f32 x, const f32 y, const f32 z);
    void SelectUnknownSoundMode2(const u32 mode);
    u32 InitializeSoundEffect2(const char* file, const f32 x, const f32 y, const f32 z);
    u32 InitializeSoundEffect3(const char* file);
    void SelectRemainingDelay(const f32 delay);
    BOOL ReleaseSoundEffect2(const u32 hash);
    BOOL AcquireSoundEffect1(const u32 hash);
    void SelectSoundSampleCacheSize1(const u32 min, const u32 max);
    void SelectSoundEffectUnknown9(const f32 value);
    void SelectSoundEffectVolume1(const f32 value);
    void SelectSoundEffectUnknown30(const s32 value);
    void SelectNextSoundEffectTag(const s32 indx, char* value);
    void SelectSoundEffectUnknown3And4(f32* value);
    void SelectSoundEffectVelocity(const f32 x, const f32 y, const f32 z);
    void SelectSoundEffectUnknown18And19(const f32 unk18, const s32 unk19);
    u32 AcquireActiveSoundEffectCount(void);
    u32 AcquireSoundEffectUnknownValue1(void);
    u32 AcquireSoundEffectUnknownValue2(const u32 value);
    BOOL SelectSoundEffectValues1(const u32 hash, const f32 unk203, const f32 volume, const s32 unk202);// TODO names
    BOOL AcquireSoundEffectDescriptor(const u32 hash, SoundEffectDescriptor* desc);
    BOOL AcquireSoundSampleDescriptor3(const u32 hash, SoundSampleDescriptor* desc);
    void UnknownSound102(void);
    f32 AcquireSoundEffectChannelVolume(const s32 indx);

    typedef SoundDecoder* (CDECLAPI* FUN_00514BE0) (void* self);// TODO
    static FUN_00514BE0 FUN_00514be0 = (FUN_00514BE0)0x00514be0;// TODO

    typedef SoundDecoder* (CDECLAPI* FUN_00514C30) (SoundDecoder* self, const char* name, const s32 param_3);// TODO
    static FUN_00514C30 FUN_00514c30 = (FUN_00514C30)0x00514c30;// TODO

    typedef const BOOL(CDECLAPI* FUN_00515050) (SoundDecoder* self, const s32 param_2);// TODO
    static FUN_00515050 FUN_00515050a = (FUN_00515050)0x00515050;// TODO


    typedef const s32(CDECLAPI* FUN_00571E10) (SoundSample* self);// TODO
    static FUN_00571E10 FUN_00571e10 = (FUN_00571E10)0x00571e10;// TODO

    typedef const void (CDECLAPI* FUN_00579AD0) (u32* values, u32 bps, u32 channels, u32 hz, s32 count, s32 size);// TODO
    static FUN_00579AD0 FUN_00579ad0 = (FUN_00579AD0)0x00579ad0;// TODO

    //////////////////////////////////////////
    // THIS STUFF BREAKS SOUND SO FAR
    //////////////////////////////////////////


    //f32 AcquireSoundEffectChannelVolume(const s32 indx);
    //typedef const f32(CDECLAPI* ACQUIRESOUNDEFFECTCHANNELVOLUME) (s32 indx);//TODO
    //static ACQUIRESOUNDEFFECTCHANNELVOLUME AcquireSoundEffectChannelVolume = (ACQUIRESOUNDEFFECTCHANNELVOLUME)0x005760b0;//TODO 
}