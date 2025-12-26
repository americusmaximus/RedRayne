#include "Assets.Archives.hxx"
#include "Strings.hxx"
#include "Logger.hxx"
#include "App.hxx"
#include "App.Options.hxx"
#include "IO.Streams.hxx"
#include "IO.Archives.hxx"

#define DEFAULT_ARCHIVE_COLLECTION_FILE_NAME "PCPOD.INI"

#define ENGLISH_MAIN_ARCHIVE_NAME "ENGLISH.POD"
#define ENGLISH_VOX_ARCHIVE_NAME "PCENGVOX.POD"
#define ENGLISH_PACKAGE_ARCHIVE_NAME "PCENGPKG.POD"

#define FRENCH_MAIN_ARCHIVE_NAME "FRENCH.POD"
#define FRENCH_VOX_ARCHIVE_NAME "PCFREVOX.POD"
#define FRENCH_PACKAGE_ARCHIVE_NAME "PCFREPKG.POD"

#define ITALIAN_MAIN_ARCHIVE_NAME "ITALIAN.POD"
#define ITALIAN_VOX_ARCHIVE_NAME "PCITAVOX.POD"
#define ITALIAN_PACKAGE_ARCHIVE_NAME "PCITAPKG.POD"

#define SPANISH_MAIN_ARCHIVE_NAME "SPANISH.POD"
#define SPANISH_VOX_ARCHIVE_NAME "PCSPAVOX.POD"
#define SPANISH_PACKAGE_ARCHIVE_NAME "PCSPAPKG.POD"

#define GERMAN_MAIN_ARCHIVE_NAME "GERMAN.POD"
#define GERMAN_VOX_ARCHIVE_NAME "PCGERVOX.POD"
#define GERMAN_PACKAGE_ARCHIVE_NAME "PCGERPKG.POD"

#define JAPANESE_MAIN_ARCHIVE_NAME "JAPANES.POD"
#define JAPANESE_VOX_ARCHIVE_NAME "PCJAPVOX.POD"
#define JAPANESE_PACKAGE_ARCHIVE_NAME "PCJAPPKG.POD"

using namespace Strings;
using namespace IO::Streams;
using namespace Memory;
using namespace Logger;
using namespace App;
using namespace App::Options;
using namespace Assets::Archives;

namespace IO::Archives
{
    ArchiveFileManagerContainer ArchiveFileManagerState;
    ArchiveFileManagerBaseContainer ArchiveFileManagerBaseState;

    ArchiveManagerContainer ArchiveManagerState;

    void* ArchiveFileManagerBaseDisposeWrapper(void* self, const DisposeMode mode)
    {
        return ArchiveFileManagerBaseDispose((ArchiveFileManagerBase*)self, mode);
    }

    void* ArchiveFileManagerBaseDispose(ArchiveFileManagerBase* self, const DisposeMode mode)
    {
        if ((mode & DisposeMode::Collection) != DisposeMode::None)
        {
            ReleaseMemory3(ReleaseObject1(self, ArchiveFileManagerBaseState._Initializer));

            return self;
        }

        self->Self = &ArchiveFileManagerBaseState.ArchiveFileManagerBaseSelf;

        ArchiveFileManagerBaseRelease(self);

        if ((mode & DisposeMode::Unknown2) != DisposeMode::None)
        {
            ReleaseMemory2(self);
        }

        return self;
    }

    void ArchiveFileManagerBaseInitializeDefaultsWrapper(void* self)
    {
        ArchiveFileManagerBaseInitializeDefaults((ArchiveFileManagerBase*)self);
    }

    void ArchiveFileManagerBaseInitializeDefaults(ArchiveFileManagerBase* self) {}

    ArchiveFileManagerBase* ArchiveFileManagerBaseInitialize(ArchiveFileManagerBase* self)
    {
        self->Count = 0;
        self->Self = &ArchiveFileManagerBaseState.ArchiveFileManagerBaseSelf;

        for (u32 x = 0; x < MAX_ARCHIVE_VOLUME_COUNT; x++)
        {
            self->Volumes[x] = NULL;
        }

        return self;
    }

    void ArchiveFileManagerBaseRelease(ArchiveFileManagerBase* self)
    {
        for (u32 x = 0; x < self->Count; x++)
        {
            auto item = self->Volumes[x];

            if (item != NULL)
            {
                ReleaseMemory2(ArchiveVolumeDispose(item, DisposeMode::None));

                self->Volumes[x] = NULL;
            }
        }

        self->Count = 0;
    }

    BOOL ArchiveFileManagerBaseIsExistsWrapper(void* self, FileDescriptor* desc)
    {
        return ArchiveFileManagerBaseIsExists((ArchiveFileManagerBase*)self, desc);
    }

