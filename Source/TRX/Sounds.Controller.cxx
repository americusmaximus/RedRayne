#include "App.Windows.hxx"
#include "Logger.hxx"
#include "Sounds.Controller.hxx"
#include "Sounds.Devices.hxx"
#include "Sounds.EAX.hxx"
#include "Sounds.hxx"

#include <math.h>

#define DS3DMODE_NONE 0
#define DSBLOCK_NONE 0
#define DSBPLAY_NONE 0
#define DSBSTATUS_NONE 0
#define DSBPRIORITY_NONE 0

using namespace App::Windows;
using namespace Logger;
using namespace Assets::Sounds;

namespace Sounds
{
    SoundControllerContainer SoundControllerState;

    GUID IKsPropertySetImpl_QuerySupport = { 0x62a69bae, 0xdf9d, 0x11d1, 0x99, 0xa6, 0x00, 0xc0, 0x4f, 0xc9, 0x9d, 0x46 }; // DSPROPSETID_VoiceManager
    GUID DSPROPSETID_EAX20_ListenerProperties = { 0x306a6a8, 0xb224, 0x11d2, { 0x99, 0xe5, 0x00, 0x00, 0xe8, 0xd8, 0xc7, 0x22 } };

    void SoundControllerConstructor(void)
    {
        SoundState._SoundController->Self = &SoundControllerState.SoundControllerSelf;
    }

    BOOL InitializeSoundControllerWrapper(void* self)
    {
        return InitializeSoundController((SoundController*)self);
    }

    BOOL InitializeSoundController(SoundController* self)
    {
        const auto result = self->Self->StopBuffers(self);

        for (u32 x = 0; x < 257; x++) // TODO constant relates to SoundUnk0x18 SoundUnk0x18Array[257]
        {
            self->Self->ReleaseSample(self, x);
        }

        if (SoundState.DirectSound.Listener != NULL)
        {
            SoundState.DirectSound.Listener->Release();

            SoundState.DirectSound.Listener = NULL;
        }

        if (SoundState.DirectSound.Buffers.Secondary.Buffer != NULL)
        {
            SoundState.DirectSound.Buffers.Secondary.Buffer->Release();

            SoundState.DirectSound.Buffers.Secondary.Buffer = NULL;
        }

        if (SoundState.EAX.Instance != NULL)
        {
            SoundState.EAX.Instance->Release();

            SoundState.EAX.Instance = NULL;
        }

        if (SoundState.DirectSound.Buffers.Primary.Buffer != NULL)
        {
            SoundState.DirectSound.Buffers.Primary.Buffer->Release();

            SoundState.DirectSound.Buffers.Primary.Buffer = NULL;
        }

        if (SoundState.DirectSound.Instance != NULL)
        {
            SoundState.DirectSound.Instance->Release();

            SoundState.DirectSound.Instance = NULL;
        }

        return result;
    }

    SoundController* InitializeSoundController1(const s32 indx)
    {
        if (!InitializeSoundController(SoundState._SoundController)) { return NULL; }

        SoundDevice device;

        if (!EnumerateSoundDevices(indx, &device)) { return NULL; }

        GUID* uid = NULL;

        if (!SoundDeviceState.EnumeratedDevices[indx].IsPrimary)
        {
            uid = &SoundDeviceState.EnumeratedDevices[indx].UID;
        }

        if (DSC(DirectSoundCreate(uid, &SoundState.DirectSound.Instance, NULL),
            "Unable to create Direct Sound instance.") == DS_OK)
        {
            if (DSC(SoundState.DirectSound.Instance->SetCooperativeLevel(
                *AppWindowState._HWND, DSSCL_PRIORITY),
                "Unable to set Direct Sound cooperative level.") == DS_OK)
            {
                DSBUFFERDESC desc =
                {
                    .dwSize = sizeof(DSBUFFERDESC),
                    .dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER
                };

                if (DSC(SoundState.DirectSound.Instance->CreateSoundBuffer(
                    &desc, &SoundState.DirectSound.Buffers.Primary.Buffer, NULL),
                    "Unable to create primary sound buffer.") != DS_OK)
                {
                    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

                    if (DSC(SoundState.DirectSound.Instance->CreateSoundBuffer(
                        &desc, &SoundState.DirectSound.Buffers.Primary.Buffer, NULL),
                        "Unable to create primary sound buffer.") != DS_OK)
                    {
                        InitializeSoundController(SoundState._SoundController);

                        return NULL;
                    }
                }

                if (AcquireSoundMixMode2() == SoundMixMode::Advanced
                    && SoundDeviceState.EnumeratedDevices[indx].MixMode == SoundMixMode::Advanced)
                {
                    SoundState.DirectSound.Buffers.Primary.Buffer->QueryInterface(
                        IID_IDirectSound3DListener, (void**)&SoundState.DirectSound.Listener);
                }

                SoundState.EAX.Index = -1; // TODO constant

                return SoundState._SoundController;
            }
        }

        InitializeSoundController(SoundState._SoundController);

        return NULL;
    }

    BOOL CreateSoundControllerBuffersWrapper(void* self, const u32 bits, const u16 channels, const u32 hz, u32* count)
    {
        return CreateSoundControllerBuffers((SoundController*)self, bits, channels, hz, count);
    }

