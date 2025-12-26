#include "ASCII.hxx"
#include "IO.hxx"
#include "Strings.hxx"
#include "Memory.hxx"
#include "Logger.hxx"
#include "Assets.hxx"

#include <stdlib.h>
#include <stdio.h>

using namespace Memory;
using namespace Strings;
using namespace Logger;
using namespace Assets;
using namespace IO::Path;

namespace IO
{
    IOContainer IOState;

    // Todo move file cahce into separate files
    FileHandlerContainer FileHandlerState;
    FileCacheContainer FileCacheState;

    void RegisterFileHandler(IOFILEHANDLER lambda)
    {
        if ((MAX_IO_FILE_HANDLER_COUNT - 1) < FileHandlerState.FileHandlerCount)
        {
            LogError("Unable to register file handler. The maximum amount of registered handlers reached.");
        }

        FileHandlerState.FileHandlers[FileHandlerState.FileHandlerCount] = lambda;

        FileHandlerState.FileHandlerCount = FileHandlerState.FileHandlerCount + 1;
    }

    void CloseCacheFile(const char* file)// TODO better name
    {
        char inFileName[STANDARD_IO_FILE_NAME_LENGTH];
        char inFileExt[STANDARD_IO_FILE_NAME_LENGTH];

        if (IsNotNull(file))
        {
            SplitPath(file, NULL, NULL, inFileName, inFileExt);
        }

        for (u32 x = 0; x < MAX_IO_FILE_CACHE_SIZE; x++)
        {
            auto item = FileCacheState.Files[x];

            if (item.Handle != NULL)
            {
                if (IsNull(file))
                {
                    CloseHandle(item.Handle);

                    item.Handle = NULL;
                    item.Name[0] = NULL;
                }
                else
                {
                    char name[STANDARD_IO_FILE_NAME_LENGTH];
                    char ext[STANDARD_IO_FILE_NAME_LENGTH];

                    SplitPath(item.Name, NULL, NULL, name, ext);

                    if (EqualStrings(inFileName, name) && EqualStrings(inFileExt, ext))
                    {
                        CloseHandle(item.Handle);

                        item.Handle = NULL;
                        item.Name[0] = NULL;
                    }
                }
            }
        }
    }

    BOOL CloseCacheFile2(const char* file, const u32 attrs)// TODO better name
    {
        CloseCacheFile(file);

        // TODO
        u32 attributes = ((attrs & 8) != 0)
            ? FILE_ATTRIBUTE_TEMPORARY
            : FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_NORMAL;

        return UpdateFileAttributes(file, attributes) == 0;
    }

    BOOL FileHandler(FileDescriptor* desc) // TODO
    {
        if (IsNullOrEmpty(desc->FileName)) { return FALSE; }

        if (*FileCacheState._IsReadingFromDisk)
        {
            LogMessage("[WARNING] [IO] Reading %s from disk synchronously.\n", desc->FileName);
        }

        if (IsNullOrEmpty(desc->FileName)) { return FALSE; }

        char buff[STANDARD_IO_FILE_NAME_LENGTH];

        if (IsAbsoluteFileName(buff, desc->FileName))
        {
            if (!FileDetails(buff, &desc->Size))
            {
                return FALSE;
            }

            desc->FileOffset = 0;

            desc->Unk003 = 0;// TODO
            desc->ItemTimeStamp = 0;
            desc->FileTimeStamp = 0;

            strcpy(desc->AbsoluteFileName, buff);

            return TRUE;
        }

        return FALSE;
    }

    BOOL HandleFile(FileDescriptor* desc)
    {
        if (FileHandlerState.FileHandlerCount == 0) { return FileHandler(desc); }

        for (u32 x = 0; x < FileHandlerState.FileHandlerCount; x++)
        {
            if (FileHandlerState.FileHandlers[x](desc)) { return TRUE; }
        }

        return FALSE;
    }

    // TODO: This is an improvisation.
    // This shoudl populate timestamps, etc.
    BOOL FileDetails(const char* file, u32* size)
    {
        if (IsNull(file)) { return FALSE; }

        WIN32_FIND_DATAA data;

        ZeroMemory(&data, sizeof(WIN32_FIND_DATAA));

        const auto handle = FindFirstFileA(file, &data);

        if (handle == INVALID_HANDLE_VALUE) { return FALSE; }

        FindClose(handle);

        *size = data.nFileSizeLow;

        return TRUE;
    }

    BOOL IsFileExists(FileDescriptor* desc, char* outMessage)
    {
        if (IsNullOrEmpty(desc->FileName))
        {
            strcpy(outMessage, "No file name specified.");

            return FALSE;
        }

        if (!HandleFile(desc))
        {
            strcpy(outMessage, "File not found.");

            return FALSE;
        }

        return TRUE;
    }

    void InitializeFileCache(void)
    {
        ZeroMemory(FileCacheState.Files, sizeof(File) * MAX_IO_FILE_CACHE_SIZE);
    }

    BOOL DeleteFile2(const char* file)
    {
        if (DeleteFileA(file)) { return FALSE; }

        return TRUE;
    }

