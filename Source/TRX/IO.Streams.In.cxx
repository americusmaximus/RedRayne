#include "IO.Streams.hxx"
#include "Memory.hxx"
#include "Objects.hxx"
#include "Logger.hxx"
#include "Mathematics.Basic.hxx"
#include "ASCII.hxx"
#include "Strings.hxx"

#include <stdlib.h>

#define IN_STREAM_FILE_BUFFER_SIZE 4096
#define IN_STREAM_FILE_BUFFER_ALIGNMENT_VALUE 32
#define IN_STREAM_FILE_BUFFER_ALIGNMENT_MASK (~(IN_STREAM_FILE_BUFFER_ALIGNMENT_VALUE-1))

#define IN_STREAM_FILE_DISTANCE_TO_SEEK_STEP 0xfff
#define IN_STREAM_FILE_DISTANCE_TO_SEEK_MASK 0xfffff000

using namespace IO::Path;
using namespace ASCII;
using namespace Objects;
using namespace Logger;
using namespace Memory;
using namespace Mathematics;
using namespace Strings;

// TODO: standardize s32/u32 size/count.
namespace IO::Streams
{
    AbstractInStreamContainer AbstractInStreamState;
    InStreamMemoryContainer InStreamMemoryState;
    InStreamFileContainer InStreamFileState;

    AbstractInStream* AbstractInStreamInitialize(AbstractInStream* self)
    {
        self->Self = &AbstractInStreamState.AbstractInStreamSelf;

        return self;
    }

    void* AbstractInStreamDisposeWrapper(void* self, const DisposeMode mode)
    {
        return AbstractInStreamDispose((AbstractInStream*)self, mode);
    }

    s32 AbstractInStreamNotImplemented(void* self)
    {
        // TODO: Not Implemented: Virtual function
        // TODO: Refactor Out
        return STREAM_RESULT_SUCCESS;
    }
    s32 AbstractInStreamZero(void* self)
    {
        // TODO: Not Implemented
        // TODO: Refactor Out
        return STREAM_RESULT_SUCCESS;
    }
    s32 AbstractInStreamNegativeOne(void* self)
    {
        // TODO: Not Implemented
        // TODO: Refactor Out
        return STREAM_RESULT_END_OF_DATA;
    }

    void* InStreamMemoryDisposeWrapper(void* self, const DisposeMode mode)
    {
        return InStreamMemoryDispose((InStreamMemory*)self, mode);
    }
    s32 InStreamMemoryReadWrapper(void* self)
    {
        return InStreamMemoryRead((InStreamMemory*)self);
    }
    BOOL InStreamMemoryBackWrapper(void* self, const s32 count)
    {
        return InStreamMemoryPutBack((InStreamMemory*)self, count);
    }
    s32 InStreamMemoryPeekWrapper(void* self)
    {
        return InStreamMemoryPeek((InStreamMemory*)self);
    }
    s32 InStreamMemoryReadBlockWrapper(void* self, void* data, const u32 count)
    {
        return InStreamMemoryReadBlock((InStreamMemory*)self, data, count);
    }
    BOOL InStreamMemorySeekWrapper(void* self, const s32 offset, const StreamSeek mode)
    {
        return InStreamMemorySeek((InStreamMemory*)self, offset, mode);
    }
    s32 InStreamMemoryTellWrapper(void* self)
    {
        return InStreamMemoryTell((InStreamMemory*)self);
    }
    BOOL InStreamMemoryIsAvailableWrapper(void* self)
    {
        return InStreamMemoryIsAvailable((InStreamMemory*)self);
    }
    s32 InStreamMemoryIsAdvanceableWrapper(void* self)
    {
        return InStreamMemoryIsAdvanceable((InStreamMemory*)self);
    }
    void InStreamMemoryReleaseWrapper(void* self)
    {
        return InStreamMemoryRelease((InStreamMemory*)self);
    }
    void InStreamMemorySetWrapper(void* self, void* data, const u32 count)
    {
        InStreamMemorySet((InStreamMemory*)self, data, count);
    }
    void InStreamMemoryOwnWrapper(void* self, void* data, const u32 count)
    {
        InStreamMemoryOwn((InStreamMemory*)self, data, count);
    }

    void* AbstractInStreamDispose(AbstractInStream* self, const DisposeMode mode)
    {
        self->Self = &AbstractInStreamState.AbstractInStreamSelf;

        return self;
    }