    BOOL ArchiveFileManagerBaseIsExists(ArchiveFileManagerBase* self, FileDescriptor* desc)
    {
        for (u32 x = 0; x < self->Count; x++)
        {
            if (ArchiveVolumeIsFileExists(self->Volumes[x], desc)) { return TRUE; }
        }

        return FALSE;
    }

    BOOL ArchiveFileManagerBaseMountWrapper(void* self, const char* name, char* outMessage)
    {
        return ArchiveFileManagerBaseMount((ArchiveFileManagerBase*)self, name, outMessage);
    }

    BOOL ArchiveFileManagerBaseMount(ArchiveFileManagerBase* self, const char* name, char* outMessage)
    {
        auto item = (ArchiveVolume*)AllocateMemory(sizeof(ArchiveVolume));

        if (item == NULL)
        {
            if (outMessage != NULL)
            {
                strcpy(outMessage, "Unable to allocate memory to mount an archive file.");
            }

            return FALSE;
        }

        ArchiveVolumeInitialize(item);

        if (ArchiveVolumeMount(item, name, outMessage))
        {
            self->Self->UnMount(self, name);

            if (self->Count < MAX_ARCHIVE_VOLUME_COUNT)
            {
                self->Volumes[self->Count] = item;
                self->Count = self->Count + 1;

                return TRUE;
            }

            if (outMessage != NULL)
            {
                sprintf(outMessage, "Unable to mount an archive, too many archives already mounted, maximum is %d.", MAX_ARCHIVE_VOLUME_COUNT);
            }
        }

        if (item != NULL)
        {
            ReleaseMemory2(ArchiveVolumeDispose(item, DisposeMode::None));
        }

        return FALSE;
    }

    void ArchiveFileManagerBaseUnMountWrapper(void* self, const char* name)
    {
        ArchiveFileManagerBaseUnMount((ArchiveFileManagerBase*)self, name);
    }

    void ArchiveFileManagerBaseUnMount(ArchiveFileManagerBase* self, const char* name)
    {
        FileDescriptor desc;

        strcpy(desc.FileName, name);

        if (!FileHandler(&desc)) { return; }

        for (u32 x = 0; x < self->Count; x++)
        {
            if (EqualStrings(self->Volumes[x]->Name, desc.AbsoluteFileName))
            {
                ArchiveFileManagerBaseUnMount(self, x);

                if (self->Count <= x) { return; }
            }
        }
    }

    BOOL ArchiveFileManagerBaseIsMountedWrapper(void* self, const char* name)
    {
        return ArchiveFileManagerBaseIsMounted((ArchiveFileManagerBase*)self, name);
    }

    BOOL ArchiveFileManagerBaseIsMounted(ArchiveFileManagerBase* self, const char* name)
    {
        FileDescriptor desc;

        strcpy(desc.FileName, name);

        if (FileHandler(&desc))
        {
            for (u32 x = 0; x < self->Count; x++)
            {
                if (EqualStrings(self->Volumes[x]->Name, desc.AbsoluteFileName)) { return TRUE; }
            }
        }

        return FALSE;
    }

    void ArchiveFileManagerBaseReMountWrapper(void* self)
    {
        ArchiveFileManagerBaseReMount((ArchiveFileManagerBase*)self);
    }

    void ArchiveFileManagerBaseReMount(ArchiveFileManagerBase* self)
    {
        char file[MAX_IO_FILE_NAME_LENGTH];
        char message[MAX_ARCHIVE_ERROR_MESSAGE_LENGTH];

        for (u32 x = 0; x < self->Count; x++)
        {
            strcpy(file, self->Volumes[x]->Name);

            if (!ArchiveVolumeMount(self->Volumes[x], file, message))
            {
                LogError("Unable to remount %s, message: %s.", file, message);
            }
        }
    }

    void ArchiveFileManagerBaseUnMount(ArchiveFileManagerBase* self, const s32 indx)
    {
        if (indx < 0 || self->Count < indx)
        {
            LogError("Unable to unmount archive file with invalid index of %d.", indx);
        }

        if (self->Volumes[indx] != NULL)
        {
            ReleaseMemory2(ArchiveVolumeDispose(self->Volumes[indx], DisposeMode::None));
        }

        self->Count = self->Count - 1;

        if (indx < self->Count)
        {
            for (u32 x = indx; x < self->Count; x++)
            {
                self->Volumes[x] = self->Volumes[x + 1];
            }
        }

        self->Volumes[self->Count] = NULL;
    }