    BOOL CreateSoundControllerBuffers(SoundController* self, const u32 bits, const u16 channels, const u32 hz, u32* count)
    {
        if (SoundState.EAX.Instance != NULL)
        {
            SoundState.EAX.Instance->Release();

            SoundState.EAX.Instance = NULL;
        }

        if (SoundState.DirectSound.Buffers.Primary.Buffer != NULL)
        {
            WAVEFORMATEX pf =
            {
                .wFormatTag = WAVE_FORMAT_PCM,
                .nChannels = channels,
                .nSamplesPerSec = hz,
                .nAvgBytesPerSec = hz * (bits >> 3) * channels,
                .nBlockAlign = (u16)((bits >> 3) * channels),
                .wBitsPerSample = (u16)bits
            };

            if (DSC(SoundState.DirectSound.Buffers.Primary.Buffer->SetFormat(&pf),
                "Unable to set primary sound buffer format.") == DS_OK)
            {
                if (DSC(SoundState.DirectSound.Buffers.Primary.Buffer->GetFormat(
                    &pf, sizeof(WAVEFORMATEX), NULL),
                    "Unable to get primary sound buffer format.") == DS_OK)
                {
                    SoundState.DirectSound.Buffers.Primary.BitsPerSample = pf.wBitsPerSample;
                    SoundState.DirectSound.Buffers.Primary.HZ = pf.nSamplesPerSec;
                    SoundState.DirectSound.Buffers.Primary.Channels = pf.nChannels;

                    if (SoundState.EAX.Instance != NULL)
                    {
                        SoundState.EAX.Instance->Release();

                        SoundState.EAX.Instance = NULL;
                    }

                    WAVEFORMATEX tbf =
                    {
                        .wFormatTag = WAVE_FORMAT_PCM,
                        .nChannels = 1, // TODO constant
                        .nSamplesPerSec = 22050, // TODO constant
                        .nAvgBytesPerSec = 22050 * 2, // TODO constant
                        .nBlockAlign = 2, // TODO constant
                        .wBitsPerSample = 16 // TODO constant
                    };

                    DSBUFFERDESC tbd =
                    {
                        .dwSize = sizeof(DSBUFFERDESC),
                        .dwFlags = DSBCAPS_CTRL3D | DSBCAPS_STATIC,
                        .dwBufferBytes = 1024, // TODO constant
                        .lpwfxFormat = &tbf
                    };

                    IDirectSoundBuffer* tb;

                    if (DSC(SoundState.DirectSound.Instance->CreateSoundBuffer(&tbd, &tb, NULL),
                        "Unable to create temporary secondary sound buffer for property set creation.") == DS_OK)
                    {
                        IDirectSound3DBuffer* buff;

                        tb->QueryInterface(IID_IDirectSound3DBuffer, (void**)&buff);

                        if (buff == NULL)
                        {
                            if (tb != NULL)
                            {
                                tb->Release();

                                tb = NULL;
                            }
                        }
                        else
                        {
                            buff->QueryInterface(IID_IKsPropertySet, (void**)&SoundState.EAX.Instance);

                            if (buff != NULL)
                            {
                                buff->Release();

                                buff = NULL;
                            }

                            if (tb != NULL)
                            {
                                tb->Release();

                                tb = NULL;
                            }
                        }
                    }

                    if (SoundState.EAX.Instance != NULL)
                    {
                        BOOL value = TRUE;
                        SoundState.EAX.Instance->Set(IKsPropertySetImpl_QuerySupport, NULL, NULL, NULL, &value, sizeof(BOOL));
                    }

                    if (AcquireSoundMixMode1() != SoundMixMode::None)
                    {
                        self = NULL; // TODO ???

                        return TRUE;
                    }

                    SoundState.DirectSound.Buffers.Secondary.Unknown1 = 8; // TODO

                    // TODO
                    const auto c = (u32)roundf((AcquireMaximumSoftWareSoundLatency()
                        * SoundState.DirectSound.Buffers.Primary.HZ) / (f32)SoundState.DirectSound.Buffers.Secondary.Unknown1);

                    SoundState.DirectSound.Buffers.Secondary.Count = (c + 0xf) & 0xfffffff0; // TODO

                    auto align = SoundState.DirectSound.Buffers.Primary.Channels * (SoundState.DirectSound.Buffers.Primary.BitsPerSample >> 3);

                    SoundState.DirectSound.Buffers.Secondary.ChannelBufferSize = align * SoundState.DirectSound.Buffers.Secondary.Count;

                    WAVEFORMATEX sf =
                    {
                        .wFormatTag = WAVE_FORMAT_PCM,
                        .nChannels = 2, // TODO constant
                        .nSamplesPerSec = 44100, // TODO constant
                        .nAvgBytesPerSec = 44100 * align, // TODO constant
                        .nBlockAlign = (u16)align,
                        .wBitsPerSample = 16 // TODO constant
                    };

                    DSBUFFERDESC sdesc =
                    {
                        .dwSize = sizeof(DSBUFFERDESC),
                        .dwBufferBytes = SoundState.DirectSound.Buffers.Secondary.Unknown1 * SoundState.DirectSound.Buffers.Secondary.ChannelBufferSize,
                        .lpwfxFormat = &sf
                    };

                    if (SoundState.Debug.GlobalFocusMode)
                    {
                        sdesc.dwFlags = DSBCAPS_GLOBALFOCUS;
                    }

                    if (SoundState.DirectSound.Buffers.Secondary.Buffer != NULL)
                    {
                        SoundState.DirectSound.Buffers.Secondary.Buffer->Release();

                        SoundState.DirectSound.Buffers.Secondary.Buffer = NULL;
                    }

                    if (DSC(SoundState.DirectSound.Instance->CreateSoundBuffer(&sdesc, &SoundState.DirectSound.Buffers.Secondary.Buffer, NULL),
                        "Unable to create secondary sound buffer.") == DS_OK)
                    {
                        *count = SoundState.DirectSound.Buffers.Secondary.Count;

                        return TRUE;
                    }
                }
            }
        }

        if (SoundState.DirectSound.Buffers.Secondary.Buffer != NULL)
        {
            SoundState.DirectSound.Buffers.Secondary.Buffer->Release();

            SoundState.DirectSound.Buffers.Secondary.Buffer = NULL;
        }

        return FALSE;
    }

    BOOL AcquireSoundControlleSecondaryBufferPositionWrapper(void* self)
    {
        return AcquireSoundControlleSecondaryBufferPosition((SoundController*)self);
    }
    
    BOOL AcquireSoundControlleSecondaryBufferPosition(SoundController* self)
    {
        if (AcquireSoundMixMode1() == SoundMixMode::None)
        {
            if (SoundState.DirectSound.Buffers.Secondary.Buffer == NULL) { return FALSE; }

            DWORD play = 0;
            DWORD write = 0;

            if (DSC(SoundState.DirectSound.Buffers.Secondary.Buffer->GetCurrentPosition(&play, &write),
                "Unable to acquire secondary sound buffer position.") != DS_OK)
            {
                return FALSE;
            }

            auto value = play / SoundState.DirectSound.Buffers.Secondary.ChannelBufferSize;

            if (SoundState.DirectSound.Buffers.Secondary.Unknown1 < value)
            {
                value = SoundState.DirectSound.Buffers.Secondary.Unknown1 - 1;
            }

            if ((s32)value < 0) { value = 0; } // TODO constant

            do
            {
                if (value == SoundState.DirectSound.Buffers.Secondary.Unknown2) { return TRUE; }

            } while (InitializeSecondarySoundBuffer1());

            return FALSE;
        }
        else
        {
            UnknownPlay101();
        }

        return TRUE;
    }

    BOOL StopSoundControllerBuffersWrapper(void* self)
    {
        return StopSoundControllerBuffers((SoundController*)self);
    }

