#include "Sounds.Basic.hxx"
#include "Sounds.hxx"
#include "Sounds.Devices.hxx"
#include "Logger.hxx"

#include <stdio.h>

using namespace Logger;

namespace Sounds
{
    SoundDeviceContainer SoundDeviceState;

    BOOL CALLBACK EnumerateSoundDevicesCallback(const LPGUID guid, const LPCSTR description, const LPCSTR module, const LPVOID context)
    {
        if (MAX_SOUND_ENUMERATED_DEVICE_COUNT < SoundDeviceState.EnumeratedDeviceCount) { return FALSE; }

        IDirectSound* instance;

        if (DSC(DirectSoundCreate(guid, &instance, NULL), "Unable to initialize Direct Sound.") != DS_OK) { return TRUE; }

        if (instance == NULL) { return TRUE; }

        DSCAPS caps = { .dwSize = sizeof(DSCAPS) };

        DSC(instance->GetCaps(&caps), "Unable to query sound device capabilities.");

        instance->Release();

        if (SoundDeviceState.EnumeratedDeviceCount == 0)
        {
            auto device = &SoundDeviceState.EnumeratedDevices[SoundDeviceState.EnumeratedDeviceCount];

            ZeroMemory(device, sizeof(SoundDeviceDetail));

            if (guid == NULL)
            {
                device->IsPrimary = TRUE;
            }
            else
            {
                device->IsPrimary = FALSE;
                device->UID = *guid;
            }

            device->MixMode = SoundMixMode::None;

            if (caps.dwFlags & DSCAPS_EMULDRIVER)
            {
                device->IsEmulated = TRUE;
            }
            else
            {
                device->IsEmulated = FALSE;
                device->MixMode = SoundMixMode::Simple;

                if (caps.dwFreeHwMixingStaticBuffers != 0)
                {
                    device->MixMode = SoundMixMode::Advanced;
                }

                LogMessage(caps.dwFreeHw3DStaticBuffers == 0
                    ? "No onboard RAM detected. Forcing sound samples to load in system memory.\n"
                    : "Onboard RAM detected. Sound samples will be loaded into available onboard memory.\n");
            }

            strncpy(device->Name, description, MAX_SOUND_DEVICE_NAME_LENGTH - 1);

            SoundDeviceState.EnumeratedDeviceCount = SoundDeviceState.EnumeratedDeviceCount + 1;
        }

        return TRUE;
    }

    BOOL EnumerateSoundDevices(const s32 indx, SoundDevice* device)
    {
        if (SoundDeviceState.EnumeratedDeviceCount < MIN_SOUND_DEVICE_COUNT)
        {
            SoundDeviceState.EnumeratedDeviceCount = MIN_SOUND_DEVICE_COUNT;

            DirectSoundEnumerateA(EnumerateSoundDevicesCallback, NULL);
        }

        if (INVALID_SOUND_DEVICE_COUNT < indx && indx < SoundDeviceState.EnumeratedDeviceCount)
        {
            const auto endev = &SoundDeviceState.EnumeratedDevices[indx];

            device->Index = indx;
            device->Unk1 = 1; // TODO constant
            device->Unk2 = 0; // TODO constant
            device->MixMode = endev->MixMode;
            device->IsEmulated = endev->IsEmulated;
            device->IsPrimary = endev->IsPrimary;

            sprintf(device->Name, "Direct Sound: %s", endev->Name);

            return TRUE;
        }

        return FALSE;
    }

    s32 AcquireSoundDeviceCount(void)
    {
        if (INVALID_SOUND_DEVICE_COUNT < SoundDeviceState.SoundDeviceCount) { return SoundDeviceState.SoundDeviceCount; }

        LockSounds();
        UnlockSound1();

        SoundDeviceState.SoundDeviceCount = MIN_SOUND_DEVICE_COUNT;

        for (u32 x = MIN_SOUND_DEVICE_COUNT; x < MAX_SOUND_DEVICE_COUNT; x++)
        {
            if (!EnumerateSoundDevices(x, &SoundDeviceState._SoundDevices[x]))
            {
                return SoundDeviceState.SoundDeviceCount;
            }

            SoundDeviceState.SoundDeviceCount = SoundDeviceState.SoundDeviceCount + 1;
        }

        return SoundDeviceState.SoundDeviceCount;
    }

    void AcquireSoundDevice(const s32 indx, SoundDevice* device)
    {
        if (indx < MIN_SOUND_DEVICE_COUNT || AcquireSoundDeviceCount() <= indx)
        {
            LogError("Unable to get sound device. Provided index is invalid.");
        }

        CopyMemory(device, &SoundDeviceState._SoundDevices[indx], sizeof(SoundDevice));
    }

    s32 SelectSoundDevice1(void)
    {
        const auto count = AcquireSoundDeviceCount();

        if (count < 1) { return INVALID_SOUND_DEVICE_INDEX; }

        for (u32 x = MIN_SOUND_DEVICE_COUNT; x < count; x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (device.MixMode == SoundMixMode::Advanced) { return x; }
        }

        for (u32 x = MIN_SOUND_DEVICE_COUNT; x < count; x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (device.MixMode == SoundMixMode::Simple) { return x; }
        }

        for (u32 x = MIN_SOUND_DEVICE_COUNT; x < count; x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (device.Unk1 == 1 && device.IsPrimary && !device.IsEmulated) { return x; } // TODO constant
        }

        for (u32 x = MIN_SOUND_DEVICE_COUNT; x < count; x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (device.Unk1 == 1 && !device.IsEmulated) { return x; } // TODO constant
        }

        for (u32 x = MIN_SOUND_DEVICE_COUNT; x < count; x++)
        {
            SoundDevice device;
            AcquireSoundDevice(x, &device);

            if (device.Unk1 == 0) { return x; } // TODO constant
        }

        return MIN_SOUND_DEVICE_INDEX;
    }

