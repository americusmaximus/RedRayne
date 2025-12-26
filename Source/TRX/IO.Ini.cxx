#include "IO.Ini.hxx"
#include "Strings.hxx"
#include "Logger.hxx"
#include "ASCII.hxx"
#include "Objects.hxx"

#include <stdio.h>

#define MAX_INI_STRING_BUFFER_LENGTH 256
#define MAX_INI_NUMBER_BUFFER_LENGTH 100

using namespace Objects;
using namespace Strings;
using namespace Memory;
using namespace Logger;

namespace IO::Ini
{
    IniFileContainer IniFileState;

    void AbstractIniSelectName(AbstractIni* self, const char* file)
    {
        if (IsNullOrEmpty(file))
        {
            self->Name[0] = NULL;

            return;
        }

        strcpy(self->Name, file);
    }

    void AbstractIniSelectSection(AbstractIni* self, const char* section)
    {
        if (IsNullOrEmpty(section))
        {
            self->Section[0] = NULL;

            return;
        }

        strcpy(self->Section, section);
    }

    void AbstractIniReadInteger(AbstractIni* self, const char* name, s32* value)
    {
        char buffer[MAX_INI_NUMBER_BUFFER_LENGTH];

        sprintf(buffer, "%d", *value);

        self->Self->ReadString(self, name, buffer, MAX_INI_NUMBER_BUFFER_LENGTH);

        sscanf(buffer, "%d", value);
    }

    void AbstractIniReadReal(AbstractIni* self, const char* name, f32* value)
    {
        char buffer[MAX_INI_NUMBER_BUFFER_LENGTH];

        sprintf(buffer, "%g", (f64)*value);

        self->Self->ReadString(self, name, buffer, MAX_INI_NUMBER_BUFFER_LENGTH);

        sscanf(buffer, "%f", value);
    }

    void AbstractIniWriteInteger(AbstractIni* ini, const char* name, const s32 value)
    {
        char buffer[MAX_INI_NUMBER_BUFFER_LENGTH];

        sprintf(buffer, "%d", value);

        ini->Self->WriteString(ini, name, buffer);
    }

    void AbstractIniWriteReal(AbstractIni* ini, const char* name, const f32 value)
    {
        char buffer[MAX_INI_NUMBER_BUFFER_LENGTH];

        sprintf(buffer, "%g", (f64)value);

        ini->Self->WriteString(ini, name, buffer);
    }

    void IniFileInitialize(IniFile* self, const char* file, const char* section)
    {
        self->Self = &IniFileState.IniFileSelf;

        AbstractIniSelectName((AbstractIni*)self, file);
        AbstractIniSelectSection((AbstractIni*)self, section);
    }

    void* IniFileDisposeWrapper(void* self, const DisposeMode mode)
    {
        return IniFileDispose((IniFile*)self, mode);
    }

    void* IniFileDispose(IniFile* self, const DisposeMode mode)
    {
        if ((mode & DisposeMode::Collection) != DisposeMode::None)
        {
            ReleaseMemory3(ReleaseObject1(self, IniFileState._Initializer));

            return self;
        }

        self->Self = &IniFileState.IniFileSelf;

        if ((mode & DisposeMode::Unknown2) != DisposeMode::None)
        {
            ReleaseMemory2(self);
        }

        return self;
    }

    void IniFileReadStringWrapper(void* self, const char* name, char* value, const u32 size)
    {
        IniFileReadString((IniFile*)self, name, value, size);
    }

    BOOL IniFileReadStringValue(const char* file, const char* section, const char* name, char* value, const u32 size)
    {
        // TODO: use winapi
        const auto f = fopen(file, "rt");

        BOOL found = FALSE;
        s32 index = -1;
        char line[MAX_INI_STRING_BUFFER_LENGTH];

        if (f != NULL)
        {
            char tag[MAX_INI_STRING_BUFFER_LENGTH];

            sprintf(tag, "[%s]\n", section);

            // Find the section in the file.
            while (!feof(f) && fgets(line, MAX_INI_STRING_BUFFER_LENGTH, f) != NULL)
            {
                if (EqualStrings(line, tag))
                {
                    found = TRUE;
                    break;
                }
            }

            if (found)
            {
                found = FALSE;

                const auto vln = strlen(name);

                while (!feof(f) && fgets(line, MAX_INI_STRING_BUFFER_LENGTH, f) != NULL)
                {
                    if (StartsWith(line, name))
                    {
                        if (line[vln] != ASCII_EQUAL && line[vln] != ASCII_SPACE) { continue; }

                        found = TRUE;

                        for (u32 x = 0; x < MAX_INI_STRING_BUFFER_LENGTH; x++)
                        {
                            const auto v = line[x];

                            if (v == NULL) { break; }

                            if (index == -1 && v == ASCII_EQUAL)
                            {
                                index = x + 1;
                            }

                            if (v == ASCII_CARRIAGE_RETURN || v == ASCII_NEW_LINE)
                            {
                                line[x] = NULL;
                            }
                        }

                        break;
                    }
                }
            }

            fclose(f);
        }

        if (found)
        {
            ZeroMemory(value, size);

            strncpy(value, &line[index], strlen(&line[index]));

            return TRUE;
        }

        return FALSE;
    }