    void* InStreamMemoryDispose(InStreamMemory* self, const DisposeMode mode)
    {
        if ((mode & DisposeMode::Collection) != DisposeMode::None)
        {
            ReleaseMemory3(ReleaseObject1(self, InStreamMemoryState._Initializer));

            return self;
        }

        self->Self = &InStreamMemoryState.InStreamMemorySelf;

        InStreamMemoryRelease(self);

        auto abs = AbstractInStreamDispose((AbstractInStream*)self, DisposeMode::Unknown1);

        if ((mode & DisposeMode::Unknown2) != DisposeMode::None)
        {
            ReleaseMemory2(abs);
        }

        return abs;
    }

    InStreamMemory* InStreamMemoryAssign(InStreamMemory* self, InStreamMemory* value)
    {
        self = (InStreamMemory*)AbstractInStreamInitialize((AbstractInStream*)self);

        self->Self = &InStreamMemoryState.InStreamMemorySelf;

        return self;
    }

    s32 InStreamMemoryRead(InStreamMemory* self)
    {
        const auto offset = self->Offset;

        if (self->Data == NULL) { return STREAM_RESULT_FAILURE; }
        if (self->Size <= offset) { return STREAM_RESULT_END_OF_DATA; }

        const auto result = self->Data[offset];

        self->Offset = offset + 1;

        return result;
    }

    BOOL InStreamMemoryPutBack(InStreamMemory* self, const s32 value)
    {
        if (-1 < value && value < MAX_STREAM_ITEM_VALUE && self->Data != NULL && 0 < self->Offset)
        {
            self->Offset = self->Offset - 1;
            return TRUE;
        }

        return FALSE;
    }

    s32 InStreamMemoryPeek(InStreamMemory* self)
    {
        if (self->Data == NULL) { return STREAM_RESULT_FAILURE; }
        if (self->Size <= self->Offset) { return STREAM_RESULT_END_OF_DATA; }

        return self->Data[self->Offset];
    }

    void InStreamMemoryRelease(InStreamMemory* self)
    {
        if (self->Data != NULL)
        {
            if (self->IsOwned)
            {
                ReleaseMemory1(self->Data);
            }
        }

        self->Data = NULL;
        self->IsOwned = FALSE;
        self->Offset = 0;
        self->Size = 0;
    }

    s32 InStreamMemoryReadBlock(InStreamMemory* self, void* data, const u32 count)
    {
        if (self->Data == NULL) { return STREAM_RESULT_FAILURE; }

        if (self->Offset < self->Size)
        {
            const auto length = Min(self->Size - self->Offset, count);

            CopyMemory(data, &self->Data[self->Offset], length);

            self->Offset = self->Offset + length;

            return length;
        }

        return STREAM_RESULT_END_OF_DATA;
    }

    BOOL InStreamMemorySeek(InStreamMemory* self, const s32 offset, const StreamSeek mode)
    {
        if (self->Data == NULL) { return FALSE; }

        s32 off = 0;

        switch (mode)
        {
        case StreamSeek::Set:
        {
            off = offset;
            break;
        }
        case StreamSeek::Current:
        {
            off = offset + self->Offset;
            break;
        }
        case StreamSeek::End:
        {
            off = offset + self->Size;
            break;
        }
        }

        if (-1 < off && off <= self->Size)
        {
            self->Offset = off;
            return TRUE;
        }

        return FALSE;
    }

    s32 InStreamMemoryTell(InStreamMemory* self)
    {
        if (self->Data == NULL) { return STREAM_RESULT_END_OF_DATA; }

        return self->Offset;
    }

    BOOL InStreamMemoryIsAvailable(InStreamMemory* self)
    {
        return self->Data != NULL;
    }

    s32 InStreamMemoryIsAdvanceable(InStreamMemory* self)
    {
        if (self->Data == NULL) { return STREAM_RESULT_END_OF_DATA; }

        if (self->Size <= self->Offset) { return TRUE; }

        return FALSE;
    }

    void InStreamMemorySet(InStreamMemory* self, void* data, const u32 size)
    {
        self->Self->Release(self);

        self->Data = (u8*)data;
        self->IsOwned = FALSE;
        self->Offset = 0;
        self->Size = size;
    }

    void InStreamMemoryOwn(InStreamMemory* self, void* data, const u32 size)
    {
        self->Self->Release(self);

        self->Data = (u8*)data;
        self->IsOwned = TRUE;
        self->Offset = 0;
        self->Size = size;
    }

