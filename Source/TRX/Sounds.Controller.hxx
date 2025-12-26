#pragma once

#include "Basic.hxx"
#include "Native.Basic.hxx"
#include "Sounds.Basic.hxx"
#include "Sounds.Samples.hxx"
#include "Sounds.Effects.hxx"

namespace Sounds
{
    typedef const BOOL(CDECLAPI* SOUNDCONTROLLERINITIALIZE) (void* self);
    typedef const BOOL(CDECLAPI* SOUNDCONTROLLERSTARTSECONDARYBUFFER) (void* self);
    typedef const BOOL(CDECLAPI* SOUNDCONTROLLERSTOPBUFFERS) (void* self);
    typedef const BOOL(CDECLAPI* SOUNDCONTROLLERCREATEBUFFERS)(void* self, const u32 bits, const u16 channels, const u32 hz, u32* count);
    typedef const BOOL(CDECLAPI* SOUNDCONTROLLERACQUIRESECONDARYBUFFERPOSITION) (void* self);
    typedef const SoundMixMode(CDECLAPI* SOUNDCONTROLLERQUERYDEVICECAPABILITIES) (void* self);
    typedef const void(CDECLAPI* SOUNDCONTROLLERCOMMITSOUNDEFFECTSETTINGS) (void* self);
    typedef const void(CDECLAPI* SOUNDCONTROLLERSELECTLISTENERPOSITION) (void* self, const f32 x, const f32 y, const f32 z);
    typedef const void(CDECLAPI* SOUNDCONTROLLERSELECTLISTENERORIENTATION) (void* self, const f32 x1, const f32 y1, const f32 z1, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf);
    typedef const void(CDECLAPI* SOUNDCONTROLLERSELECTLISTENERVELOCITY) (void* self, const f32 x, const f32 y, const f32 z);
    typedef const void(CDECLAPI* SOUNDCONTROLLERSELECTDISTANCEFACTOR) (void* self, const f32 value);
    typedef const void(CDECLAPI* SOUNDCONTROLLESELECTENVIRONMENTALAUDIOEXTENSIONSENVIRONMENT) (void* self, const s32 indx, const f32 volume, const f32 time); // TODO damping, ...
    typedef const u32(CDECLAPI* SOUNDCONTROLLERRCREATESECONDARYBUFFER) (void* self, const SoundSampleDescriptor* desc);
    typedef const void(CDECLAPI* SOUNDCONTROLLERRELEASESAMPLE) (void* self, const s32 indx);
    typedef void* (CDECLAPI* SOUNDCONTROLLERLOCKACTIVESOUNDSAMPLEBUFFER) (void* self, const s32 indx, const s32 offset, const s32 length, const SoundSampleDescriptor* desc);
    typedef const s32(CDECLAPI* SOUNDCONTROLLERUNLOCKACTIVESOUNDSAMPLEBUFFER) (void* self, const s32 indx, const s32 offset, const s32 length, const SoundSampleDescriptor* desc);
    typedef const u32(CDECLAPI* SOUNDCONTROLLERUNKNOWNVALUE) (void* self, const u32 indx);
    typedef const u32(CDECLAPI* SOUNDCONTROLLERALLOCATESOUNDEFFECT) (void* self, const s32 indx);
    typedef const BOOL(CDECLAPI* SOUNDCONTROLLERSELECTSOUNDEFFECTOPTIONS) (void* self, SoundEffect* effect, const s32 todo);
    typedef const f32(CDECLAPI* SOUNDCONTROLLERACQUIRESOUNDEFFECTPOSITION) (void* self, SoundEffect* effect);
    typedef const BOOL(CDECLAPI* SOUNDCONTROLLERSTARTSOUNDEFFECT) (void* self, SoundEffect* effect);
    typedef const void(CDECLAPI* SOUNDCONTROLLERRELEASEEFFECT) (void* self, SoundEffect* effect);
    typedef const BOOL(CDECLAPI* SOUNDCONTROLLERACQUIREACTIVESOUNDEFFECTBUFFERSTATUS) (void* self, SoundEffect* effect);

