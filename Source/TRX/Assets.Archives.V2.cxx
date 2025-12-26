#include "Assets.Archives.hxx"
#include "IO.Streams.hxx"
#include "Logger.hxx"
#include "Memory.hxx"

#define ARCHIVE_VOLUME_VERSION 200
#define ARCHIVE_V2_HEADER_COMMENT_LENGTH 80
#define MAX_ARCHIVE_V2_FILE_NAME_LENGTH 256

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
        char Comment[ARCHIVE_V2_HEADER_COMMENT_LENGTH];
        u32 Count;
        u32 AuditCount;
    };

    BOOL ArchiveVolumeV2Mount(ArchiveVolume* self, const char* name, InStreamFile* stream, char* outMessage)
    {
        self->Version = ARCHIVE_VOLUME_VERSION;

        ArchiveVolumeHeader header;

        if (InStreamFileReadBlock(stream, &header, sizeof(ArchiveVolumeHeader)) == sizeof(ArchiveVolumeHeader))
        {
            self->Count = header.Count;
            self->AuditCount = header.AuditCount;

            strcpy(self->Comment, header.Comment);

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

            const auto fns = self->Count * MAX_ARCHIVE_V2_FILE_NAME_LENGTH;

            self->FileNames = (char*)AllocateMemory(fns);

            if (self->FileNames == NULL)
            {
                sprintf(outMessage, "Unable to allocate memory while loading archive file %s", name);

                return FALSE;
            }

            if (InStreamFileReadBlock(stream, self->FileNames, fns) == fns)
            {
                u32 indx = 0;

                for (u32 x = 0; x < self->Count; x++)
                {
                    char* str = &self->FileNames[indx];

                    const u32 len = strlen(str);

                    self->Files[x].Name = str;

                    indx = indx + len + 1;
                }

                self->AuditOffset = self->Files[self->Count - 1].Offset + self->Files[self->Count - 1].Size;

                LogMessage("[ARCHIVE] Archive file %s sucessfully mounted, it contains %d files.\n", name, self->Count);

                return TRUE;
            }
        }

        sprintf(outMessage, "Archive file %s is corrupted", name);

        return FALSE;
    }
}