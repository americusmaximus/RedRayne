#include "ASCII.hxx"
#include "IO.Streams.hxx"
#include "Strings.hxx"
#include "Logger.hxx"
#include "Objects.hxx"

#include <stdio.h>

#define OUT_STREAM_FILE_BUFFER_SIZE 16384

using namespace Objects;
using namespace ASCII;
using namespace Logger;
using namespace Strings;
using namespace Memory;
using namespace IO;
using namespace IO::Path;

// TODO: standardize s32/u32 size/count.
namespace IO::Streams
{
    AbstractOutStreamContainer AbstractOutStreamState;
    OutStreamMemoryContainer OutStreamMemoryState;
    OutStreamFileContainer OutStreamFileState;

    AbstractOutStream* AbstractOutStreamConstructor(AbstractOutStream* self)
    {
        self->Self = &AbstractOutStreamState.AbstractOutStreamSelf;

        return self;
    }

    void* AbstractOutStreamDispose(AbstractOutStream* self, const DisposeMode mode)
    {
        self->Self = &AbstractOutStreamState.AbstractOutStreamSelf;

        return self;
    }

    void* AbstractOutStreamDisposeWrapper(void* self, const DisposeMode mode)
    {
        return AbstractOutStreamDispose((AbstractOutStream*)self, mode);
    }

    s32 AbstractOutStreamNotImplemented(void* self)
    {
        // TODO: Not Implemented: Virtual function
        // TODO: Refactor Out
        return STREAM_RESULT_SUCCESS;
    }
    s32 AbstractOutStreamZero(void* self)
    {
        // TODO: Not Implemented
        // TODO: Refactor Out
        return STREAM_RESULT_SUCCESS;
    }
    s32 AbstractOutStreamNegativeOne(void* self)
    {
        // TODO: Not Implemented
        // TODO: Refactor Out
        return STREAM_RESULT_END_OF_DATA;
    }

    void* OutStreamMemoryDisposeWrapper(void* self, const DisposeMode mode)
    {
        return OutStreamMemoryDispose((OutStreamMemory*)self, mode);
    }

    void* OutStreamMemoryDispose(OutStreamMemory* self, const DisposeMode mode)
    {
        if ((mode & DisposeMode::Collection) != DisposeMode::None)
        {
            ReleaseMemory3(ReleaseObject1(self, OutStreamMemoryState._Initializer));

            return self;
        }

        self->Self = &OutStreamMemoryState.OutStreamMemorySelf;

        OutStreamMemoryRelease(self);

        auto abs = AbstractOutStreamDispose((AbstractOutStream*)self, DisposeMode::Unknown1);

        if ((mode & DisposeMode::Unknown2) != DisposeMode::None)
        {
            ReleaseMemory2(abs);
        }

        return abs;
    }

    void OutStreamMemoryReleaseWrapper(void* self)
    {
        OutStreamMemoryRelease((OutStreamMemory*)self);
    }

    void OutStreamMemoryRelease(OutStreamMemory* self)
    {
        if (self->Data != NULL)
        {
            if (self->IsEmpty)
            {
                ReleaseMemory1(self->Data);
            }

            self->Data = NULL;
        }

        self->Offset = 0;
        self->IsEmpty = TRUE;
        self->Size = 0;
    }


    void OutStreamMemorySetWrapper(void* self, void* data, const u32 size)
    {
        OutStreamMemorySet((OutStreamMemory*)self, data, size);
    }

    void OutStreamMemorySet(OutStreamMemory* self, void* data, const u32 size)
    {
        self->Self->Release(self);

        self->Offset = 0;
        self->Unk3 = 0;
        self->IsEmpty = FALSE;

        self->Data = (u8*)data;
        self->Size = size;
    }

    s32 OutStreamMemoryTellWrapper(void* self)
    {
        return OutStreamMemoryTell((OutStreamMemory*)self);
    }