    BOOL DeleteFile1(const char* file)
    {
        return DeleteFile2(file);
    }

    BOOL DeleteFile3(const char* file)
    {
        CloseCacheFile(file);

        return !DeleteFile1(file);
    }

    BOOL MoveFile1(const char* src, const char* dest)
    {
        CloseCacheFile(src);
        CloseCacheFile(dest);

        return MoveFile2(src, dest);
    }

    BOOL MoveFile2(const char* src, const char* dest)
    {
        return MoveFileA(src, dest);
    }

    BOOL CreateFolder1(const char* path)
    {
        char dir[STANDARD_IO_DIRECTORY_NAME_LENGTH];

        strcpy(dir, path);

        // Create nested folders.
        for (u32 x = 0; x < STANDARD_IO_DIRECTORY_NAME_LENGTH; x++)
        {
            const char v = dir[x];

            if (v == NULL) { break; }

            if (v == ASCII_SLASH || v == ASCII_BACK_SLASH)
            {
                dir[x] = NULL;

                CreateFolder2(dir);

                dir[x] = v;
            }
        }

        return CreateFolder2(path);
    }

    BOOL CreateFolder2(const char* path)
    {
        return CreateDirectoryA(path, NULL);
    }

    s32 FileAttributes(const char* file)
    {
        char buffer[STANDARD_IO_FILE_NAME_LENGTH];

        for (u32 x = 0; x < STANDARD_IO_FILE_NAME_LENGTH; x++)
        {
            const auto v = file[x];

            buffer[x] = v;

            if (v == NULL || v == ASCII_ASTERISK) { break; }
        }

        for (u32 x = 0; x < STANDARD_IO_FILE_NAME_LENGTH; x++)
        {
            const auto v = buffer[x];

            if (v == NULL) { break; }

            if (v == ASCII_QUESTION_MARK)
            {
                buffer[x] = NULL;
                break;
            }
        }

        if (IsNotNullOrEmpty(buffer))
        {
            WIN32_FIND_DATAA data;
            const auto hande = FindFirstFileA(buffer, &data);

            if (hande == INVALID_HANDLE_VALUE) { -1; }

            return data.dwFileAttributes;
        }

        return -1;
    }

    s32 UpdateFileAttributes(const char* file, u32 attrs)
    {
        auto current = FileAttributes(file);

        if (current == -1) { return -1; }

        auto attributes = current & (~FILE_ATTRIBUTE_READONLY);

        if ((attrs & FILE_ATTRIBUTE_NORMAL) == 0)
        {
            attributes = attributes | FILE_ATTRIBUTE_READONLY;
        }

        if (!SetFileAttributesA(file, attributes)) { return -1; }

        return 0;
    }

    s32 AcquireFileSize(const char* dir, const char* file)
    {
        FileDescriptor desc;

        CombinePath(desc.FileName, dir, file);

        if (HandleFile(&desc)) { return desc.Size; }

        return INVALID_FILE_SIZE;
    }
}

namespace IO::Path
{
    BOOL IsAbsoluteFileName(char* buffer, const char* file)
    {
        if (FullFileName(buffer, file, STANDARD_IO_FILE_NAME_LENGTH) == buffer)
        {
            return TRUE;
        }

        strcpy(buffer, file);

        return FALSE;
    }

    char* FullFileName(char* buffer, const char* file, const u32 size)
    {
        // TODO: Do we need all the magic skipped here?

        if (IsNotNullOrEmpty(file))
        {
            char* lp;

            if (!GetFullPathNameA(file, size, buffer, &lp)) { return NULL; }

            return buffer;
        }

        return CurrentFolder(buffer, size);
    }

    char* CurrentFolder(char* buffer, const u32 size)
    {
        char directory[MAX_IO_FILE_NAME_LENGTH];

        if (!GetCurrentDirectoryA(MAX_IO_FILE_NAME_LENGTH, directory)) { return NULL; }

        // TODO: is allocation needed in practice?
        if (IsNull(buffer))
        {
            buffer = (char*)AllocateMemory(MAX_IO_FILE_NAME_LENGTH);
        }

        strcpy(buffer, directory);

        return buffer;
    }

    void CombinePath(char* buffer, const char* dir, const char* file)
    {
        if (IsNotNullOrEmpty(dir))
        {
            sprintf(buffer, "%s\\%s", dir, file);
        }
        else
        {
            strcpy(buffer, file);
        }
    }

    void SplitPath(const char* file, char* disk, char* dir, char* name, char* ext)
    {
        _splitpath(file, disk, dir, name, ext);
    }

    void MakePath(char* file, char* disk, const char* dir, const char* name, const char* ext)
    {
        _makepath(file, disk, dir, name, ext);
    }

    void ChangeFileExtension(char* file, const char* ext)
    {
        char name[STANDARD_IO_FILE_NAME_LENGTH];
        char dir[STANDARD_IO_DIRECTORY_NAME_LENGTH];
        char disk[STANDARD_IO_DISK_NAME_LENGTH];

        SplitPath(file, disk, dir, name, NULL);
        MakePath(file, disk, dir, name, ext);
    }
}