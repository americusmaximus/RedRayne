#include "Assets.Sounds.hxx"
#include "Sounds.hxx"
#include "IO.Streams.hxx"
#include "Logger.hxx"
#include "Memory.hxx"
#include "Native.hxx"

#define FILE_ATTRIBUTE_NONE 0

using namespace Native;
using namespace Sounds;
using namespace Memory;
using namespace Logger;
using namespace IO;
using namespace IO::Streams;

namespace Assets::Sounds
{
    SoundAssetContainer SoundAssetState;

    void InitializeSoundRecordDescriptorCollection(const char* file)
    {
        InStreamFile stream;

        InStreamFileInitialize(&stream);

        if (!InStreamFileOwn(&stream, SOUND_DIRECTORY_NAME, file, FALSE))
        {
            LogMessage("[ERROR] [SOUND] Unable to open sound sample collection %s.\n", file);

            InStreamFileDispose(&stream, DisposeMode::None);

            return;
        }

        u32 version = 0;

        if (AbstractInStreamScan((AbstractInStream*)&stream, "%d\n", &version) != 1)
        {
            LogError("Sound sample collection %s is corrupted.", file);
        }

        if (version != SOUND_RECORD_COLLECTION_VERSION)
        {
            LogMessage("[ERROR] [SOUND] Sound sample collection %s version %d is not supported.\n", file, version);

            InStreamFileDispose(&stream, DisposeMode::None);

            return;
        }

        u32 count = 0;

        if (AbstractInStreamScan((AbstractInStream*)&stream, "%d\n", &count) != 1)
        {
            LogError("Sound sample collection %s is corrupted.", file);
        }

        LogMessage("[SOUND] Loading sound sample collection %s with %d sound samples.\n", file, count);

        if (count < 1)
        {
            InStreamFileDispose(&stream, DisposeMode::None);

            return;
        }

        SoundState.Records.Cache.Items = (SoundSampleRecordDescriptor*)ReallocateMemory(SoundState.Records.Cache.Items,
            (SoundState.Records.Cache.Count + count) * sizeof(SoundSampleRecordDescriptor));

        if (SoundState.Records.Cache.Items == NULL)
        {
            LogError("Unable to allocate memory for sound record collection.");
        }

        for (u32 x = 0; x < count; x++)
        {
            SoundSampleDescriptor desc;

            SoundSampleDescriptorInitialize1(&desc);

            if (AbstractInStreamScan((AbstractInStream*)&stream, "\"%[^\"]\"\n",
                &desc.Definition.Name) != 1)
            {
                LogError("Sound sample collection %s is corrupted.", file);
            }

            s32 todo1 = 0; // TODO

            if (AbstractInStreamScan((AbstractInStream*)&stream,
                "%d , %d , %d , %d , %d , %d , %d , %d, %d\n",
                &desc.Definition.Type, &desc.Definition.BitsPerSample, &desc.Definition.Channels,
                &desc.Definition.HZ, &desc.Definition.Length,
                &todo1, &desc.CacheControl, &desc.Offset, &desc.Priority) != 9)
            {
                LogError("Sound sample collection %s is corrupted.", file);
            }

            desc.LoopMode = 1; // TODO
            desc.ChannelLength[0] = desc.Definition.Length;
            desc.Unk6 = (todo1 != 0) - 1; // TODO

            if (AbstractInStreamScan((AbstractInStream*)&stream, "%f , %f , %f\n",
                &desc.ReferenceDistance, &desc.MinimumDistance, &desc.MaximumDistance) != 3)
            {
                LogError("Sound sample collection %s is corrupted.", file);
            }

            u32 size = 0;
            u32 packet = 0;

            AcquireSoundSampleDescriptorDetails(&desc, &size, &packet);

            if (desc.Definition.Length % size != 0)
            {
                LogMessage("[WARNING] [SOUND] Sound sample %s has length %d which is not a multiple of packet size %d.\n",
                    desc.Definition.Name, desc.Definition.Length, size);
            }

            InitializeSoundRecordDescriptor(&SoundState.Records.Cache.Items[SoundState.Records.Cache.Count], &desc);

            SoundState.Records.Cache.Count = SoundState.Records.Cache.Count + 1;
        }

        LogMessage("[SOUND] Sound sample collection %s loaded, %d sound samples available.\n", file, SoundState.Records.Cache.Count);

        InStreamFileDispose(&stream, DisposeMode::None);
    }

    void ReleaseSoundRecordDescriptorCollection(void)
    {
        if (SoundState.Records.Cache.Items != NULL)
        {
            ReleaseMemory1(SoundState.Records.Cache.Items);

            SoundState.Records.Cache.Items = NULL;
        }

        SoundState.Records.Cache.Count = 0;
    }

    void InitializeSoundAssets(void)
    {
        SoundAssetState.Mutex = InitializeMutex();

        if (SoundAssetState.Mutex == NULL)
        {
            LogError("Unable to acquire sound mutex.");
        }

        SoundAssetState.State = SoundAssetThreadState::Initialization;

        SoundAssetState.Thread = InitializeThread(HandleSoundAssets, NULL);

        while (SoundAssetState.State == SoundAssetThreadState::Initialization)
        {
            Wait(0.01); // TODO Constant
        }
    }

