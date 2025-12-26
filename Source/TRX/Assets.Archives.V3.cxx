#include "Assets.Archives.hxx"
#include "IO.Streams.hxx"
#include "Logger.hxx"
#include "Memory.hxx"

#define ARCHIVE_VOLUME_VERSION 300
#define ARCHIVE_VOLUME_HEADER_COMMENT_LENGTH 80
#define ARCHIVE_VOLUME_HEADER_AUTHOR_LENGTH 80
#define ARCHIVE_VOLUME_HEADER_COPYRIGHT_LENGTH 80
#define ARCHIVE_VOLUME_FILE_NAME_LENGTH 256

using namespace Memory;
using namespace Logger;
using namespace IO::Streams;

namespace Assets::Archives
{
    struct ArchiveFileItem
    {
        u32 NameOffset;
        u32 Size;
        u32 Offset;
        u32 TimeStamp;
        u32 CRC;
    };

    struct ArchiveVolumeHeader
    {
        char Magic[ARCHIVE_VOLUME_MAGIC_LENGTH];
        u32 CRC;
        char Comment[ARCHIVE_VOLUME_HEADER_COMMENT_LENGTH];
        u32 Count;
        u32 AuditCount;
        u32 Revision;
        u32 Priority;
        char Author[ARCHIVE_VOLUME_HEADER_AUTHOR_LENGTH];
        char Copyright[ARCHIVE_VOLUME_HEADER_COPYRIGHT_LENGTH];
        u32 IndexOffset;
        u32 Unknown1; // TODO
        u32 IndexSize;
        u32 UnknownCount; // TODO
        u32 Unknown3; // TODO
        u32 Unknown4; //TODO
    };

    BOOL ArchiveVolumeV3Mount(ArchiveVolume* self, const char* name, InStreamFile* stream, char* outMessage)
    {
        self->Version = ARCHIVE_VOLUME_VERSION;

        ArchiveVolumeHeader header;

        if (InStreamFileReadBlock(stream, &header, sizeof(ArchiveVolumeHeader)) == sizeof(ArchiveVolumeHeader))
        {
            self->Count = header.Count;
            self->AuditCount = header.AuditCount;
            self->UnknownCount = header.UnknownCount;
            self->Priority = header.Priority;
            self->Revision = header.Revision;

            strcpy(self->Comment, header.Comment);
            strcpy(self->Author, header.Author);
            strcpy(self->Copyright, header.Copyright);

            if (self->Count < MIN_ARCHIVE_VOLUME_FILE_COUNT || MAX_ARCHIVE_VOLUME_FILE_COUNT < self->Count)
            {
                sprintf(outMessage, "Archive file %s is corrupted", name);

                return FALSE;
            }

            self->Files = (ArchiveFile*)AllocateMemory(self->Count * sizeof(ArchiveFile));

            if (self->Files == NULL)
            {
                sprintf(outMessage, "Unable to allocate memory while loading archive file %s", name);

                return FALSE;
            }

            InStreamFileSeek(stream, header.IndexOffset, StreamSeek::Set);

            for (u32 x = 0; x < self->Count; x++)
            {
                ArchiveFileItem item;

                if (InStreamFileReadBlock(stream, &item, sizeof(ArchiveFileItem)) != sizeof(ArchiveFileItem))
                {
                    sprintf(outMessage, "Unable to read archive file %s, or the file is corrupted", name);

                    return FALSE;
                }

                auto file = &self->Files[x];

                self->Files[x].Name = NULL;
                self->Files[x].Size = item.Size;
                self->Files[x].Offset = item.Offset;
                self->Files[x].CRC = item.CRC;
                self->Files[x].TimeStamp = item.TimeStamp;
            }

            if (header.IndexSize < self->Count || (self->Count * ARCHIVE_VOLUME_FILE_NAME_LENGTH) < header.IndexSize)
            {
                sprintf(outMessage, "Archive file %s is corrupted", name);

                return FALSE;
            }

            self->FileNames = (char*)AllocateMemory(header.IndexSize);

            if (self->FileNames == NULL)
            {
                sprintf(outMessage, "Unable to allocate memory while loading archive file %s", name);

                return FALSE;
            }

            if (InStreamFileReadBlock(stream, self->FileNames, header.IndexSize) == header.IndexSize)
            {
                u32 indx = 0;

                for (u32 x = 0; x < self->Count; x++)
                {
                    char* str = &self->FileNames[indx];

                    const u32 len = strlen(str);

                    self->Files[x].Name = str;

                    indx = indx + len + 1;
                }

                self->UnknownOffset = InStreamFileTell(stream);

                // NOTE: These unknown structures are 0x108 bytes long.

                self->AuditOffset = self->UnknownOffset + self->UnknownCount * 0x108;

                LogMessage("[ARCHIVE] Archive file %s sucessfully mounted, it contains %d files.\n", name, self->Count);

                return TRUE;
            }
        }

        sprintf(outMessage, "Archive file %s is corrupted", name);

        return FALSE;
    }
}