#include "Assets.Archives.hxx"
#include "IO.Streams.hxx"
#include "Logger.hxx"
#include "Memory.hxx"

#define ARCHIVE_VOLUME_VERSION 1
#define ARCHIVE_VOLUME_HEADER_COMMENT_LENGTH 80
#define ARCHIVE_VOLUME_FILE_NAME_LENGTH 32

using namespace Memory;
using namespace Logger;
using namespace IO::Streams;

namespace Assets::Archives
{
    struct ArchiveFileItem
    {
        char Name[ARCHIVE_VOLUME_FILE_NAME_LENGTH];
        u32 Size;
        u32 Offset;
    };

    struct ArchiveVolumeHeader
    {
        u32 Count;
        char Comment[ARCHIVE_VOLUME_HEADER_COMMENT_LENGTH];
    };

    BOOL ArchiveVolumeV1Mount(ArchiveVolume* self, const char* name, InStreamFile* stream, char* outMessage)
    {
        self->Version = ARCHIVE_VOLUME_VERSION;

        ArchiveVolumeHeader header;

        if (InStreamFileReadBlock(stream, &header, sizeof(ArchiveVolumeHeader)) == sizeof(ArchiveVolumeHeader))
        {
            self->Count = header.Count;

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

            self->FileNames = (char*)AllocateMemory(self->Count * ARCHIVE_VOLUME_FILE_NAME_LENGTH);

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

                file->Name = &self->FileNames[x * ARCHIVE_VOLUME_FILE_NAME_LENGTH];

                strcpy(file->Name, item.Name);

                self->Files[x].Size = item.Size;
                self->Files[x].Offset = item.Offset;
                self->Files[x].CRC = 0;
                self->Files[x].TimeStamp = self->TimeStamp;
            }

            LogMessage("[ARCHIVE] Archive file %s sucessfully mounted, it contains %d files.\n", name, self->Count);

            return TRUE;
        }

        sprintf(outMessage, "Archive file %s is corrupted", name);

        return FALSE;
    }
}