    DWORD WINAPI HandleSoundAssets(LPVOID)
    {
        if (SoundAssetState.State == SoundAssetThreadState::Initialization)
        {
            SoundAssetState.State = SoundAssetThreadState::Active;
        }

        while (TRUE)
        {
            WaitMutex(SoundAssetState.Mutex);

            if (SoundAssetState.State == SoundAssetThreadState::None)
            {
                ReleaseMutex1(SoundAssetState.Mutex);

                SoundAssetState.State = SoundAssetThreadState::None;

                return EXIT_SUCCESS;
            }

            SoundAssetOperation* item = NULL;

            for (u32 x = MIN_SOUND_ASSET_OPERATION_INDEX; x < MAX_SOUND_ASSET_OPERATION_COUNT; x++)
            {
                if ((MAX_SOUND_ASSET_OPERATION_COUNT - 1) < x) { x = MIN_SOUND_ASSET_OPERATION_INDEX; }

                if (SoundAssetState.Operations[x].State == SoundAssetOperationState::Pending)
                {
                    item = &SoundAssetState.Operations[x];

                    break;
                }
            }

            if (item == NULL)
            {
                ReleaseMutex1(SoundAssetState.Mutex);

                Wait(0.02); // TODO Constant

                continue;
            }

            item->State = SoundAssetOperationState::Processing;

            ReleaseMutex1(SoundAssetState.Mutex);

            auto file = CreateFileA(item->Name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NONE, NULL);

            if (file == INVALID_HANDLE_VALUE)
            {
                WaitMutex(SoundAssetState.Mutex);

                item->State = SoundAssetOperationState::Available;

                ReleaseMutex1(SoundAssetState.Mutex);
            }
            else if (item->Offset == 0 || SetFilePointer(file, item->Offset, NULL, FILE_BEGIN) == item->Offset)
            {
                DWORD read = 0;

                const auto result = ReadFile(file, item->Data, item->Length, &read, NULL);

                CloseHandle(file);

                WaitMutex(SoundAssetState.Mutex);

                if (!result || read != item->Length)
                {
                    item->State = SoundAssetOperationState::Available;

                    ReleaseMutex1(SoundAssetState.Mutex);
                }
                else
                {
                    item->State = SoundAssetOperationState::Completed;

                    ReleaseMutex1(SoundAssetState.Mutex);
                }
            }
            else
            {
                CloseHandle(file);

                WaitMutex(SoundAssetState.Mutex);

                item->State = SoundAssetOperationState::Available;

                ReleaseMutex1(SoundAssetState.Mutex);
            }
        }
    }

    void AwaitSoundAssetAvailability(const s32 indx)
    {
        if (INVALID_SOUND_ASSET_OPERATION_INDEX < indx && indx < MAX_SOUND_ASSET_OPERATION_COUNT)
        {
            while (TRUE)
            {
                WaitMutex(SoundAssetState.Mutex);

                if (SoundAssetState.Operations[indx].State != SoundAssetOperationState::Processing) { break; }

                ReleaseMutex1(SoundAssetState.Mutex);

                Wait(0.01); // TODO constant
            }

            SoundAssetState.Operations[indx].State = SoundAssetOperationState::None;

            ReleaseMutex1(SoundAssetState.Mutex);
        }
    }
    
    s32 InitializeSoundAssetOperation(FileDescriptor* desc, const s32 offset, const s32 length, void* data)
    {
        if (desc->Size < offset + length) { return INVALID_SOUND_ASSET_OPERATION_INDEX; }

        WaitMutex(SoundAssetState.Mutex);

        auto indx = MIN_SOUND_ASSET_OPERATION_INDEX;

        for (u32 x = MIN_SOUND_ASSET_OPERATION_INDEX; x < MAX_SOUND_ASSET_OPERATION_COUNT; x++)
        {
            const auto operation = SoundAssetState.Operations[x];

            if (operation.State == SoundAssetOperationState::None) { break; }

            indx = x;
        }

        if (indx == MAX_SOUND_ASSET_OPERATION_COUNT)
        {
            ReleaseMutex1(SoundAssetState.Mutex);

            return INVALID_SOUND_ASSET_OPERATION_INDEX;
        }

        {
            auto operation = &SoundAssetState.Operations[indx];

            ZeroMemory(operation, sizeof(SoundAssetOperation));

            operation->State = SoundAssetOperationState::Pending;

            strcpy(operation->Name, desc->AbsoluteFileName);

            operation->Offset = desc->FileOffset + offset;

            operation->Length = length;

            operation->Data = data;
        }

        ReleaseMutex1(SoundAssetState.Mutex);

        return indx;
    }

    SoundAssetOperationState AcquireSoundAssetOperationState(const s32 indx)
    {
        if (INVALID_SOUND_ASSET_OPERATION_INDEX < indx && indx < MAX_SOUND_ASSET_OPERATION_COUNT)
        {
            return SoundAssetState.Operations[indx].State;
        }

        return SoundAssetOperationState::Available;
    }
}