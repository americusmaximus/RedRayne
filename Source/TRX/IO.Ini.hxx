#pragma once

#include "IO.hxx"
#include "Memory.hxx"
#include "Objects.hxx"

#define MAX_INI_SECTION_LENGTH 256

namespace IO::Ini
{
    typedef const void* (CDECLAPI* ABSTRACTINIDISPOSE) (void* self, const Memory::DisposeMode mode);
    typedef const void(CDECLAPI* ABSTRACTINIREADSTRING) (void* self, const char* name, char* value, const u32 size);
    typedef const void(CDECLAPI* ABSTRACTINIWRITESTRING) (void* self, const char* name, const char* value);

    struct AbstractIniSelf
    {
        ABSTRACTINIDISPOSE Dispose;
        ABSTRACTINIREADSTRING ReadString;
        ABSTRACTINIWRITESTRING WriteString;
    };

    struct AbstractIni
    {
        char Name[STANDARD_IO_FILE_NAME_LENGTH];
        char Section[MAX_INI_SECTION_LENGTH];
        AbstractIniSelf* Self;
    };

    void AbstractIniSelectName(AbstractIni* self, const char* file);
    void AbstractIniSelectSection(AbstractIni* self, const char* section);

    void AbstractIniReadInteger(AbstractIni* self, const char* name, s32* value);
    void AbstractIniReadReal(AbstractIni* self, const char* name, f32* value);

    void AbstractIniWriteInteger(AbstractIni* ini, const char* name, const s32 value);
    void AbstractIniWriteReal(AbstractIni* ini, const char* name, const f32 value);

    typedef const void*(CDECLAPI* INIFILEDISPOSE) (void* self, const Memory::DisposeMode mode);
    typedef const void(CDECLAPI* INIFILEREADSTRING) (void* self, const char* name, char* value, const u32 size);
    typedef const void(CDECLAPI* INIFILEWRITESTRING) (void* self, const char* name, const char* value);

    void* IniFileDisposeWrapper(void* self, const Memory::DisposeMode mode);
    void IniFileReadStringWrapper(void* self, const char* name, char* value, const u32 size);
    void IniFileWriteStringWrapper(void* self, const char* name, const char* value);

    struct IniFileSelf
    {
        //INIFILEDISPOSE Dispose = (INIFILEDISPOSE)0x004a7a50;//TODO
        //INIFILEREADSTRING ReadString = (INIFILEREADSTRING)0x004a7b20;//TODO
        //INIFILEWRITESTRING WriteString = (INIFILEWRITESTRING)0x004a7bb0;//TODO

        INIFILEDISPOSE Dispose = (INIFILEDISPOSE)&IniFileDisposeWrapper;
        INIFILEREADSTRING ReadString = (INIFILEREADSTRING)&IniFileReadStringWrapper;
        INIFILEWRITESTRING WriteString = (INIFILEWRITESTRING)&IniFileWriteStringWrapper;
    };

    struct IniFileContainer
    {
        IniFileSelf IniFileSelf;
        Objects::AbstractObjectInitializer* _Initializer = (Objects::AbstractObjectInitializer*)0x006113d0;//TODO
    };

    extern IniFileContainer IniFileState;

    struct IniFile
    {
        char Name[STANDARD_IO_FILE_NAME_LENGTH];
        char Section[MAX_INI_SECTION_LENGTH];
        IniFileSelf* Self;
    };

    void IniFileInitialize(IniFile* self, const char* file, const char* section);
    void* IniFileDispose(IniFile* self, const Memory::DisposeMode mode);

    void IniFileReadString(IniFile* self, const char* name, char* value, const u32 size);
    void IniFileWriteString(IniFile* self, const char* name, const char* value);
}