    InStreamMemory* InStreamMemoryInitialize(InStreamMemory* self)
    {
        self = (InStreamMemory*)AbstractInStreamInitialize((AbstractInStream*)self);

        self->Self = &InStreamMemoryState.InStreamMemorySelf;

        self->Data = NULL;
        self->IsOwned = FALSE;
        self->Offset = 0;
        self->Size = 0;

        return self;
    }

    void InStreamMemoryInitialize2(InStreamMemory* self, void* data, const u32 size)
    {
        self = (InStreamMemory*)AbstractInStreamInitialize((AbstractInStream*)self);

        self->Self = &InStreamMemoryState.InStreamMemorySelf;

        self->Data = NULL;
        self->IsOwned = FALSE;
        self->Offset = 0;
        self->Size = 0;

        InStreamMemorySet(self, data, size);
    }

    void* InStreamFileDisposeWrapper(void* self, const DisposeMode mode)
    {
        return InStreamFileDispose((InStreamFile*)self, mode);
    }

    void* InStreamFileDispose(InStreamFile* self, const DisposeMode mode)
    {
        if ((mode & DisposeMode::Collection) != DisposeMode::None)
        {
            ReleaseMemory3(ReleaseObject1(self, InStreamFileState._Initializer));

            return self;
        }

        self->Self = &InStreamFileState.InStreamFileSelf;

        InStreamFileRelease(self);

        auto abs = AbstractInStreamDispose((AbstractInStream*)self, DisposeMode::Unknown1);

        if ((mode & DisposeMode::Unknown2) != DisposeMode::None)
        {
            ReleaseMemory2(self);
        }

        return abs;
    }

    s32 InStreamFileReadWrapper(void* self)
    {
        return InStreamFileRead((InStreamFile*)self);
    }

    s32 InStreamFileRead(InStreamFile* self)
    {
        const auto result = InStreamFileReadFile2(self, self->BufferOffset);

        if (STREAM_RESULT_END_OF_DATA < result)
        {
            self->BufferOffset = self->BufferOffset + 1;

            if (self->Unknown2 == 0 && result == ASCII_CARRIAGE_RETURN)
            {
                if (InStreamFileReadFile2(self, self->BufferOffset) == ASCII_NEW_LINE)
                {
                    self->BufferOffset = self->BufferOffset + 1;

                    return ASCII_NEW_LINE;
                }

                return ASCII_NEW_LINE;
            }
        }

        return result;
    }

    BOOL InStreamFileBackWrapper(void* self, const s32 value)
    {
        return InStreamFilePutBack((InStreamFile*)self, value);
    }

    BOOL InStreamFilePutBack(InStreamFile* self, const s32 value)
    {
        if (STREAM_RESULT_END_OF_DATA < value && value < MAX_STREAM_ITEM_VALUE && self->FileHandle != NULL && 0 < self->BufferOffset)
        {
            self->BufferOffset = self->BufferOffset - 1;

            if (value == ASCII_NEW_LINE && self->Unknown2 == 0 && 0 < self->BufferOffset)
            {
                if (InStreamFileReadFile2(self, self->BufferOffset) == ASCII_NEW_LINE)
                {
                    if (InStreamFileReadFile2(self, self->BufferOffset - 1) == ASCII_CARRIAGE_RETURN)
                    {
                        self->BufferOffset = self->BufferOffset - 1;

                        return TRUE;
                    }
                }
            }

            return TRUE;
        }

        return FALSE;
    }

    s32 InStreamFilePeekWrapper(void* self)
    {
        return InStreamFilePeek((InStreamFile*)self);
    }


    s32 InStreamFilePeek(InStreamFile* self)
    {
        const auto result = InStreamFileReadFile2(self, self->BufferOffset);

        if (STREAM_RESULT_END_OF_DATA < result)
        {
            if (self->Unknown2 != 0 || result != ASCII_CARRIAGE_RETURN)
            {
                return result;
            }

            return ASCII_NEW_LINE;
        }

        return result;
    }


    s32 InStreamFileReadBlockWrapper(void* self, void* data, const s32 count)
    {
        return InStreamFileReadBlock((InStreamFile*)self, data, count);
    }