    BOOL InitializeSoundControllerWrapper(void* self);
    BOOL StartSoundControllerSecondaryBufferWrapper(void* self);
    BOOL StopSoundControllerBuffersWrapper(void* self);
    BOOL CreateSoundControllerBuffersWrapper(void* self, const u32 bits, const u16 channels, const u32 hz, u32* count);
    BOOL AcquireSoundControlleSecondaryBufferPositionWrapper(void* self);
    SoundMixMode QuerySoundControllerDeviceCapabilitiesWrapper(void* self);
    void CommitSoundControllerSoundEffectSettingsWrapper(void* self);
    void SelectSoundControllerListenerPositionWrapper(void* self, const f32 x, const f32 y, const f32 z);
    void SelectSoundControllerListenerOrientationWrapper(void* self, const f32 x1, const f32 y1, const f32 z1, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf);
    void SelectSoundControllerListenerVelocityWrapper(void* self, const f32 x, const f32 y, const f32 z);
    void SelectSoundControllerListenerDistanceFactorWrapper(void* self, const f32 value);
    u32 CreateSoundControllerSecondaryBufferWrapper(void* self, const SoundSampleDescriptor* desc);
    void ReleaseSoundControllerSampleWrapper(void* self, const s32 indx);
    void* LockActiveSoundControllerSoundSampleBufferWrapper(void* self, const s32 indx, const s32 offset, const s32 length, const SoundSampleDescriptor* desc);
    s32 UnlockActiveSoundControllerSoundSampleBufferWrapper(void* self, const s32 indx, const s32 offset, const s32 length, const SoundSampleDescriptor* desc);
    void SelectSoundControllerEnvironmentalAudioExtensionsEnvironmentWrapper(void* self, const s32 indx, const f32 volume, const f32 time); // TODO dumping, ...
    s32 UnknownSoundControllerValueWrapper(void* self, const u32 indx);
    u32 AllocateSoundControllerSoundEffectWrapper(void* self, const s32 indx);
    BOOL SelectSoundControllerSoundEffectOptionsWrapper(void* self, SoundEffect* effect, const u32 mode);
    f32 AcquireSoundControllerSoundEffectPositionWrapper(void* self, SoundEffect* effect);
    BOOL StartSoundControllerSoundEffectWrapper(void* self, SoundEffect* effect);
    void ReleaseSoundControllerSoundEffectWrapper(void* self, SoundEffect* effect);
    BOOL AcquireSoundControllerActiveSoundEffectBufferStatusWrapper(void* self, SoundEffect* effect);

