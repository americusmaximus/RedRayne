#pragma once

#include "Basic.hxx"
#include "Sounds.Basic.hxx"

#define INVALID_SOUND_DEVICE_COUNT (-1)
#define INVALID_SOUND_DEVICE_INDEX (-1)

#define MIN_SOUND_DEVICE_INDEX 0

#define MIN_SOUND_DEVICE_COUNT 0
#define MAX_SOUND_DEVICE_COUNT 8

#define MAX_SOUND_RECORDING_DEVICE_COUNT 6
#define MAX_SOUND_ENUMERATED_DEVICE_COUNT 12

#define MAX_SOUND_DEVICE_NAME_LENGTH 256

namespace Sounds
{
    struct SoundDeviceDetail
    {
        BOOL IsPrimary;
        GUID UID;
        char Name[MAX_SOUND_DEVICE_NAME_LENGTH];
        BOOL IsEmulated;
        SoundMixMode MixMode;
    };

    struct SoundDevice
    {
        char Name[MAX_SOUND_DEVICE_NAME_LENGTH];
        s32 Unk1;// TODO
        s32 Index;
        SoundMixMode MixMode;
        s32 Unk2;// TODO
        BOOL IsPrimary;
        BOOL IsEmulated;
    };

    struct SoundRecordingDevice
    {
        char Name[MAX_SOUND_DEVICE_NAME_LENGTH];
        s32 Unk1;// TODO
        u32 Index;
    };

    struct SoundDeviceContainer
    {
        s32 SoundDeviceCount = INVALID_SOUND_DEVICE_COUNT;// (s32*)0x006254f0
        s32 SoundDeviceIndex = INVALID_SOUND_DEVICE_INDEX;// (s32*)0x006254f4
        SoundDevice* _SoundDevices = (SoundDevice*)0x008d7c34;// TODO  [MAX_SOUND_DEVICE_COUNT]

        s32 SoundRecordingDeviceCount = INVALID_SOUND_DEVICE_COUNT;// (s32*)0x0062551c
        s32 SoundRecordingDeviceIndex = INVALID_SOUND_DEVICE_INDEX;//(s32*)0x00625520
        SoundRecordingDevice SoundRecordingDevices[MAX_SOUND_RECORDING_DEVICE_COUNT];// = (SoundRecordingDevice*)0x008f863c;// todo  [MAX_SOUND_RECORDING_DEVICE_COUNT]

        s32 EnumeratedDeviceCount = INVALID_SOUND_DEVICE_COUNT;//(s32*)0x00625e08
        SoundDeviceDetail EnumeratedDevices[MAX_SOUND_ENUMERATED_DEVICE_COUNT];// = (SoundDeviceDetail*)0x008f8e8c; //todo [MAX_SOUND_ENUMERATED_DEVICE_COUNT]
    };

    extern SoundDeviceContainer SoundDeviceState;

    BOOL CALLBACK EnumerateSoundDevicesCallback(const LPGUID guid, const LPCSTR description, const LPCSTR module, const LPVOID context);
    BOOL EnumerateSoundDevices(const s32 indx, SoundDevice* device);
    BOOL SelectSoundDeviceMixMode(void);
    s32 AcquireSoundDeviceCount(void);
    s32 AcquireSoundDeviceIndex(void);
    s32 AcquireSoundRecordingDeviceCount(void);
    s32 AcquireSoundRecordingDeviceIndex(void);
    s32 SelectSoundDevice1(void);
    s32 SelectSoundRecordingDevice1(void);
    void AcquireSoundDevice(const s32 indx, SoundDevice* device);
    void AcquireSoundRecordingDevice(s32 indx, SoundRecordingDevice* device);
    void SelectSoundDevice2(const s32 indx);
    void SelectSoundDeviceOutputOptions(void);
    void SelectSoundRecordingDevice2(const s32 indx);
}