    s32 InStreamFileReadBlock(InStreamFile* self, void* data, const s32 count)
    {
        if (self->FileHandle == NULL) { return STREAM_RESULT_FAILURE; }
        if (self->FileSize <= self->BufferOffset) { return STREAM_RESULT_END_OF_DATA; }

        auto result = 0;
        auto readable = self->FileSize - self->BufferOffset;
        auto left = count;

        if (readable < count)
        {
            left = readable;
        }

        while (TRUE)
        {
            if (left < 1) { return result; }

            if (self->BufferOffset < self->Version || (self->Version + self->BufferSize1 <= self->BufferOffset))
            {
                if (!InStreamFileReadFile1(self, self->BufferOffset)) { break; }
            }

            readable = self->Version + self->BufferSize1 - self->BufferOffset;

            if (left < readable)
            {
                readable = left;
            }

            left = left - readable;

            void* ptr = &((u8*)data)[result];

            CopyMemory(ptr, &self->AlignedBuffer[self->BufferOffset - self->Version], readable);

            result = result + readable;
            self->BufferOffset = self->BufferOffset + readable;
        }

        return result;
    }

    BOOL InStreamFileSeekWrapper(void* self, const s32 offset, const StreamSeek mode)
    {
        return InStreamFileSeek((InStreamFile*)self, offset, mode);
    }

    BOOL InStreamFileSeek(InStreamFile* self, const s32 offset, const StreamSeek mode)
    {
        if (self->FileHandle == NULL) { return FALSE; }

        s32 off = 0;

        switch (mode)
        {
        case StreamSeek::Set:
        {
            off = offset;
            break;
        }
        case StreamSeek::Current:
        {
            off = offset + self->BufferOffset;
            break;
        }
        case StreamSeek::End:
        {
            off = offset + self->FileSize;
            break;
        }
        }

        if (-1 < off && off <= self->FileSize)
        {
            self->BufferOffset = off;
            return TRUE;
        }

        return FALSE;
    }

    s32 InStreamFileTellWrapper(void* self)
    {
        return InStreamFileTell((InStreamFile*)self);
    }
    s32 InStreamFileTell(InStreamFile* self)
    {
        if (self->FileHandle == NULL) { return STREAM_RESULT_END_OF_DATA; }

        return self->BufferOffset;
    }

    BOOL InStreamFileIsAvailableWrapper(void* self)
    {
        return InStreamFileIsAvailable((InStreamFile*)self);
    }

    BOOL InStreamFileIsAvailable(InStreamFile* self)
    {
        return self->FileHandle != NULL;
    }

    s32 InStreamFileIsAdvanceableWrapper(void* self)
    {
        return InStreamFileIsAdvanceable((InStreamFile*)self);
    }

    s32 InStreamFileIsAdvanceable(InStreamFile* self)
    {
        if (self->FileHandle == NULL) { return STREAM_RESULT_END_OF_DATA; }

        if (self->FileSize <= self->BufferOffset) { return TRUE; }

        return FALSE;
    }

    s32 InStreamFileReleaseWrapper(void* self)
    {
        return InStreamFileRelease((InStreamFile*)self);
    }

    s32 InStreamFileRelease(InStreamFile* self) // TODO is return value needed?
    {
        if (self->FileHandle != NULL)
        {
            if (self->FileOffset != 0)
            {
                u32 indx = 0;
                auto file = &FileCacheState.Files[indx];

                if (file->Handle != NULL)
                {
                    do
                    {
                        indx = indx + 1;
                        file = &FileCacheState.Files[indx];

                        if (MAX_IO_FILE_HANDLER_INDEX < indx)
                        {
                            indx = rand() % MAX_IO_FILE_CACHE_SIZE;
                            file = &FileCacheState.Files[indx];

                            CloseHandle(file->Handle);
                            file->Handle = self->FileHandle;

                            break;
                        }
                    } while (file->Handle != NULL);
                }

                strcpy(file->Name, self->FileName);
            }
            else
            {
                CloseHandle(self->FileHandle);
            }

            self->FileHandle = NULL;
        }

        self->FileName[0] = NULL;
        self->FileSize = 0;
        self->FileOffset = 0;
        self->BufferOffset = 0;

        InStreamFileRelease1(self);

        return TRUE;
    }

    void InStreamFileRelease1(InStreamFile* self)
    {
        if (self->Buffer != NULL)
        {
            ReleaseMemory1(self->Buffer);

            self->Buffer = NULL;
        }

        self->Version = 0;
        self->BufferSize = 0;
        self->BufferSize1 = 0;
        self->AlignedBuffer = NULL;
    }