    s32 OutStreamMemoryTell(OutStreamMemory* self)
    {
        return self->Offset;
    }

    BOOL OutStreamMemorySeekWrapper(void* self, const s32 offset, const StreamSeek mode)
    {
        return OutStreamMemorySeek((OutStreamMemory*)self, offset, mode);
    }

    BOOL OutStreamMemorySeek(OutStreamMemory* self, const s32 offset, const StreamSeek mode)
    {
        s32 off = 0;

        switch (mode)
        {
        case StreamSeek::Set:
        {
            return FALSE;
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

        if (-1 < off && (off <= self->Size || self->IsEmpty))
        {
            self->Offset = off;
            return TRUE;
        }

        return FALSE;
    }


    s32 OutStreamMemoryWriteWrapper(void* self, const u8 value)
    {
        return OutStreamMemoryWrite((OutStreamMemory*)self, value);
    }

    s32 OutStreamMemoryWrite(OutStreamMemory* self, const u8 value)
    {
        if (self->Size <= self->Offset)
        {
            if (!self->IsEmpty) { return STREAM_RESULT_END_OF_DATA; }

            const auto length = self->Offset + OUT_STREAM_FILE_BUFFER_SIZE;

            u8* data = (u8*)ReallocateMemory(self->Data, length);

            if (data == NULL) { return STREAM_RESULT_FAILURE; }

            self->Data = data;
            self->Size = length;
        }

        self->Data[self->Offset] = value;

        self->Offset = self->Offset + 1;

        if (self->Unk3 <= self->Offset)
        {
            self->Unk3 = self->Offset;
        }

        return STREAM_RESULT_SUCCESS;
    }

    s32 OutStreamMemoryWriteBlockWrapper(void* self, const void* data, const u32 size)
    {
        return OutStreamMemoryWriteBlock((OutStreamMemory*)self, data, size);
    }

    s32 OutStreamMemoryWriteBlock(OutStreamMemory* self, const void* data, const u32 size)
    {
        if (self->Size - self->Offset < size)
        {
            if (!self->IsEmpty)
            {
                if (self->Size - self->Offset == 0) { return STREAM_RESULT_END_OF_DATA; }
            }
            else
            {
                const auto length = self->Offset + size + OUT_STREAM_FILE_BUFFER_SIZE;

                u8* data = (u8*)ReallocateMemory(self->Data, length);

                if (data == NULL) { return STREAM_RESULT_FAILURE; }

                self->Data = data;
                self->Size = length;
            }
        }

        CopyMemory(&self->Data[self->Offset], data, size);

        self->Offset = self->Offset + size;

        if (self->Unk3 <= self->Offset)
        {
            self->Unk3 = self->Offset;
        }

        return size;
    }

    OutStreamMemory* OutStreamMemoryInitialize(OutStreamMemory* self)
    {
        self = (OutStreamMemory*)AbstractOutStreamConstructor((AbstractOutStream*)self);

        self->Self = &OutStreamMemoryState.OutStreamMemorySelf;

        self->Data = NULL;
        self->Size = 0;
        self->Offset = 0;
        self->Unk3 = 0;
        self->IsEmpty = TRUE;

        return self;
    }

    void OutStreamMemoryInitialize2(OutStreamMemory* self, void* data, const u32 size)
    {
        self = (OutStreamMemory*)AbstractOutStreamConstructor((AbstractOutStream*)self);

        self->Self = &OutStreamMemoryState.OutStreamMemorySelf;

        self->Data = NULL;
        self->Size = 0;
        self->Offset = 0;
        self->Unk3 = 0;
        self->IsEmpty = TRUE;

        OutStreamMemorySet(self, data, size);
    }

    OutStreamMemory* OutStreamMemoryAssign(OutStreamMemory* self, OutStreamMemory* value)
    {
        self = (OutStreamMemory*)AbstractOutStreamConstructor((AbstractOutStream*)self);

        self->Self = &OutStreamMemoryState.OutStreamMemorySelf;

        return self;
    }



    void* OutStreamFileDisposeWrapper(void* self, const DisposeMode mode)
    {
        return OutStreamFileDispose((OutStreamFile*)self, mode);
    }

    void* OutStreamFileDispose(OutStreamFile* self, const DisposeMode mode)
    {
        if ((mode & DisposeMode::Collection) != DisposeMode::None)
        {
            ReleaseMemory3(ReleaseObject1(self, OutStreamFileState._Initializer));

            return self;
        }

        self->Self = &OutStreamFileState.OutStreamFileSelf;

        OutStreamFileRelease(self);

        auto abs = AbstractOutStreamDispose((AbstractOutStream*)self, DisposeMode::Unknown1);

        if ((mode & DisposeMode::Unknown2) != DisposeMode::None)
        {
            ReleaseMemory2(abs);
        }

        return abs;
    }

    BOOL OutStreamFileReleaseWrapper(void* self)
    {
        return OutStreamFileRelease((OutStreamFile*)self);
    }

    BOOL OutStreamFileRelease(OutStreamFile* self)
    {
        if (self->Self->IsAvailable(self))
        {
            fclose(self->Handle);

            if (self->BackUp && !IsLogFileOpen)
            {
                char file[STANDARD_IO_FILE_NAME_LENGTH];

                sprintf(file, "%s.$bak$", self->Name);

                DeleteFile3(file);
            }
        }

        self->Name[0] = NULL;

        return TRUE;
    }

    BOOL OutStreamFileIsAvailableWrapper(void* self)
    {
        return OutStreamFileIsAvailable((OutStreamFile*)self);
    }

    BOOL OutStreamFileIsAvailable(OutStreamFile* self)
    {
        return self->Handle != NULL;
    }

    BOOL OutStreamFileIsAdvanceableWrapper(void* self)
    {
        return OutStreamFileIsAdvanceable((OutStreamFile*)self);
    }

    BOOL OutStreamFileIsAdvanceable(OutStreamFile* self)
    {
        if (self->Self->IsAvailable(self)) { return feof(self->Handle) == 0; }

        return FALSE;
    }

    BOOL OutStreamFileSeekWrapper(void* self, const s32 offset, const StreamSeek mode)
    {
        return OutStreamFileSeek((OutStreamFile*)self, offset, mode);
    }

    BOOL OutStreamFileSeek(OutStreamFile* self, const s32 offset, const StreamSeek mode)
    {
        if (!self->Self->IsAvailable(self)) { return FALSE; }

        return fseek(self->Handle, offset, (u32)mode) == 0;
    }

    s32 OutStreamFileTellWrapper(void* self)
    {
        return OutStreamFileTell((OutStreamFile*)self);
    }

    s32 OutStreamFileTell(OutStreamFile* self)
    {
        if (!self->Self->IsAvailable(self)) { return STREAM_RESULT_END_OF_DATA; }

        return ftell(self->Handle);
    }

    s32 OutStreamFileWriteWrapper(void* self, const u8 value)
    {
        return OutStreamFileWrite((OutStreamFile*)self, value);
    }

    s32* _INT_00628200 = (s32*)0x00628200; // TODO

    void OutStreamFileLogError(OutStreamFile* self)
    {
        self->BackUp = FALSE;

        LogMessage("[WARNING] [IO] Unable to write to file %s.", self->Name);

        if (*_INT_00628200 != 1) // TODO
        {
            LogError("Unable to write to file %s.", self->Name);
        }
    }

    s32 OutStreamFileWrite(OutStreamFile* self, const u8 value)
    {
        if (!self->Self->IsAvailable(self)) { return STREAM_RESULT_FAILURE; }

        const s32 v = (s32)value;

        if (v == fputc(v, self->Handle)) { return STREAM_RESULT_SUCCESS; }

        OutStreamFileLogError(self);

        return STREAM_RESULT_FAILURE;
    }
    
    s32 OutStreamFileWriteBlockWrapper(void* self, const void* data, const u32 size)
    {
        return OutStreamFileWriteBlock((OutStreamFile*)self, data, size);
    }

    s32 OutStreamFileWriteBlock(OutStreamFile* self, const void* data, const u32 size)
    {
        if (!self->Self->IsAvailable(self)) { return STREAM_RESULT_FAILURE; }

        const auto result = fwrite(data, sizeof(u8), size, self->Handle);

        if (result != size)
        {
            OutStreamFileLogError(self);

            if (feof(self->Handle) != 0) { return STREAM_RESULT_FAILURE; }
        }

        return result;
    }

    BOOL OutStreamFileOpenFile1Wrapper(void* self, const char* file, const char* attrs)
    {
        return OutStreamFileOpenFile1((OutStreamFile*)self, file, attrs);
    }

    BOOL OutStreamFileOpenFile1(OutStreamFile* self, const char* file, const char* attrs)
    {
        self->Self->Release(self);

        CloseCacheFile(file);

        if (self->BackUp)
        {
            char backup[STANDARD_IO_FILE_NAME_LENGTH];

            sprintf(backup, "%s.$bak$", file);

            DeleteFile3(backup);

            FileDescriptor desc;
            strcpy(desc.FileName, backup);

            if (FileHandler(&desc) && !MoveFile1(file, backup)) { return FALSE; }
        }

        strcpy(self->Name, file);

        for (u32 x = 0; x < STANDARD_IO_DIRECTORY_NAME_LENGTH; x++)
        {
            const char v = self->Name[x];

            if (v == NULL) { break; }

            if (v == ASCII_SLASH || v == ASCII_BACK_SLASH)
            {
                self->Name[x] = NULL;

                CreateFolder1(self->Name);

                self->Name[x] = v;
            }
        }

        self->Handle = fopen(self->Name, attrs);

        if (self->Handle != NULL) { return TRUE; }

        self->Self->Release(self);

        return FALSE;
    }

    BOOL OutStreamFileOpenFile2Wrapper(void* self, const char* dir, const char* file, const char* attrs)
    {
        return OutStreamFileOpenFile2((OutStreamFile*)self, dir, file, attrs);
    }

    BOOL OutStreamFileOpenFile2(OutStreamFile* self, const char* dir, const char* file, const char* attrs)
    {
        if (IsNull(dir))
        {
            return self->Self->OpenFile1(self, file, attrs);
        }

        char path[MAX_IO_FILE_NAME_LENGTH];
        
        MakePath(path, NULL, dir, file, NULL);

        return self->Self->OpenFile1(self, path, attrs);
    }

    OutStreamFile* OutStreamFileInitialize(OutStreamFile* self)
    {
        self = (OutStreamFile*)AbstractOutStreamConstructor((AbstractOutStream*)self);

        self->Self = &OutStreamFileState.OutStreamFileSelf;

        self->Handle = NULL;
        self->Name[0] = NULL;
        self->BackUp = FALSE;

        return self;
    }

    void OutStreamFileInitialize2(OutStreamFile* self, const char* file, const char* attrs)
    {
        self = (OutStreamFile*)AbstractOutStreamConstructor((AbstractOutStream*)self);

        self->Self = &OutStreamFileState.OutStreamFileSelf;

        self->Handle = NULL;
        self->Name[0] = NULL;
        self->BackUp = FALSE;

        OutStreamFileOpenFile1(self, file, attrs);
    }

    OutStreamFile* OutStreamFileAssign(OutStreamFile* self, OutStreamFile* value)
    {
        self = (OutStreamFile*)AbstractOutStreamConstructor((AbstractOutStream*)self);

        self->Self = &OutStreamFileState.OutStreamFileSelf;

        return self;
    }
}