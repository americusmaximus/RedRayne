#pragma once

#include "Basic.hxx"
#include "Native.Basic.hxx"

#define MAX_IO_FILE_NAME_LENGTH 260

#define STANDARD_IO_DISK_NAME_LENGTH 3
#define STANDARD_IO_FILE_NAME_LENGTH 256
#define STANDARD_IO_DIRECTORY_NAME_LENGTH 256
#define STANDARD_IO_EXTENSION_NAME_LENGTH 256

// TODO: consolidate names
#define MAX_IO_FILE_HANDLER_COUNT 5
#define MAX_IO_FILE_HANDLER_INDEX (MAX_IO_FILE_HANDLER_COUNT - 1)
#define MAX_IO_FILE_CACHE_SIZE MAX_IO_FILE_HANDLER_COUNT

namespace IO // TODO
{
    struct File
    {
        HANDLE Handle;
        char Name[MAX_IO_FILE_NAME_LENGTH];
    };

    struct FileDescriptor
    {
        char FileName[STANDARD_IO_FILE_NAME_LENGTH];
        char AbsoluteFileName[STANDARD_IO_FILE_NAME_LENGTH];
        s32 FileOffset; // TODO type
        u32 Size;
        s32 Unk003;// TODO
        s32 ItemTimeStamp;// TODO
        s32 FileTimeStamp;// TODO
        s32 Unk006;// TODO
    };

    typedef const BOOL(CDECLAPI* IOFILEHANDLER) (FileDescriptor* desc);

    // TODO: Consolidate with FileCacheContainer
    // TODO: move into different files
    struct FileHandlerContainer
    {
        u32 FileHandlerCount;// = (u32*)0x00679638;//TODO
        IOFILEHANDLER FileHandlers[MAX_IO_FILE_HANDLER_COUNT];// = (FILEHANDLER*)0x0067963c;// TODO
    };

    extern FileHandlerContainer FileHandlerState;

    void RegisterFileHandler(IOFILEHANDLER lambda);

    // TODO: Consolidate with FileHandlerContainer
    // TODO: move into different files
    struct FileCacheContainer
    {
        u8* _IsReadingFromDisk = (u8*)0x00679650;//TODO
        File Files[MAX_IO_FILE_CACHE_SIZE];// = (File*)0x008fdb10;// TODO
    };

    extern FileCacheContainer FileCacheState;

    struct IOContainer
    {
        char** _DirectoryName = (char**)0x0060992b;//TODO char[257]
    };

    extern IOContainer IOState;

    void InitializeFileCache(void);

    BOOL FileHandler(FileDescriptor* desc);

    BOOL HandleFile(FileDescriptor* desc);

    BOOL FileDetails(const char* file, u32* size);

    BOOL IsFileExists(FileDescriptor* desc, char* outMessage);

    void CloseCacheFile(const char* file);// TODO bettern name
    BOOL CloseCacheFile2(const char* file, const u32 attrs);// TODO bettern name, enumeration?

    BOOL DeleteFile1(const char* file);
    BOOL DeleteFile2(const char* file);
    BOOL DeleteFile3(const char* file);

    BOOL MoveFile1(const char* src, const char* dest);
    BOOL MoveFile2(const char* src, const char* dest);

    BOOL CreateFolder1(const char* path);
    BOOL CreateFolder2(const char* path);

    s32 FileAttributes(const char* file);
    s32 UpdateFileAttributes(const char* file, u32 attrs);

    s32 AcquireFileSize(const char* dir, const char* file);
}

namespace IO::Path
{
    BOOL IsAbsoluteFileName(char* buffer, const char* file);
    char* FullFileName(char* buffer, const char* file, const u32 size);
    void ChangeFileExtension(char* file, const char* ext);
    char* CurrentFolder(char* buffer, const u32 size);

    void CombinePath(char* buffer, const char* dir, const char* file);
    void SplitPath(const char* file, char* disk, char* dir, char* name, char* ext);
    void MakePath(char* file, char* disk, const char* dir, const char* name, const char* ext);
}