    BOOL StopSoundControllerBuffers(SoundController* self)
    {
        auto result = TRUE;

        if (SoundState.DirectSound.Buffers.Secondary.Buffer != NULL)
        {
            if (DSC(SoundState.DirectSound.Buffers.Secondary.Buffer->Stop(),
                "Unable to stop secondaty sound buffer.") != DS_OK)
            {
                result = FALSE;
            }
        }

        if (SoundState.DirectSound.Buffers.Primary.Buffer != NULL)
        {
            if (DSC(SoundState.DirectSound.Buffers.Primary.Buffer->Stop(),
                "Unable to stop primary sound buffer.") != DS_OK)
            {
                result = FALSE;
            }
        }

        for (u32 x = INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX; x < MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE; x++)
        {
            if (!StopSoundControllerSecondaryBuffer(x))
            {
                result = FALSE;
            }
        }

        if (StopSoundThread()) { return result; }

        return FALSE;
    }

    BOOL StartSoundControllerSecondaryBufferWrapper(void* self)
    {
        return StartSoundControllerSecondaryBuffer((SoundController*)self);
    }

    BOOL StartSoundControllerSecondaryBuffer(SoundController* self)
    {
        f32 time = 0.1f;//TODO constant

        if (SoundState.DirectSound.Buffers.Secondary.Buffer != NULL)
        {
            SoundState.DirectSound.Buffers.Secondary.ChannelBufferSize =
                (SoundState.DirectSound.Buffers.Primary.BitsPerSample >> 3)
                * SoundState.DirectSound.Buffers.Secondary.Count
                * SoundState.DirectSound.Buffers.Primary.Channels;

            time = 0.5f * (f32)SoundState.DirectSound.Buffers.Secondary.Count
                / (f32)SoundState.DirectSound.Buffers.Primary.HZ; // TODO constant

            SoundState.DirectSound.Buffers.Secondary.Unknown2 = 0; // TODO constant

            for (u32 x = 0; x < SoundState.DirectSound.Buffers.Secondary.Unknown1; x++)
            {
                if (!InitializeSecondarySoundBuffer1())
                {
                    self->Self->StopBuffers(self);

                    return FALSE;
                }
            }
        }

        if (RestartSoundThread(time))
        {
            if (SoundState.DirectSound.Buffers.Secondary.Buffer == NULL) { return TRUE; }

            if (DSC(SoundState.DirectSound.Buffers.Secondary.Buffer->SetCurrentPosition(0),
                "Unable to set secondary sound buffer playback position to 0.") == DS_OK)
            {
                if (DSC(SoundState.DirectSound.Buffers.Secondary.Buffer->Play(NULL, DSBPRIORITY_NONE, DSBPLAY_LOOPING),
                    "Unable to play secondary sound buffer.") == DS_OK)
                {
                    return TRUE;
                }
            }
        }

        self->Self->StopBuffers(self);

        return FALSE;
    }

    SoundMixMode QuerySoundControllerDeviceCapabilitiesWrapper(void* self)
    {
        return QuerySoundControllerDeviceCapabilities((SoundController*)self);
    }

    SoundMixMode QuerySoundControllerDeviceCapabilities(SoundController* self)
    {
        if (SoundState.DirectSound.Instance == NULL) { return SoundMixMode::None; }

        if (SoundState.DirectSound.Listener == NULL)
        {
            if (AcquireSoundMixMode2() == SoundMixMode::None) { return SoundMixMode::None; }

            DSCAPS caps = { .dwSize = sizeof(DSCAPS) };

            if (DSC(SoundState.DirectSound.Instance->GetCaps(&caps), "Unable to query Direct Sound capabilities.") == DS_OK)
            {
                return SoundMixMode::Simple;
            }

            return SoundMixMode::None;
        }

        return SoundMixMode::Advanced;
    }

    void CommitSoundControllerSoundEffectSettingsWrapper(void* self)
    {
        CommitSoundControllerSoundEffectSettings((SoundController*)self);
    }

    void CommitSoundControllerSoundEffectSettings(SoundController* self)
    {
        if (SoundState.DirectSound.Listener != NULL)
        {
            DSC(SoundState.DirectSound.Listener->CommitDeferredSettings(),
                "Unable to commit sound buffer settings.");
        }
    }

    void SelectSoundControllerListenerPositionWrapper(void* self, const f32 x, const f32 y, const f32 z)
    {
        SelectSoundControllerListenerPosition((SoundController*)self, x, y, z);
    }

    void SelectSoundControllerListenerPosition(SoundController* self, const f32 x, const f32 y, const f32 z)
    {
        if (SoundState.DirectSound.Listener != NULL)
        {
            SoundState.DirectSound.Listener->SetPosition(x, y, z, TRUE);
        }
    }

    void SelectSoundControllerListenerOrientationWrapper(void* self, const f32 x1, const f32 y1, const f32 z1, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf)
    {
        SelectSoundControllerListenerOrientation((SoundController*)self, x1, y1, z1, xt, yt, zt, xf, yf, zf);
    }

    void SelectSoundControllerListenerOrientation(SoundController* self, const f32 x1, const f32 y1, const f32 z1, const f32 xt, const f32 yt, const f32 zt, const f32 xf, const f32 yf, const f32 zf)
    {
        if (SoundState.DirectSound.Listener != NULL)
        {
            SoundState.DirectSound.Listener->SetOrientation(xf, yf, zf, xt, yt, zt, TRUE);
        }
    }

    void SelectSoundControllerListenerVelocityWrapper(void* self, const f32 x, const f32 y, const f32 z)
    {
        SelectSoundControllerListenerVelocity((SoundController*)self, x, y, z);
    }

    void SelectSoundControllerListenerVelocity(SoundController* self, const f32 x, const f32 y, const f32 z)
    {
        if (SoundState.DirectSound.Listener != NULL)
        {
            SoundState.DirectSound.Listener->SetVelocity(x, y, z, TRUE);
        }
    }

    void SelectSoundControllerListenerDistanceFactorWrapper(void* self, const f32 value)
    {
        SelectSoundControllerListenerDistanceFactor((SoundController*)self, value);
    }

    void SelectSoundControllerListenerDistanceFactor(SoundController* self, const f32 value)
    {
        if (SoundState.DirectSound.Listener != NULL)
        {
            SoundState.DirectSound.Listener->SetDistanceFactor(value * 0.3048f, FALSE); // TODO
        }
    }

    f32 AcquireSoundControllerSoundEffectPositionWrapper(void* self, SoundEffect* effect)
    {
        return AcquireSoundControllerSoundEffectPosition((SoundController*)self, effect);
    }

    f32 AcquireSoundControllerSoundEffectPosition(SoundController* self, SoundEffect* effect)
    {
        const auto indx = effect->UnknownIndex;

        if (indx < MIN_ACTIVE_SOUND_BUFFER_POOL_INDEX || (MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE - 1) < indx
            || SoundState.SoundUnk0x24Array[indx].Buffer == NULL)
        {
            LogError("Unable to get sound effect playback position, invalid index.");
        }

        if (effect->Sample != NULL)
        {
            DWORD play = 0;
            DWORD write = 0;

            if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->GetCurrentPosition(&play, &write),
                "Unable to acquire secondary sound buffer playback position.") != DS_OK)
            {
                return -1.0f; // TODO constant
            }