    void SelectSoundDevice2(const s32 indx)
    {
        if (AcquireSoundAvailableMode())
        {
            LogError("Unable to select sound device. Sound device is already active.");
        }

        const auto di = indx < MIN_SOUND_DEVICE_COUNT ? SoundDeviceState.SoundDeviceIndex : indx;
        const auto count = AcquireSoundDeviceCount();

        if (INVALID_SOUND_DEVICE_INDEX < di && di < count)
        {
            SoundDeviceState.SoundDeviceIndex = di;

            return;
        }
        
        SoundDeviceState.SoundDeviceIndex = SelectSoundDevice1();
    }

    s32 AcquireSoundRecordingDeviceCount(void)
    {
        if (INVALID_SOUND_DEVICE_INDEX < SoundDeviceState.SoundRecordingDeviceCount) { return SoundDeviceState.SoundRecordingDeviceCount; }

        LockSounds();
        UnlockSound1();

        SoundDeviceState.SoundRecordingDeviceCount = MIN_SOUND_DEVICE_COUNT;

        return SoundDeviceState.SoundRecordingDeviceCount;
    }

    void AcquireSoundRecordingDevice(s32 indx, SoundRecordingDevice* device)
    {
        if (indx < MIN_SOUND_DEVICE_COUNT || AcquireSoundRecordingDeviceCount() <= indx)
        {
            LogError("Unable to get sound recording device. Invalid index %d.", indx);
        }

        CopyMemory(device, &SoundDeviceState.SoundRecordingDevices[indx], sizeof(SoundRecordingDevice));
    }

    s32 SelectSoundRecordingDevice1(void)
    {
        const auto count = AcquireSoundRecordingDeviceCount();

        if (count < 1) { return INVALID_SOUND_DEVICE_INDEX; }

        for (u32 x = MIN_SOUND_DEVICE_COUNT; x < count; x++)
        {
            SoundRecordingDevice device;
            AcquireSoundRecordingDevice(x, &device);

            if (device.Unk1 == 1) { return x; } // TODO constant
        }

        for (u32 x = MIN_SOUND_DEVICE_COUNT; x < count; x++)
        {
            SoundRecordingDevice device;
            AcquireSoundRecordingDevice(x, &device);

            if (device.Unk1 == 0) { return x; } // TODO constant
        }

        return MIN_SOUND_DEVICE_INDEX;
    }

    void SelectSoundRecordingDevice2(const s32 indx)
    {
        if (IsSoundRecordingPresent())
        {
            LogError("Unable to select sound recording device. Sound recording device is already active.");
        }

        const auto di = indx < MIN_SOUND_DEVICE_INDEX ? SoundDeviceState.SoundRecordingDeviceIndex : indx;
        const auto count = AcquireSoundRecordingDeviceCount();

        if (INVALID_SOUND_DEVICE_INDEX < di && di < count)
        {
            SoundDeviceState.SoundRecordingDeviceIndex = di;

            return;
        }

        SoundDeviceState.SoundRecordingDeviceIndex = SelectSoundRecordingDevice1();
    }

    s32 AcquireSoundRecordingDeviceIndex(void)
    {
        return SoundDeviceState.SoundRecordingDeviceIndex;
    }

    BOOL SelectSoundDeviceMixMode(void)
    {
        if (!StopSound2()) { return FALSE; }

        SelectSoundDevice2(SoundDeviceState.SoundDeviceIndex);

        if (INVALID_SOUND_DEVICE_INDEX < SoundDeviceState.SoundDeviceIndex)
        {
            if (SoundDeviceState._SoundDevices[SoundDeviceState.SoundDeviceIndex].Unk1 == 1) // TODO constant
            {
                *SoundState._SoundControllerPointer = InitializeSoundController1(SoundDeviceState._SoundDevices[SoundDeviceState.SoundDeviceIndex].Index);
            }

            if (*SoundState._SoundControllerPointer != NULL)
            {
                SoundState.DirectSound.MixMode = (*SoundState._SoundControllerPointer)->Self->QueryCapabilities(*SoundState._SoundControllerPointer);

                return TRUE;
            }
        }

        return FALSE;
    }

    s32 AcquireSoundDeviceIndex(void) { return SoundDeviceState.SoundDeviceIndex; }

    void SelectSoundDeviceOutputOptions(void)
    {
        SoundDevice device;

        AcquireSoundDevice(AcquireSoundDeviceIndex(), &device);

        if (device.MixMode == SoundMixMode::None) { return; }

        if (AcquireSoundMixMode2() == SoundMixMode::None)
        {
            SelectSoundMixMode(SoundMixMode::Simple);
        }

        SelectSoundOutputOptions(16, 2, 44100); // TODO constants
    }
}