    BOOL InStreamFileSetWrapper(void* self, const char* file, const BOOL mode)
    {
        return InStreamFileSet((InStreamFile*)self, file, mode);
    }
    BOOL InStreamFileOwnWrapper(void* self, const char* dir, const char* file, const BOOL mode)
    {
        return InStreamFileOwn((InStreamFile*)self, dir, file, mode);
    }

    BOOL InStreamFileSet(InStreamFile* self, const char* file, const BOOL mode)
    {
        return self->Self->Own(self, NULL, file, mode);
    }

    BOOL InStreamFileOwn(InStreamFile* self, const char* dir, const char* file, const BOOL mode)
    {
        FileDescriptor desc;

        self->Self->Release(self);

        CombinePath(desc.FileName, dir, file);

        if (HandleFile(&desc))
        {
            return self->Self->OpenFile(self, &desc, mode);
        }

        self->Self->Release(self);

        return FALSE;
    }

    BOOL InStreamFileOpenFileWrapper(void* self, FileDescriptor* desc, const BOOL mode)
    {
        return InStreamFileOpenFile((InStreamFile*)self, desc, mode);
    }

    BOOL InStreamFileOpenFile(InStreamFile* self, FileDescriptor* desc, const BOOL mode)
    {
        self->Self->Release(self);

        if (*FileCacheState._IsReadingFromDisk)
        {
            LogMessage("[WARNING] [IO] Reading %s from disk synchronously.\n", desc->FileName);
        }

        self->FileSize = desc->Size;
        self->BufferOffset = 0;
        self->FileOffset = desc->FileOffset;

        for (u32 x = 0; x < MAX_IO_FILE_HANDLER_COUNT; x++)
        {
            auto file = &FileCacheState.Files[x];

            if (file->Handle != NULL)
            {
                if (EqualStrings(file->Name, desc->AbsoluteFileName))
                {
                    self->FileHandle = file->Handle;

                    file->Handle = NULL;
                    file->Name[0] = NULL;

                    strcpy(self->FileName, desc->AbsoluteFileName);

                    InStreamFileAllocateBuffer(self);

                    self->Unknown2 = mode;

                    return TRUE;
                }
            }
        }

        const auto handle = CreateFileA(desc->AbsoluteFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

        if (handle == INVALID_HANDLE_VALUE)
        {
            self->FileHandle = NULL;
            return FALSE;
        }

        if (handle != NULL)
        {
            self->FileHandle = handle;

            if (!self->Self->IsAvailable(self))
            {
                self->Self->Release(self);

                return FALSE;
            }

            strcpy(self->FileName, desc->AbsoluteFileName);

            InStreamFileAllocateBuffer(self);

            self->Unknown2 = mode;

            return TRUE;
        }

        return FALSE;
    }

    void InStreamFileAllocateBuffer(InStreamFile* self)
    {
        InStreamFileRelease1(self);

        self->BufferSize = IN_STREAM_FILE_BUFFER_SIZE;
        self->Buffer = (u8*)AllocateMemory(IN_STREAM_FILE_BUFFER_SIZE + IN_STREAM_FILE_BUFFER_ALIGNMENT_VALUE);

        if (self->Buffer == NULL)
        {
            LogError("Unable to allocate memory for file buffer.");
        }

        self->Version = 0;
        self->BufferSize1 = 0;
        self->AlignedBuffer = (u8*)(((addr)self->Buffer + IN_STREAM_FILE_BUFFER_ALIGNMENT_VALUE) & IN_STREAM_FILE_BUFFER_ALIGNMENT_MASK);
    }

    BOOL InStreamFileReadFile1(InStreamFile* self, const s32 count)
    {
        const auto distance = count + self->FileOffset & IN_STREAM_FILE_DISTANCE_TO_SEEK_MASK;

        self->BufferSize1 = self->FileSize - (distance - self->FileOffset);
        self->Version = distance - self->FileOffset;

        self->BufferSize1 = self->BufferSize1 + IN_STREAM_FILE_DISTANCE_TO_SEEK_STEP & IN_STREAM_FILE_DISTANCE_TO_SEEK_MASK;

        if (self->BufferSize < self->BufferSize1)
        {
            self->BufferSize1 = self->BufferSize;
        }

        SetFilePointer(self->FileHandle, distance, NULL, FILE_BEGIN);

        DWORD read = 0;
        ReadFile(self->FileHandle, self->AlignedBuffer, self->BufferSize1, &read, NULL);

        self->BufferSize1 = read;

        if (read < 1)
        {
            self->BufferSize1 = 0;

            return FALSE;
        }

        return TRUE;
    }

    s32 InStreamFileReadFile2(InStreamFile* self, const s32 count)
    {
        if (self->FileHandle == NULL) { return STREAM_RESULT_FAILURE; }

        if (count < self->FileSize)
        {
            if (count < self->Version || (self->Version + self->BufferSize1 <= count))
            {
                if (!InStreamFileReadFile1(self, count))
                {
                    return STREAM_RESULT_END_OF_DATA;
                }
            }

            return self->AlignedBuffer[count - self->Version];
        }

        return STREAM_RESULT_END_OF_DATA;
    }

    InStreamFile* InStreamFileInitialize(InStreamFile* self)
    {
        self = (InStreamFile*)AbstractInStreamInitialize((AbstractInStream*)self);

        self->Self = &InStreamFileState.InStreamFileSelf;

        self->FileHandle = NULL;
        self->FileSize = 0;
        self->FileOffset = 0;
        self->BufferOffset = 0;
        self->Buffer = NULL;
        self->AlignedBuffer = NULL;
        self->Version = 0;
        self->BufferSize = 0;
        self->BufferSize1 = 0;
        self->FileName[0] = NULL;

        return self;
    }

    void InStreamFileInitialize2(InStreamFile* self, const char* file, const BOOL mode)
    {
        self = (InStreamFile*)AbstractInStreamInitialize((AbstractInStream*)self);

        self->Self = &InStreamFileState.InStreamFileSelf;

        self->FileHandle = NULL;
        self->FileSize = 0;
        self->FileOffset = 0;
        self->BufferOffset = 0;
        self->Buffer = NULL;
        self->AlignedBuffer = NULL;
        self->Version = 0;
        self->BufferSize = 0;
        self->BufferSize1 = 0;
        self->FileName[0] = NULL;

        InStreamFileSet(self, file, mode);
    }

    InStreamFile* InStreamFileAssign(InStreamFile* self, InStreamFile* value)
    {
        self = (InStreamFile*)AbstractInStreamInitialize((AbstractInStream*)self);

        self->Self = &InStreamFileState.InStreamFileSelf;

        return self;
    }

    // TODO: Does return value needed?
    s32 AbstractStreamSkipLine(AbstractInStream* self, const u32 count)
    {
        s32 left = count;
        s32 skipped = 0;

        while (0 < left)
        {
            s32 value = 0;

            do
            {
                value = self->Self->Read(self);

                if (value == STREAM_RESULT_END_OF_DATA) { return STREAM_RESULT_END_OF_DATA; }
                if (value < STREAM_RESULT_SUCCESS) { return STREAM_RESULT_FAILURE; }

            } while (value != ASCII_NEW_LINE);

            left = left - 1;
            skipped = skipped + 1;

        }

        return skipped;
    }

    s32 InStreamFileReadString(InStreamFile* self, char* buffer, const u32 size)
    {
        u32 indx = 0;

        while (TRUE)
        {
            const auto value = self->Self->Read(self);

            if (value == STREAM_RESULT_END_OF_DATA)
            {
                buffer[indx] = NULL;

                if (indx == 0) { return STREAM_RESULT_END_OF_DATA; }

                return indx;
            }

            if (value < STREAM_RESULT_SUCCESS) { break; }

            if (value != ASCII_CARRIAGE_RETURN)
            {
                if (value == ASCII_NEW_LINE)
                {
                    buffer[indx] = NULL;

                    return indx;
                }

                if (indx < (size - 1))
                {
                    buffer[indx] = (char)value;
                    indx = indx + 1;
                }
            }
        }

        buffer[indx] = NULL;

        return STREAM_RESULT_FAILURE;
    }

    s32 AbstractInStreamIsSpace(AbstractInStream* self)
    {
        s32 value = self->Self->Read(self);

        while (TRUE)
        {
            if (value < STREAM_RESULT_SUCCESS) { return value; }

            if (IsNotSpace(value)) { break; }

            value = self->Self->Read(self);

            if (value == STREAM_RESULT_END_OF_DATA) { return STREAM_RESULT_SUCCESS; }
        }

        if (self->Self->PutBack(self, value)) { return STREAM_RESULT_SUCCESS; }

        return STREAM_RESULT_FAILURE;
    }
}