    ArchiveFileManager* ArchiveFileManagerInitialize(ArchiveFileManager* self)
    {
        ArchiveFileManagerBaseInitialize((ArchiveFileManagerBase*)self);

        self->Self = &ArchiveFileManagerState.ArchiveFileManagerSelf;

        return self;
    }

    void* ArchiveFileManagerDisposeWrapper(void* self, const DisposeMode mode)
    {
        return ArchiveFileManagerDispose((ArchiveFileManager*)self, mode);
    }

    void* ArchiveFileManagerDispose(ArchiveFileManager* self, const DisposeMode mode)
    {
        if ((mode & DisposeMode::Collection) != DisposeMode::None)
        {
            ReleaseMemory3(ReleaseObject1(self, ArchiveFileManagerState._Initializer));

            return self;
        }

        auto abs = ArchiveFileManagerBaseDispose((ArchiveFileManagerBase*)self, DisposeMode::Unknown1);

        if ((mode & DisposeMode::Unknown2) != DisposeMode::None)
        {
            ReleaseMemory2(abs);
        }

        return abs;
    }

    void ArchiveFileManagerMountDefaultsWrapper(void* self)
    {
        ArchiveFileManagerMountDefaults((ArchiveFileManager*)self);
    }

    void ArchiveFileManagerMountDefaults(ArchiveFileManager* self)
    {
        LogMessage("[ARCHIVE] Mounting default archives...\n");

        char main[STANDARD_IO_FILE_NAME_LENGTH];
        char vox[STANDARD_IO_FILE_NAME_LENGTH];

        char buffer[MAX_ARCHIVE_ERROR_MESSAGE_LENGTH];

        switch (AcquireAppLanguage())
        {
        case ApplicationLanguage::French:
        {
            strcpy(main, FRENCH_MAIN_ARCHIVE_NAME);
            strcpy(vox, FRENCH_VOX_ARCHIVE_NAME);

            break;
        }
        case ApplicationLanguage::Italian:
        {
            strcpy(main, ITALIAN_MAIN_ARCHIVE_NAME);
            strcpy(vox, ITALIAN_VOX_ARCHIVE_NAME);

            break;
        }
        case ApplicationLanguage::Spanish:
        {
            strcpy(main, SPANISH_MAIN_ARCHIVE_NAME);
            strcpy(vox, SPANISH_VOX_ARCHIVE_NAME);

            break;
        }
        case ApplicationLanguage::German:
        {
            strcpy(main, GERMAN_MAIN_ARCHIVE_NAME);
            strcpy(vox, GERMAN_VOX_ARCHIVE_NAME);

            break;
        }
        case ApplicationLanguage::Japanese:
        {
            strcpy(main, JAPANESE_MAIN_ARCHIVE_NAME);
            strcpy(vox, JAPANESE_VOX_ARCHIVE_NAME);

            break;
        }
        case ApplicationLanguage::English:
        default:
        {
            strcpy(main, ENGLISH_MAIN_ARCHIVE_NAME);
            strcpy(vox, ENGLISH_VOX_ARCHIVE_NAME);

            break;
        }
        }

        if (!self->Self->Mount(self, main, buffer) && !self->Self->Mount(self, ENGLISH_MAIN_ARCHIVE_NAME, buffer))
        {
            LogError("Unable to mount archive %s.", main);
        }

        if (!self->Self->Mount(self, vox, buffer) && !self->Self->Mount(self, ENGLISH_VOX_ARCHIVE_NAME, buffer))
        {
            LogError("Unable to mount archive %s.", vox);
        }

        InStreamFile stream;

        InStreamFileInitialize(&stream);

        if (!InStreamFileSet(&stream, DEFAULT_ARCHIVE_COLLECTION_FILE_NAME, FALSE))
        {
            LogError("Unable to open archive %s.", DEFAULT_ARCHIVE_COLLECTION_FILE_NAME);
        }

        u32 count = 0;
        AbstractInStreamScan((AbstractInStream*)&stream, "%d\n", &count);

        for (u32 x = 0; x < count; x++)
        {
            char name[STANDARD_IO_FILE_NAME_LENGTH];

            InStreamFileReadString(&stream, name, STANDARD_IO_FILE_NAME_LENGTH);

            if (!self->Self->Mount(self, name, buffer))
            {
                LogError("Unable to mount archive %s.", name);
            }
        }

        InStreamFileRelease(&stream);

        LogMessage("[ARCHIVE] Mounting default archives completed.\n");

        InStreamFileDispose(&stream, DisposeMode::None);
    }

    void ArchiveFileManagerInitialize2(ArchiveFileManager* self)
    {
        if (self == NULL)
        {
            LogError("Unable to initialize archives.");
        }

        if (self->Count != 0) { return; }

        self->Self->MountDefaults(self);
    }
}