    struct SoundControllerSelf
    {
        SOUNDCONTROLLERINITIALIZE Initialize = (SOUNDCONTROLLERINITIALIZE)&InitializeSoundControllerWrapper; // 0x0057b470
        SOUNDCONTROLLERSTARTSECONDARYBUFFER StartSecondaryBuffer = (SOUNDCONTROLLERSTARTSECONDARYBUFFER)&StartSoundControllerSecondaryBufferWrapper; // 0x0057b540
        SOUNDCONTROLLERSTOPBUFFERS StopBuffers = (SOUNDCONTROLLERSTOPBUFFERS)&StopSoundControllerBuffersWrapper; // 0x0057b6b0
        SOUNDCONTROLLERCREATEBUFFERS CreateBuffers = (SOUNDCONTROLLERCREATEBUFFERS)&CreateSoundControllerBuffersWrapper; // 0x0057b970
        SOUNDCONTROLLERACQUIRESECONDARYBUFFERPOSITION AcquireSecondaryBufferPosition = (SOUNDCONTROLLERACQUIRESECONDARYBUFFERPOSITION)&AcquireSoundControlleSecondaryBufferPositionWrapper; // 0x0057bea0
        SOUNDCONTROLLERQUERYDEVICECAPABILITIES QueryCapabilities = (SOUNDCONTROLLERQUERYDEVICECAPABILITIES)&QuerySoundControllerDeviceCapabilitiesWrapper; // 0x0057bf80
        SOUNDCONTROLLERCOMMITSOUNDEFFECTSETTINGS CommitSoundEffectSettings = (SOUNDCONTROLLERCOMMITSOUNDEFFECTSETTINGS)&CommitSoundControllerSoundEffectSettingsWrapper; // 0x0057dad0
        SOUNDCONTROLLERSELECTLISTENERPOSITION SelectListenerPosition = (SOUNDCONTROLLERSELECTLISTENERPOSITION)&SelectSoundControllerListenerPositionWrapper; // 0x0057c040
        SOUNDCONTROLLERSELECTLISTENERORIENTATION SelectListenerOrientation = (SOUNDCONTROLLERSELECTLISTENERORIENTATION)SelectSoundControllerListenerOrientationWrapper; // 0x0057c070
        SOUNDCONTROLLERSELECTLISTENERVELOCITY SetListenerVelocity = (SOUNDCONTROLLERSELECTLISTENERVELOCITY)&SelectSoundControllerListenerVelocityWrapper; // 0x0057c0a0
        SOUNDCONTROLLERSELECTDISTANCEFACTOR SetListenerDistanceFactor = (SOUNDCONTROLLERSELECTDISTANCEFACTOR)&SelectSoundControllerListenerDistanceFactorWrapper; // 0x0057c0d0
        SOUNDCONTROLLESELECTENVIRONMENTALAUDIOEXTENSIONSENVIRONMENT SelectEnvironmentalAudioExtensionsEnvironment = (SOUNDCONTROLLESELECTENVIRONMENTALAUDIOEXTENSIONSENVIRONMENT)&SelectSoundControllerEnvironmentalAudioExtensionsEnvironmentWrapper; // 0x0057da20
        SOUNDCONTROLLERRCREATESECONDARYBUFFER CreateSecondaryBuffer = (SOUNDCONTROLLERRCREATESECONDARYBUFFER)&CreateSoundControllerSecondaryBufferWrapper; // 0x0057c100
        SOUNDCONTROLLERRELEASESAMPLE ReleaseSample = (SOUNDCONTROLLERRELEASESAMPLE)&ReleaseSoundControllerSampleWrapper; // 0x0057c3e0
        SOUNDCONTROLLERLOCKACTIVESOUNDSAMPLEBUFFER LockSample = (SOUNDCONTROLLERLOCKACTIVESOUNDSAMPLEBUFFER)&LockActiveSoundControllerSoundSampleBufferWrapper; // 0x0057c440
        SOUNDCONTROLLERUNLOCKACTIVESOUNDSAMPLEBUFFER UnlockActiveSoundSampleBuffer = (SOUNDCONTROLLERUNLOCKACTIVESOUNDSAMPLEBUFFER)&UnlockActiveSoundControllerSoundSampleBufferWrapper; // 0x0057c5a0
        SOUNDCONTROLLERUNKNOWNVALUE UnknownValue = (SOUNDCONTROLLERUNKNOWNVALUE)UnknownSoundControllerValueWrapper; // 0x00578820
        SOUNDCONTROLLERALLOCATESOUNDEFFECT AllocateSoundEffect = (SOUNDCONTROLLERALLOCATESOUNDEFFECT)&AllocateSoundControllerSoundEffectWrapper; // 0x0057c6a0
        SOUNDCONTROLLERSELECTSOUNDEFFECTOPTIONS SelectSoundEffectOptions = (SOUNDCONTROLLERSELECTSOUNDEFFECTOPTIONS)&SelectSoundControllerSoundEffectOptionsWrapper; // 0x0057c6a0
        SOUNDCONTROLLERACQUIRESOUNDEFFECTPOSITION AcquireSoundEffectPosition = (SOUNDCONTROLLERACQUIRESOUNDEFFECTPOSITION)&AcquireSoundControllerSoundEffectPositionWrapper; // 0x0057d200
        SOUNDCONTROLLERSTARTSOUNDEFFECT StartSoundEffect = (SOUNDCONTROLLERSTARTSOUNDEFFECT)&StartSoundControllerSoundEffectWrapper; // 0x0057d330
        SOUNDCONTROLLERRELEASEEFFECT ReleaseSoundEffect = (SOUNDCONTROLLERRELEASEEFFECT)&ReleaseSoundControllerSoundEffectWrapper; // 0x0057d790
        SOUNDCONTROLLERACQUIREACTIVESOUNDEFFECTBUFFERSTATUS AcquireActiveSoundEffectBufferStatus = (SOUNDCONTROLLERACQUIREACTIVESOUNDEFFECTBUFFERSTATUS)&AcquireSoundControllerActiveSoundEffectBufferStatusWrapper; // 0x0057d150
    };

