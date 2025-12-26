#include "Assets.Archives.hxx"
#include "IO.Archives.hxx"
#include "Logger.hxx"
#include "Memory.hxx"
#include "Strings.hxx"

using namespace Objects;
using namespace IO;
using namespace IO::Path;
using namespace Logger;
using namespace IO::Streams;
using namespace Memory;
using namespace Strings;
using namespace IO::Archives;

namespace Assets::Archives
{
    BOOL ArchiveFileHandler(FileDescriptor* desc)
    {
        if (*ArchiveManagerState._ArchiveFileManagerPointer == NULL) { return FALSE; }

        return (*ArchiveManagerState._ArchiveFileManagerPointer)->Self->IsExists(*ArchiveManagerState._ArchiveFileManagerPointer, desc);
    }

    ArchiveVolumeVersion AcquireArchiveVolumeVersion(const char* magic)
    {
        if (memcmp(magic, ARCHIVE_VOLUME_V1E_MAGIC, ARCHIVE_VOLUME_MAGIC_LENGTH) == 0) { return ArchiveVolumeVersion::V1E; }
        if (memcmp(magic, ARCHIVE_VOLUME_V2_MAGIC, ARCHIVE_VOLUME_MAGIC_LENGTH) == 0) { return ArchiveVolumeVersion::V2; }
        if (memcmp(magic, ARCHIVE_VOLUME_V3_MAGIC, ARCHIVE_VOLUME_MAGIC_LENGTH) == 0) { return ArchiveVolumeVersion::V3; }

        return ArchiveVolumeVersion::V1;
    }

    void ArchiveVolumeInitialize(ArchiveVolume* self)
    {
        self->Version = 0;

        self->Name[0] = NULL;

        self->Comment[0] = NULL;
        self->Author[0] = NULL;
        self->Copyright[0] = NULL;

        self->Count = 0;
        self->AuditCount = 0;

        self->UnknownCount = 0;

        self->Files = NULL;
        self->FileNames = NULL;

        self->Priority = DEFAULT_ARCHIVE_VOLUME_PRIORITY;
        self->Revision = DEFAULT_ARCHIVE_VOLUME_REVISION;
    }

    void* ArchiveVolumeDispose(ArchiveVolume* self, Memory::DisposeMode mode)
    {
        ArchiveVolumeRelease(self);

        return self;
    }

    void ArchiveVolumeRelease(ArchiveVolume* self)
    {
        self->Version = 0;

        self->Name[0] = NULL;

        self->Count = 0;

        if (self->Files != NULL)
        {
            ReleaseMemory1(self->Files);

            self->Files = NULL;
        }

        self->UnknownCount = 0;
        self->AuditCount = 0;

        if (self->FileNames != NULL)
        {
            ReleaseMemory1(self->FileNames);

            self->FileNames = NULL;
        }

        self->Comment[0] = NULL;
        self->Author[0] = NULL;
        self->Copyright[0] = NULL;

        self->Priority = DEFAULT_ARCHIVE_VOLUME_PRIORITY;
        self->Revision = DEFAULT_ARCHIVE_VOLUME_REVISION;
    }

    BOOL ArchiveVolumeIsFileExists(ArchiveVolume* self, FileDescriptor* desc)
    {
        const auto indx = ArchiveVolumeAcquireFileIndex(self, desc);

        if (indx < 0) { return FALSE; }

        ArchiveVolumeAcquireFile(self, indx, desc);

        return TRUE;
    }

    s32 ArchiveVolumeAcquireFileIndex(ArchiveVolume* self, FileDescriptor* desc)
    {
        for (u32 x = 0; x < self->Count; x++)
        {
            if (EqualStrings(desc->FileName, self->Files[x].Name)) { return x; }
        }

        return INVALID_ARCHIVE_FILE_INDEX;
    }

    void ArchiveVolumeAcquireFile(ArchiveVolume* self, s32 indx, FileDescriptor* desc)
    {
        strcpy(desc->AbsoluteFileName, self->Name);

        auto file = &self->Files[indx];

        desc->FileOffset = file->Offset;
        desc->Size = file->Size;

        desc->Unk003 = 1;

        desc->ItemTimeStamp = file->TimeStamp;
        desc->FileTimeStamp = self->TimeStamp;
        desc->Unk006 = self->DescUnk006;
    }

    BOOL ArchiveVolumeMount(ArchiveVolume* self, const char* name, char* outMessage)
    {
        LogMessage("[ARCHIVE] Mounting archive file %s...\n", name);

        char buffer[MAX_ARCHIVE_ERROR_MESSAGE_LENGTH];

        InStreamFile stream;

        InStreamFileInitialize(&stream);
        ArchiveVolumeRelease(self);

        FileDescriptor desc;

        CombinePath(desc.FileName, NULL, name);

        if (!FileHandler(&desc))
        {
            InStreamFileRelease(&stream);
            ArchiveVolumeRelease(self);

            if (outMessage != NULL)
            {
                sprintf(outMessage, "File %s not found.", name);
            }

            LogMessage("[ERROR] [ARCHIVE] File %s not found.\n", name);

            InStreamFileDispose(&stream, DisposeMode::None);

            return FALSE;
        }

        strcpy(self->Name, desc.AbsoluteFileName);

        self->TimeStamp = desc.ItemTimeStamp;

        self->DescUnk006 = desc.Unk006;

        if (!InStreamFileSet(&stream, self->Name, TRUE))
        {
            InStreamFileRelease(&stream);
            ArchiveVolumeRelease(self);

            if (outMessage != NULL)
            {
                sprintf(outMessage, "Unable to open file %s.", name);
            }

            LogMessage("[ERROR] [ARCHIVE] Unable to open file %s.\n", name);

            InStreamFileDispose(&stream, DisposeMode::None);

            return FALSE;
        }

        char extension[STANDARD_IO_EXTENSION_NAME_LENGTH];

        SplitPath(name, NULL, NULL, NULL, extension);

        BOOL result = FALSE;
        auto version = ArchiveVolumeVersion::Invalid;

        {
            char magic[ARCHIVE_VOLUME_MAGIC_LENGTH];

            if (InStreamFileReadBlock(&stream, &magic, ARCHIVE_VOLUME_MAGIC_LENGTH) == ARCHIVE_VOLUME_MAGIC_LENGTH)
            {
                version = AcquireArchiveVolumeVersion(magic);
            }

            InStreamFileSeek(&stream, 0, StreamSeek::Set);
        }

        switch (version)
        {
        case ArchiveVolumeVersion::V1:
        {
            result = ArchiveVolumeV1Mount(self, name, &stream, buffer);
            break;
        }
        case ArchiveVolumeVersion::V1E:
        {
            result = ArchiveVolumeV1EMount(self, name, &stream, buffer);

            break;
        }
        case ArchiveVolumeVersion::V2:
        {
            result = ArchiveVolumeV2Mount(self, name, &stream, buffer);
            break;
        }
        case ArchiveVolumeVersion::V3:
        {
            result = ArchiveVolumeV3Mount(self, name, &stream, buffer);
            break;
        }
        default:
        {
            result = FALSE;

            sprintf(buffer, "Archive file %s is corrupted", name);
        }
        }

        InStreamFileRelease(&stream);

        if (!result)
        {
            ArchiveVolumeRelease(self);

            if (outMessage != NULL)
            {
                strcpy(outMessage, buffer);
            }

            LogMessage("[ERROR] [ARCHIVE] %s.\n", buffer);
        }

        InStreamFileDispose(&stream, DisposeMode::None);

        return result;
    }
}