    void IniFileReadString(IniFile* self, const char* name, char* value, const u32 size)
    {
        if (IsNullOrEmpty(self->Name))
        {
            LogError("Unable to read a string value from an ini file. File name not set.");
        }

        if (IsNullOrEmpty(self->Section))
        {
            LogError("Unable to read a string value from an ini file. Section name not set.");
        }

        IniFileReadStringValue(self->Name, self->Section, name, value, size);
    }

    void IniFileWriteStringWrapper(void* self, const char* name, const char* value)
    {
        IniFileWriteString((IniFile*)self, name, value);
    }

    u32 IniFileAcquireLineIndex(const char* file, const char* section, const char* name, const char* value, BOOL* update)
    {
        *update = TRUE;

        u32 result = 0;
        BOOL found = FALSE;
        const auto f = fopen(file, "rt");// TODO: use winapi

        if (f == NULL) { return 0; }

        char buffer[MAX_INI_STRING_BUFFER_LENGTH];
        char line[MAX_INI_STRING_BUFFER_LENGTH];

        sprintf(buffer, "[%s]\n", section);

        while (!feof(f) && fgets(line, (MAX_INI_STRING_BUFFER_LENGTH - 1), f) != NULL)
        {
            result = result + 1;

            if (EqualStrings(line, buffer))
            {
                found = TRUE;

                break;
            }
        }

        if (found)
        {
            found = FALSE;

            const auto vln = strlen(name);

            while (!feof(f) && fgets(line, (MAX_INI_STRING_BUFFER_LENGTH - 1), f) != NULL)
            {
                if (StartsWith(line, name))
                {
                    if (line[vln] != ASCII_EQUAL && line[vln] != ASCII_SPACE) { continue; }

                    found = TRUE;

                    for (u32 x = 0; x < MAX_INI_STRING_BUFFER_LENGTH; x++)
                    {
                        const auto v = line[x];

                        if (v == NULL) { break; }

                        if (v == ASCII_EQUAL)
                        {
                            if (StartsWith(&line[x + 1], value))
                            {
                                *update = FALSE;

                                break;
                            }
                        }
                    }

                    break;
                }

                result = result + 1;
            }
        }

        fclose(f);

        return found ? result : 0;
    }

    BOOL IniFileWriteStringValue(const char* file, const char* section, const char* name, const char* value)
    {
        BOOL update = 0;
        const auto li = IniFileAcquireLineIndex(file, section, name, value, &update);

        if (!update) { return TRUE; }

        auto f1 = fopen(file, "rt");// TODO: use winapi

        if (f1 == NULL)
        {
            auto const f2 = fopen(file, "wt");// TODO: use winapi

            if (f2 == NULL) { return FALSE; }

            fclose(f2);

            f1 = fopen(file, "rt");// TODO: use winapi

            if (f1 == NULL) { return FALSE; }
        }

        char buffer[MAX_INI_SECTION_LENGTH];

        strcpy(buffer, file);
        buffer[strlen(buffer) - 1] = ASCII_CHARACTER_LOWER_CASE_X;

        const auto f2 = fopen(buffer, "wt");// TODO: use winapi

        if (f2 == NULL) { LogError("Unable to create an ini file to write to."); }

        if (li == 0)
        {
            BOOL found = FALSE;
            char tag[MAX_INI_STRING_BUFFER_LENGTH];
            char line[MAX_INI_STRING_BUFFER_LENGTH];

            sprintf(tag, "[%s]\n", section);

            while (!feof(f1) && fgets(line, (MAX_INI_STRING_BUFFER_LENGTH - 1), f1) != NULL)
            {
                fprintf(f2, line);

                if (EqualStrings(tag, line))
                {
                    fprintf(f2, "%s=%s\n", name, value);
                    found = TRUE;
                }
            }

            if (!found)
            {
                fprintf(f2, tag);
                fprintf(f2, "%s=%s\n", name, value);
            }
        }
        else
        {
            char line[MAX_INI_STRING_BUFFER_LENGTH];

            for (u32 x = 0; x < li; x++)
            {
                fgets(line, (MAX_INI_STRING_BUFFER_LENGTH - 1), f1);
                fprintf(f2, line);
            }

            fprintf(f2, "%s=%s\n", name, value);

            fgets(line, (MAX_INI_STRING_BUFFER_LENGTH - 1), f1);

            while (!feof(f1) && fgets(line, (MAX_INI_STRING_BUFFER_LENGTH - 1), f1) != NULL)
            {
                fprintf(f2, line);
            }
        }

        fclose(f1);
        fclose(f2);

        DeleteFile1(file);
        MoveFile2(buffer, file);

        return TRUE;
    }

    void IniFileWriteString(IniFile* self, const char* name, const char* value)
    {
        if (IsNullOrEmpty(self->Name))
        {
            LogError("Unable to write a string value to an ini file. File name not set.");
        }

        if (IsNullOrEmpty(self->Section))
        {
            LogError("Unable to write a string value to an ini file. Section name not set.");
        }

        IniFileWriteStringValue(self->Name, self->Section, name, value);
    }
}