    struct SoundController
    {
        SoundControllerSelf* Self;// TODO. The game initializes this in the "ENTRY" phase by a static constructor.
    };

    struct SoundControllerContainer
    {
        SoundControllerSelf SoundControllerSelf;
    };

    extern SoundControllerContainer SoundControllerState;

    void SoundControllerConstructor(void);

    BOOL InitializeSoundController(SoundController* self);
    BOOL StartSoundControllerSecondaryBuffer(SoundController* self);
    BOOL StopSoundControllerBuffers(SoundController* self);
    BOOL CreateSoundControllerBuffers(SoundController* self, const u32 bits, const u16 channels, const u32 hz, u32* count);
    BOOL AcquireSoundControlleSecondaryBufferPosition(SoundController* self);
    SoundMixMode QuerySoundControllerDeviceCapabilities(SoundController* self);
    void CommitSoundControllerSoundEffectSettings(SoundController* self);
    void SelectSoundControllerListenerPosition(SoundController* self, const f32 x, const f32 y, const f32 z);
    void SelectSoundControllerListenerOrientation(SoundController* self, const f32 x1, const f32 y1, const f32 z1, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf);
    void SelectSoundControllerListenerVelocity(SoundController* self, const f32 x, const f32 y, const f32 z);
    void SelectSoundControllerListenerDistanceFactor(SoundController* self, const f32 value);
    void SelectSoundControllerEnvironmentalAudioExtensionsEnvironment(SoundController* self, const s32 indx, const f32 volume, const f32 time); // TODO dumping, ...
    u32 CreateSoundControllerSecondaryBuffer(SoundController* self, const SoundSampleDescriptor* desc);
    void ReleaseSoundControllerSample(SoundController* self, const s32 indx);
    void* LockActiveSoundControllerSoundSampleBuffer(SoundController* self, const s32 indx, const s32 offset, const s32 length, const SoundSampleDescriptor* desc);
    s32 UnlockActiveSoundControllerSoundSampleBuffer(SoundController* self, const s32 indx, const s32 offset, const s32 length, const SoundSampleDescriptor* desc);
    s32 UnknownSoundControllerValue(SoundController* self, const u32 indx);
    u32 AllocateSoundControllerSoundEffect(SoundController* self, const s32 indx);
    BOOL SelectSoundControllerSoundEffectOptions(SoundController* self, SoundEffect* effect, const u32 mode);
    f32 AcquireSoundControllerSoundEffectPosition(SoundController* self, SoundEffect* effect);
    BOOL StartSoundControllerSoundEffect(SoundController* self, SoundEffect* effect);
    void ReleaseSoundControllerSoundEffect(SoundController* self, SoundEffect* effect);
    BOOL AcquireSoundControllerActiveSoundEffectBufferStatus(SoundController* self, SoundEffect* effect);

    BOOL StopSoundControllerSecondaryBuffer(const s32 indx);

    SoundController* InitializeSoundController1(const s32 indx);

    typedef const BOOL(CDECLAPI* SOUNDRECORDINGCONTROLLERINITIALIZE) (void* self);
    typedef const BOOL(CDECLAPI* SOUNDRECORDINGCONTROLLERSTART) (void* self);
    typedef const BOOL(CDECLAPI* SOUNDRECORDINGCONTROLLERSTOP) (void* self);

    // NOTE: This is never used.
    struct SoundRecordingControllerSelf
    {
        SOUNDRECORDINGCONTROLLERINITIALIZE Initialize;
        SOUNDRECORDINGCONTROLLERSTART Start;
        SOUNDRECORDINGCONTROLLERSTOP Stop;
    };

    // NOTE: This is never used.
    struct SoundRecordingController
    {
        SoundRecordingControllerSelf* Self;
    };
}