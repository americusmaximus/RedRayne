#pragma once

#include "IO.hxx"
#include "IO.Streams.hxx"
#include "Memory.hxx"
#include "Objects.hxx"

#define ARCHIVE_VOLUME_MAGIC_LENGTH 4

#define ARCHIVE_VOLUME_V1E_MAGIC "dtxe"
#define ARCHIVE_VOLUME_V2_MAGIC "POD2"
#define ARCHIVE_VOLUME_V3_MAGIC "POD3"

#define MAX_ARCHIVE_ERROR_MESSAGE_LENGTH 512

#define INVALID_ARCHIVE_FILE_INDEX (-1)

#define MIN_ARCHIVE_VOLUME_FILE_COUNT 0
#define MAX_ARCHIVE_VOLUME_FILE_COUNT 9999999

#define ARCHIVE_VOLUME_COMMENT_LENGTH 256
#define ARCHIVE_VOLUME_AUTHOR_LENGTH 256
#define ARCHIVE_VOLUME_COPYRIGHT_LENGTH 256

#define DEFAULT_ARCHIVE_VOLUME_PRIORITY 1000
#define DEFAULT_ARCHIVE_VOLUME_REVISION 1000

namespace Assets::Archives
{
    BOOL ArchiveFileHandler(IO::FileDescriptor* desc);

    enum class ArchiveVolumeVersion : u32
    {
        Invalid = 0,
        V1 = 1, // POD
        V1E = 2, // EPD
        V2 = 3, // POD2
        V3 = 4 // POD3
    };

    ArchiveVolumeVersion AcquireArchiveVolumeVersion(const char* magic);

    struct ArchiveFile
    {
        char* Name;
        u32 Size;
        u32 Offset;
        u32 TimeStamp;
        u32 CRC;
    };

    struct ArchiveVolume
    {
        u32 Version;

        char Name[STANDARD_IO_FILE_NAME_LENGTH];

        u32 TimeStamp;

        char Comment[ARCHIVE_VOLUME_COMMENT_LENGTH];
        char Author[ARCHIVE_VOLUME_AUTHOR_LENGTH];
        char Copyright[ARCHIVE_VOLUME_COPYRIGHT_LENGTH];

        u32 Priority;
        u32 Revision;

        s32 DescUnk006;//TODO

        s32 Count;
        ArchiveFile* Files;

        // Note: One large array for all file names in the archive file.
        // This is done so that there is only one memory allocation to keep track of.
        // ArchiveFileItemDefinition.Name points to different locations within this allocated memory.
        char* FileNames;

        s32 AuditCount;
        u32 AuditOffset;
        s32 UnknownCount;
        s32 UnknownOffset;
    };

    BOOL ArchiveVolumeV1Mount(ArchiveVolume* self, const char* name, IO::Streams::InStreamFile* stream, char* outMessage);
    BOOL ArchiveVolumeV1EMount(ArchiveVolume* self, const char* name, IO::Streams::InStreamFile* stream, char* outMessage);
    BOOL ArchiveVolumeV2Mount(ArchiveVolume* self, const char* name, IO::Streams::InStreamFile* stream, char* outMessage);
    BOOL ArchiveVolumeV3Mount(ArchiveVolume* self, const char* name, IO::Streams::InStreamFile* stream, char* outMessage);


    void ArchiveVolumeInitialize(ArchiveVolume* self);
    void ArchiveVolumeRelease(ArchiveVolume* self);
    void* ArchiveVolumeDispose(ArchiveVolume* self, const Memory::DisposeMode mode);
    BOOL ArchiveVolumeIsFileExists(ArchiveVolume* self, IO::FileDescriptor* desc);
    void ArchiveVolumeAcquireFile(ArchiveVolume* self, const s32 indx, IO::FileDescriptor* desc);
    s32 ArchiveVolumeAcquireFileIndex(ArchiveVolume* self, IO::FileDescriptor* desc);
    BOOL ArchiveVolumeMount(ArchiveVolume* self, const char* name, char* outMessage);
}