            return (f32)AcquireSoundSampleDescriptorValue204(&effect->Sample->Descriptor, play, 0);
        }

        return -1.0f; // TODO constant
    }

    u32 CreateSoundControllerSecondaryBufferWrapper(void* self, const SoundSampleDescriptor* desc)
    {
        return CreateSoundControllerSecondaryBuffer((SoundController*)self, desc);
    }

    u32 CreateSoundControllerSecondaryBuffer(SoundController* self, const SoundSampleDescriptor* desc)
    {
        if (SoundState.DirectSound.Instance == NULL) { return INVALID_SOUND_BUFFER_POOL_INDEX; }

        u32 indx = MIN_SOUND_BUFFER_POOL_INDEX;

        for (u32 x = indx; x < MAX_SOUND_BUFFER_POOL_SIZE; x++)
        {
            if (SoundState.SoundBufferPool[x].Buffer == NULL) { break; }

            indx = indx + 1;
        }

        if (MAX_SOUND_BUFFER_POOL_SIZE < indx) { return INVALID_SOUND_BUFFER_POOL_INDEX; }

        WAVEFORMATEX bf = { .cbSize = sizeof(WAVEFORMATEX) };

        DSBUFFERDESC bd =
        {
            .dwSize = sizeof(DSBUFFERDESC),
            .lpwfxFormat = &bf
        };

        bd.dwFlags = (SoundState.DirectSound.Listener == NULL)
            ? (DSBCAPS_LOCDEFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY)
            : (DSBCAPS_LOCDEFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D);

        if (desc->Definition.Type == SoundSampleType::WAVYADPCM)
        {
            bf.wFormatTag = WAVE_FORMAT_YAMAHA_ADPCM;
            bf.nChannels = desc->Definition.Channels;
            bf.nSamplesPerSec = desc->Definition.HZ;
            bf.nBlockAlign = desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3) / 2; // TODO constant
            bf.nAvgBytesPerSec = desc->Definition.HZ * desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3) / 2; // TODO constant
            bf.wBitsPerSample = 4;

            bd.dwBufferBytes = desc->Definition.Length * desc->Definition.Channels / 2; // TODO constant
        }
        else
        {
            bf.wFormatTag = WAVE_FORMAT_PCM;
            bf.nChannels = desc->Definition.Channels;
            bf.nSamplesPerSec = desc->Definition.HZ;
            bf.nBlockAlign = desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3);
            bf.nAvgBytesPerSec = desc->Definition.HZ * desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3);
            bf.wBitsPerSample = desc->Definition.BitsPerSample;

            bd.dwBufferBytes = desc->Definition.Length * desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3);
        }

        if (DSC(SoundState.DirectSound.Instance->CreateSoundBuffer(
            &bd, &SoundState.SoundBufferPool[indx].Buffer, NULL),
            "Unable to create secondary sound buffer.") != DS_OK)
        {
            return INVALID_SOUND_BUFFER_POOL_INDEX;
        }

        if (SoundState.SoundBufferPool[indx].Buffer != NULL)
        {
            SoundState.SoundBufferPool[indx].Offset = 256 * desc->Definition.Channels * (desc->Definition.BitsPerSample >> 3); // TODO constant
            SoundState.SoundBufferPool[indx].Length = bd.dwBufferBytes;
            SoundState.SoundBufferPool[indx].Audio = NULL;

            return indx;
        }

        return INVALID_SOUND_BUFFER_POOL_INDEX;
    }

    u32 AllocateSoundControllerSoundEffectWrapper(void* self, const s32 indx)
    {
        return AllocateSoundControllerSoundEffect((SoundController*)self, indx);
    }

    u32 AllocateSoundControllerSoundEffect(SoundController* self, const s32 indx)
    {
        if (indx < MIN_SOUND_BUFFER_POOL_INDEX || MAX_SOUND_BUFFER_POOL_SIZE < indx
            || SoundState.SoundBufferPool[indx].Buffer == NULL
            || SoundState.SoundBufferPool[indx].Offset == 0)
        {
            LogError("Unable to allocate sound effect, invalid index %d.", indx);
        }

        if (SoundState.DirectSound.Instance == NULL)
        {
            LogError("Unable to allocate sound effect, Direct Sound is not initialized.");
        }

        u32 index = MIN_ACTIVE_SOUND_BUFFER_POOL_INDEX;

        for (u32 x = index; x < MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE; x++)
        {
            if (SoundState.SoundUnk0x24Array[x].Buffer == NULL) { break; }

            index = index + 1;
        }

        if ((MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE - 1) < index)
        {
            LogMessage("[ERROR] [SOUND] Unable to allocate sound effect, no free sound buffers available.\n");

            return INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX;
        }

        u32 i = INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX;

        for (i = SoundState.SoundBufferPool[indx].ActiveIndex;
            i != INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX && SoundState.SoundUnk0x24Array[i].Buffer != SoundState.SoundBufferPool[indx].Buffer;
            i = SoundState.SoundUnk0x24Array[i].Unk5) { }

        if (i == INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX)
        {
            SoundState.SoundUnk0x24Array[index].Buffer = SoundState.SoundBufferPool[indx].Buffer;

            SoundState.SoundUnk0x24Array[index].Unk3 = indx;

            if (SoundState.SoundBufferPool[indx].ActiveIndex != INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX)
            {
                SoundState.SoundUnk0x24Array[SoundState.SoundBufferPool[indx].ActiveIndex].Unk4 = index;
            }
        }
        else
        {
            if (DSC(SoundState.DirectSound.Instance->DuplicateSoundBuffer(
                SoundState.SoundBufferPool[indx].Buffer, &SoundState.SoundUnk0x24Array[index].Buffer),
                "Unable to duplicate sound buffer.") != DS_OK)
            {
                return INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX;
            }

            if (SoundState.SoundUnk0x24Array[index].Buffer == NULL) { return INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX; }

            SoundState.SoundUnk0x24Array[SoundState.SoundBufferPool[indx].ActiveIndex].Unk4 = index;
            SoundState.SoundUnk0x24Array[index].Unk3 = indx;
        }

        SoundState.SoundUnk0x24Array[index].Unk4 = 0;
        SoundState.SoundUnk0x24Array[index].Unk5 = SoundState.SoundBufferPool[indx].ActiveIndex;

        SoundState.SoundBufferPool[indx].ActiveIndex = index;

        if (SoundState.DirectSound.Listener != NULL)
        {
            DSC(SoundState.SoundUnk0x24Array[index].Buffer->QueryInterface(
                IID_IDirectSound3DBuffer, (void**)&SoundState.SoundUnk0x24Array[index].AdvancedBuffer),
                "Unable to create a 3D sound buffer from a secondary sound buffer.");

            if (SoundState.SoundUnk0x24Array[index].AdvancedBuffer == NULL)
            {
                StopSoundControllerSecondaryBuffer(index);

                return INVALID_ACTIVE_SOUND_BUFFER_POOL_INDEX;
            }
        }

        SoundState.SoundUnk0x24Array[index].HZ = -1; // TODO constant
        SoundState.SoundUnk0x24Array[index].Unk9 = -1; // TODO constant
        SoundState.SoundUnk0x24Array[index].Pan = DSBPAN_CENTER;
        SoundState.SoundUnk0x24Array[index].Volume = DSBVOLUME_MAX;

        return index;
    }

    BOOL StopSoundControllerSecondaryBuffer(const s32 indx)
    {
        auto result = TRUE;

        if (SoundState.SoundUnk0x24Array[indx].Buffer != NULL)
        {
            result = DSC(SoundState.SoundUnk0x24Array[indx].Buffer->Stop(),
                "Unable to stop secondary sound effect buffer.") == DS_OK;

            if (SoundState.SoundUnk0x24Array[indx].AdvancedBuffer != NULL)
            {
                SoundState.SoundUnk0x24Array[indx].AdvancedBuffer->Release();
                SoundState.SoundUnk0x24Array[indx].AdvancedBuffer = NULL;
            }

            if (SoundState.SoundUnk0x24Array[indx].Unk5 != 0)
            {
                SoundState.SoundUnk0x24Array[SoundState.SoundUnk0x24Array[indx].Unk5].Unk4 = SoundState.SoundUnk0x24Array[indx].Unk4;
            }

            if (indx == SoundState.SoundBufferPool[SoundState.SoundUnk0x24Array[indx].Unk3].ActiveIndex)
            {
                SoundState.SoundBufferPool[SoundState.SoundUnk0x24Array[indx].Unk3].ActiveIndex = SoundState.SoundUnk0x24Array[indx].Unk5;
            }
            else
            {
                SoundState.SoundUnk0x24Array[SoundState.SoundUnk0x24Array[indx].Unk4].Unk5 = SoundState.SoundUnk0x24Array[indx].Unk5;
            }

            if (SoundState.SoundUnk0x24Array[indx].Buffer != NULL
                && SoundState.SoundUnk0x24Array[indx].Buffer != SoundState.SoundBufferPool[SoundState.SoundUnk0x24Array[indx].Unk3].Buffer)
            {
                SoundState.SoundUnk0x24Array[indx].Buffer->Release();

                SoundState.SoundUnk0x24Array[indx].Buffer = NULL;
            }
        }

        SoundState.SoundUnk0x24Array[indx].Buffer = NULL;

        SoundState.SoundUnk0x24Array[indx].Unk5 = 0;
        SoundState.SoundUnk0x24Array[indx].Unk4 = 0;
        SoundState.SoundUnk0x24Array[indx].Unk3 = 0;

        return result;
    }

    void ReleaseSoundControllerSoundEffectWrapper(void* self, SoundEffect* effect)
    {
        ReleaseSoundControllerSoundEffect((SoundController*)self, effect);
    }

    void ReleaseSoundControllerSoundEffect(SoundController* self, SoundEffect* effect)
    {
        const auto indx = effect->UnknownIndex;

        if (indx < MIN_ACTIVE_SOUND_BUFFER_POOL_INDEX || (MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE - 1) < indx
            || SoundState.SoundUnk0x24Array[indx].Buffer == NULL)
        {
            LogMessage("Unable to release sound effect, invalid index %d.", indx);

            return;
        }

        StopSoundControllerSecondaryBuffer(indx);
    }

    void ReleaseSoundControllerSampleWrapper(void* self, const s32 indx)
    {
        ReleaseSoundControllerSample((SoundController*)self, indx);
    }

    void ReleaseSoundControllerSample(SoundController* self, const s32 indx)
    {
        if (SoundState.SoundBufferPool[indx].Buffer != NULL)
        {
            SoundState.SoundBufferPool[indx].Buffer->Release();

            SoundState.SoundBufferPool[indx].Buffer = NULL;
        }

        SoundState.SoundBufferPool[indx].Offset = 0;
        SoundState.SoundBufferPool[indx].Length = 0;

        SoundState.SoundBufferPool[indx].Audio = NULL;
        SoundState.SoundBufferPool[indx].AudioSize = 0;
    }

    BOOL AcquireSoundControllerActiveSoundEffectBufferStatusWrapper(void* self, SoundEffect* effect)
    {
        return AcquireSoundControllerActiveSoundEffectBufferStatus((SoundController*)self, effect);
    }

    BOOL AcquireSoundControllerActiveSoundEffectBufferStatus(SoundController* self, SoundEffect* effect)
    {
        const auto indx = effect->UnknownIndex;

        if (indx < MIN_ACTIVE_SOUND_BUFFER_POOL_INDEX || (MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE - 1) < indx
            || SoundState.SoundUnk0x24Array[indx].Buffer == NULL) { return FALSE; }

        DWORD status = DSBSTATUS_NONE;

        if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->GetStatus(&status),
            "Unable to get active sound effect buffer status.") != DS_OK)
        {
            return FALSE;
        }

        if (status & DSBSTATUS_PLAYING) { return TRUE; }

        return FALSE;
    }

    s32 UnlockActiveSoundControllerSoundSampleBufferWrapper(void* self, const s32 indx, const s32 offset, const s32 length, const SoundSampleDescriptor* desc)
    {
        return UnlockActiveSoundControllerSoundSampleBuffer((SoundController*)self, indx, offset, length, desc);
    }

    s32 UnlockActiveSoundControllerSoundSampleBuffer(SoundController* self, const s32 indx, const s32 offset, const s32 length, const SoundSampleDescriptor* desc)
    {
        if (indx < MIN_SOUND_BUFFER_POOL_INDEX || MAX_SOUND_BUFFER_POOL_SIZE < indx
            || SoundState.SoundBufferPool[indx].Buffer == NULL
            || SoundState.SoundBufferPool[indx].Offset == 0)
        {
            LogError("Unable to unlock sound effect, invalid index %d.", indx);
        }

        if (SoundState.SoundBufferPool[indx].Audio == NULL) { return -1; } // TODO constant

        DSC(SoundState.SoundBufferPool[indx].Buffer->Unlock(
            SoundState.SoundBufferPool[indx].Audio, SoundState.SoundBufferPool[indx].AudioSize, NULL, 0),
            "Unable to unlock sound effect buffer.");

        SoundState.SoundBufferPool[indx].Audio = NULL;

        return 1; // TODO constant
    }

    BOOL StartSoundControllerSoundEffectWrapper(void* self, SoundEffect* effect)
    {
        return StartSoundControllerSoundEffect((SoundController*)self, effect);
    }

    BOOL StartSoundControllerSoundEffect(SoundController* self, SoundEffect* effect)
    {
        const auto indx = effect->UnknownIndex;

        if (indx < MIN_ACTIVE_SOUND_BUFFER_POOL_INDEX || (MAX_ACTIVE_SOUND_BUFFER_POOL_SIZE - 1) < indx
            || SoundState.SoundUnk0x24Array[indx].Buffer == NULL)
        {
            LogError("Unable to start sound effect, invalid index %d.", indx);
        }

        if (!self->Self->SelectSoundEffectOptions(self, effect, U32_MAX)) { return FALSE; } // TODO constant

        if (effect->Sample == NULL)
        {
            LogError("Unable to start sound effect, sound sample is missing.");
        }

        if (1 < effect->Sample->Descriptor.LoopMode || 0 < effect->Sample->Descriptor.Unk6) // TODO constants
        {
            LogError("Unable to start sound effect, exotic jump sequences not allowed for hardware mixed sounds.");
        }

        DWORD options = DSBPLAY_NONE;

        if (effect->Sample->Descriptor.LoopMode == 1 && -1 < effect->Sample->Descriptor.Unk6) // TODO constants
        {
            options = options | DSBPLAY_LOOPING;
        }

        if (effect->Sample->Length != effect->Sample->Descriptor.Definition.Length)
        {
            options = options | DSBPLAY_LOOPING;
        }

        if (effect->Descriptor.Unk30 & 2)// TODO constants
        {
            options = options | DSBPLAY_LOOPING;
        }

        const auto result = SoundState.SoundUnk0x24Array[indx].Buffer->Play(NULL, DSBPRIORITY_NONE, options);

        if (result == DSERR_BADFORMAT)
        {
            if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetFrequency(DSBFREQUENCY_ORIGINAL),
                "Unable to set secondary hardware sound buffer frequency.") != DS_OK)
            {
                return FALSE;
            }

            if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetVolume(DSBVOLUME_MIN),
                "Unable to set secondary hardware sound buffer volume.") != DS_OK)
            {
                return FALSE;
            }

            if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->Play(NULL, DSBPRIORITY_NONE, DSBPLAY_NONE),
                "Unable to play secondary hardware sound buffer.") != DS_OK)
            {
                return FALSE;
            }

            if (SoundState.SoundUnk0x24Array[indx].HZ != -1 &&
                SoundState.SoundUnk0x24Array[indx].HZ != effect->Sample->Descriptor.Definition.HZ) // TODO constants
            {
                const auto hz = AcquireSoundFrequency(effect->Sample->Descriptor.Definition.HZ * effect->Descriptor.HZ);

                if (1 < (u32)(hz - SoundState.SoundUnk0x24Array[indx].HZ)) // TODO constant
                {
                    if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetFrequency(hz),
                        "Unable to set secondary hardware sound buffer frequency.") != DS_OK)
                    {
                        return FALSE;
                    }

                    SoundState.SoundUnk0x24Array[indx].HZ = hz;
                }
            }

            if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetCurrentPosition(0),
                "Unable to set secondary hardware sound buffer playback position.") != DS_OK)
            {
                return FALSE;
            }

            if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetVolume(SoundState.SoundUnk0x24Array[indx].Volume),
                "Unable to set secondary hardware sound buffer volume.") != DS_OK)
            {
                return FALSE;
            }
        }
        else if (DSC(result, "Unable to play secondary hardware sound buffer.") != DS_OK)
        {
            return FALSE;
        }

        effect->Options = effect->Options & 0xBFFFFFFF; // TODO constants

        return TRUE;
    }

    BOOL SelectSoundControllerSoundEffectOptionsWrapper(void* self, SoundEffect* effect, const u32 mode)
    {
        return SelectSoundControllerSoundEffectOptions((SoundController*)self, effect, mode);
    }

    BOOL SelectSoundControllerSoundEffectOptions(SoundController* self, SoundEffect* effect, const u32 mode)
    {
        const auto indx = effect->UnknownIndex;

        if (indx < 1 || 30 < indx || SoundState.SoundUnk0x24Array[indx].Buffer == NULL)
        {
            LogError("Unable to select sound effect options, invalid index %d.", indx);
        }

        if (SoundState.SoundUnk0x24Array[indx].Buffer == NULL) { return FALSE; }

        auto result = TRUE;

        if (SoundState.SoundUnk0x24Array[indx].AdvancedBuffer != NULL)
        {
            if (mode == U32_MAX) // TODO constants
            {
                auto options = DS3DMODE_NORMAL;

                if (effect->Descriptor.Unk30 & 1) // TODO constants
                {
                    options = DS3DMODE_DISABLE;
                }

                if (DSC(SoundState.SoundUnk0x24Array[indx].AdvancedBuffer->SetMode(options, DS3D_DEFERRED),
                    "Unable to set sound buffer into spatial mode.") != DS_OK)
                {
                    result = FALSE;
                }
            }

            auto volume = 0.0f;
            auto minimumDistance = 0.0f;
            auto maximumDistance = 0.0f;

            if (mode & 0x88) // TODO constants
            {
                minimumDistance = effect->Descriptor.MinimumDistance;
                maximumDistance = effect->Descriptor.MaximumDistance;

                auto value = AcquireSoundEffectChannelVolume(effect->Descriptor.NextChannelIndex);

                volume = value;

                if ((effect->Descriptor.Unk30 & 1) == 0 && 0.0f < value) // TODO constants
                {
                    volume = 1.0f; // TODO constants
                    minimumDistance = minimumDistance * value;
                    maximumDistance = maximumDistance * value;
                }
            }

            if (mode & 2) // TODO constants
            {
                if (DSC(SoundState.SoundUnk0x24Array[indx].AdvancedBuffer->SetPosition(
                effect->Descriptor.Location.X, effect->Descriptor.Location.Y, effect->Descriptor.Location.Z, TRUE),
                    "Unable to set secondary sound hardware buffer position.") != DS_OK)
                {
                    result = FALSE;
                }
            }

            if (mode & 4) // TODO constants
            {
                if (DSC(SoundState.SoundUnk0x24Array[indx].AdvancedBuffer->SetVelocity(
                    effect->Descriptor.Velocity.X, effect->Descriptor.Velocity.Y, effect->Descriptor.Velocity.Z, TRUE),
                    "Unable to set secondary sound hardware buffer velocity.") != DS_OK)
                {
                    result = FALSE;
                }
            }

            if (mode & 8) // TODO constants
            {
                auto value = AcquireUnknownSoundValue102(effect->Descriptor.Volume * volume);

                auto v1 = value - SoundState.SoundUnk0x24Array[indx].Volume;
                auto v2 = v1 >> 0x1f;// TODO

                if (mode == U32_MAX || 1 < ((v1 ^ v2) - v2)) // TODO
                {
                    if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetVolume(value),
                        "Unable to set secondary sound hardware buffer volume.") != DS_OK)
                    {
                        result = FALSE;
                    }
                }

                SoundState.SoundUnk0x24Array[indx].Volume = value;
            }

            if (mode & 0x10) // TODO constants
            {
                auto value = AcquireSoundFrequency(effect->Sample->Descriptor.Definition.HZ * effect->HZ);

                auto v1 = value - SoundState.SoundUnk0x24Array[indx].HZ;
                auto v2 = v1 >> 0x1f;// TODO

                if (mode == U32_MAX || 1 < ((v1 ^ v2) - v2)) // TODO
                {
                    if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetFrequency(value),
                        "Unable to set secondary sound hardware buffer frequency.") != DS_OK)
                    {
                        result = FALSE;
                    }
                }

                SoundState.SoundUnk0x24Array[indx].HZ = value;
            }

            if (mode & 0x80) // TODO constants
            {
                if (DSC(SoundState.SoundUnk0x24Array[indx].AdvancedBuffer->SetMinDistance(minimumDistance, TRUE),
                    "Unable to set secondary sound hardware buffer minimum distance.") != DS_OK)
                {
                    result = FALSE;
                }

                if (DSC(SoundState.SoundUnk0x24Array[indx].AdvancedBuffer->SetMaxDistance(maximumDistance, TRUE),
                    "Unable to set secondary sound hardware buffer maximum distance.") != DS_OK)
                {
                    result = FALSE;
                }
            }

            if (mode & 0x100) // TODO constants
            {
                if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetCurrentPosition((u32)roundf(effect->Position)),
                    "Unable to set secondary sound hardware buffer playback position.") != DS_OK) // TODO constant
                {
                    result = FALSE;
                }
            }

            effect->Options = effect->Options & (~mode | 0x40000000);// TODO

            if (!result)
            {
                LogMessage("[ERROR] [SOUND] Unable to set secondary sound hardware buffer options.\n");
            }

            return result;
        }

        if (mode & 0xca) // TODO constants
        {
            auto maximum = F32_MIN;
            const auto count = AcquireSoundChannelCount();

            for (u32 x = 0; x < count; x++) // TODO constants
            {
                if (maximum < effect->Pans[x])
                {
                    maximum = effect->Pans[x];
                }
            }

            // The value in lPan is measured in hundredths of a decibel(dB), in the range of DSBPAN_LEFT to DSBPAN_RIGHT.
            // These values are currently defined in Dsound.h as - 10, 000 and 10, 000 respectively.
            // The value DSBPAN_LEFT means the right channel is attenuated by 100 dB.
            // The value DSBPAN_RIGHT means the left channel is attenuated by 100 dB.
            // The neutral value is DSBPAN_CENTER, defined as zero.This value of 0 in the lplPan parameter'
            // means that both channels are at full volume(they are attenuated by 0 decibels).
            // At any setting other than DSBPAN_CENTER, one of the channels is at full volume and
            // the other is attenuated.

            auto val = DSBPAN_CENTER;

            if (count == 2 && 0.0f < maximum)
            {
                if (effect->Pans[1] <= effect->Pans[0])
                {
                    if (effect->Pans[0] < effect->Pans[1])
                    {
                        if (0.0f < effect->Pans[1])
                        {
                            val = (s32)roundf(AcquireUnknownSoundValue101(-868.589f * log2f(effect->Pans[0] / effect->Pans[1]))); // TODO constants

                            if (val < DSBPAN_CENTER || DSBPAN_LEFT < val)
                            {
                                val = DSBPAN_RIGHT;
                            }
                        }
                        else
                        {
                            val = DSBPAN_LEFT;
                        }
                    }
                }
                else if (effect->Pans[0] <= 0.0f)
                {
                    val = DSBPAN_RIGHT;
                }
                else
                {
                    val = (s32)roundf(AcquireUnknownSoundValue101(868.589f * log2f(effect->Pans[1] / effect->Pans[0]))); // TODO constants

                    if (val < DSBPAN_RIGHT || DSBPAN_CENTER < val)
                    {
                        val = DSBPAN_CENTER;
                    }
                }
            }

            {
                const auto value = AcquireUnknownSoundValue102(maximum);

                auto v1 = value - SoundState.SoundUnk0x24Array[indx].Volume;

                auto v2 = v1 >> 0x1f;// TODO

                if (mode == U32_MAX || 1 < ((v1 ^ v2) - v2)) // TODO
                {
                    if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetVolume(value),
                        "Unable to set secondary sound hardware buffer volume.") != DS_OK)
                    {
                        result = FALSE;
                    }
                }

                SoundState.SoundUnk0x24Array[indx].Volume = value;
            }

            {
                auto v1 = val - SoundState.SoundUnk0x24Array[indx].Pan;

                auto v2 = v1 >> 0x1f;// TODO

                if (mode == U32_MAX || 1 < ((v1 ^ v2) - v2)) // TODO
                {
                    if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetPan(val),
                        "Unable to set secondary sound hardware buffer pan.") != DS_OK)
                    {
                        result = FALSE;
                    }
                }

                SoundState.SoundUnk0x24Array[indx].Pan = val;
            }
        }

        if (mode & 0x96)//TODO constant
        {
            auto value = AcquireSoundFrequency(effect->Sample->Descriptor.Definition.HZ * effect->HZ);

            auto v1 = value - SoundState.SoundUnk0x24Array[indx].HZ;
            auto v2 = v1 >> 0x1f;// TODO

            if (mode != U32_MAX || 1 < ((v1 ^ v2) - v2)) // TODO
            {
                if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetFrequency(value),
                    "Unable to set secondary sound hardware buffer frequency.") != DS_OK)
                {
                    result = FALSE;
                }
            }

            SoundState.SoundUnk0x24Array[indx].HZ = value;
        }

        if (mode & 0x100)//TODO constant
        {
            if (DSC(SoundState.SoundUnk0x24Array[indx].Buffer->SetCurrentPosition((u32)roundf(effect->Position)),
                "Unable to set secondary sound hardware buffer playback position.") != DS_OK) // TODO constant
            {
                result = FALSE;
            }
        }

        effect->Options = effect->Options & (~mode | 0x40000000);// TODO

        if (!result)
        {
            LogMessage("[ERROR] [SOUND] Unable to set secondary sound hardware buffer options.\n");
        }

        return result;
    }

    s32 UnknownSoundControllerValueWrapper(void* self, const u32 indx)
    {
        return UnknownSoundControllerValue((SoundController*)self, indx);
    }

    s32 UnknownSoundControllerValue(SoundController* self, const u32 indx)
    {
        return -1;//TODO Constant
    }

    // TODO 2 more params!
    void SelectSoundControllerEnvironmentalAudioExtensionsEnvironmentWrapper(void* self, const s32 indx, const f32 volume, const f32 time) // TODO dumping, ...
    {
        SelectSoundControllerEnvironmentalAudioExtensionsEnvironment((SoundController*)self, indx, volume, time);
    }

    // TODO enum for indx?
    // TODO 2 more params!
    void SelectSoundControllerEnvironmentalAudioExtensionsEnvironment(SoundController* self, const s32 indx, const f32 volume, const f32 time) // TODO dumping, ...
    {
        if (indx < 0 || 26 < indx || SoundState.EAX.Instance == NULL) { return; } // TODO constants

        SoundState.EAX.Index = indx;

        auto value = &Sounds::EAX::Environments[indx];

        if (SoundState.EAX.Instance->Set(DSPROPSETID_EAX20_ListenerProperties,
            DSPROPERTY_EAX_LISTENERPROPERTY::DSPROPERTY_EAXLISTENER_ALLPARAMETERS,
            NULL, 0, (void**)&value, sizeof(EAXLISTENERPROPERTIES)) != DS_OK)
        {
            return;
        }

        LogMessage("[SOUND] Environmental Audio Extensions environment is set to %d.\n", value->dwEnvironment);
    }

    void* LockActiveSoundControllerSoundSampleBufferWrapper(void* self, const s32 indx, const s32 offset, const s32 length, const SoundSampleDescriptor* desc)
    {
        return LockActiveSoundControllerSoundSampleBuffer((SoundController*)self, indx, offset, length, desc);
    }

    void* LockActiveSoundControllerSoundSampleBuffer(SoundController* self, const s32 indx, const s32 offset, const s32 length, const SoundSampleDescriptor* desc)
    {
        if (indx < MIN_SOUND_BUFFER_POOL_INDEX || MAX_SOUND_BUFFER_POOL_SIZE < indx
            || SoundState.SoundBufferPool[indx].Buffer == NULL
            || SoundState.SoundBufferPool[indx].Offset == 0)
        {
            return NULL;
        }

        if (SoundState.SoundBufferPool[indx].Audio != NULL)
        {
            LogError("Unable to lock sound sample, it is already locked.");
        }

        auto offsetValue = offset * SoundState.SoundBufferPool[indx].Offset;
        auto offsetMask = offsetValue >> 0x1f;// TODO
        offsetValue = (offsetValue + offsetMask * (-0x100)) - (offsetMask << 7 < 0); // TODO

        auto lenOffset = (((offset >> 24) << 16) | (offset >> 8)) & 0xffffffe; // TODO
        auto len = (length + 0x1fU) & 0xffffffe0; // TODO
        auto lenVal = SoundState.SoundBufferPool[indx].Length - lenOffset;// TODO

        if (lenVal < len) { len = lenVal; }

        if (DSC(SoundState.SoundBufferPool[indx].Buffer->Lock(
            lenOffset, len, &SoundState.SoundBufferPool[indx].Audio,
            (LPDWORD)&SoundState.SoundBufferPool[indx].AudioSize, NULL, NULL, DSBLOCK_NONE),
            "Unable to lock hardware sound buffer.") != DS_OK)
        {
            return NULL;
        }
        
        if ((SoundState.SoundBufferPool[indx].Length + -0x20) <= (len + lenOffset)) // TODO
        {
            auto address = ((addr)SoundState.SoundBufferPool[indx].Audio
                + ((addr)SoundState.SoundBufferPool[indx].Length - lenOffset) + -0x20); // TODO

            ZeroMemory((void*)address, 0x20); // TODO
        }

        return (void*)((addr)SoundState.SoundBufferPool[indx].Audio + ((offsetValue >> 8) - lenOffset)); // TODO
    }
    
    BOOL InitializeSecondarySoundBuffer1(void)
    {
        if (SoundState.DirectSound.Buffers.Secondary.Buffer == NULL) { return FALSE; }

        void* bufferData1 = NULL;
        DWORD bufferData1Length = 0;
        void* bufferData2 = NULL;
        DWORD bufferData2Length = 0;

        if (DSC(SoundState.DirectSound.Buffers.Secondary.Buffer->Lock(
            SoundState.DirectSound.Buffers.Secondary.Unknown2 * SoundState.DirectSound.Buffers.Secondary.ChannelBufferSize,
            SoundState.DirectSound.Buffers.Secondary.ChannelBufferSize,
            &bufferData1, &bufferData1Length, &bufferData2, &bufferData2Length, DSBLOCK_NONE),
            "Unable to lock secondary sound buffer.") != DS_OK)
        {
            return FALSE;
        }

        if (bufferData2Length == NULL
            && bufferData1Length == SoundState.DirectSound.Buffers.Secondary.ChannelBufferSize)
        {
            auto bps = SoundState.DirectSound.Buffers.Primary.BitsPerSample >> 3;

            u32 values[8];
            u32 total = 0;

            for (u32 x = 0; x < SoundState.DirectSound.Buffers.Primary.Channels; x++)
            {
                values[x] = bps;
                total = total + bps;
            }

            FUN_00579ad0(values,
                SoundState.DirectSound.Buffers.Primary.BitsPerSample,
                SoundState.DirectSound.Buffers.Primary.Channels,
                SoundState.DirectSound.Buffers.Primary.HZ,
                SoundState.DirectSound.Buffers.Secondary.Count,
                SoundState.DirectSound.Buffers.Primary.Channels * bps);

            SoundState.DirectSound.Buffers.Secondary.Unknown2 = SoundState.DirectSound.Buffers.Secondary.Unknown2 + 1;

            if (SoundState.DirectSound.Buffers.Secondary.Unknown1 <= SoundState.DirectSound.Buffers.Secondary.Unknown2)
            {
                SoundState.DirectSound.Buffers.Secondary.Unknown2 = 0;
            }

            if (DSC(SoundState.DirectSound.Buffers.Secondary.Buffer->Unlock(
                bufferData1, bufferData1Length, bufferData2, bufferData2Length),
                "Unable to unlock secondary sound buffer.") != DS_OK)
            {
                return FALSE;
            }

            return TRUE;
        }

        return FALSE;
    }
}