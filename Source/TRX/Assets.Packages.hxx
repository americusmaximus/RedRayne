#pragma once

#include "IO.Streams.hxx"

#define MAX_ASSET_PACKAGE_EXTENSION_LENGTH 4
#define MAX_ASSET_PACKAGE_HANDLER_COUNT 16

namespace Assets::Packages
{
    struct AssetPackageContainer
    {
        u32* _Count = (u32*)0x0075bf58;//TODO
        char** _Extensions = (char**)0x0075bf5c;// TODO array of 16[4]
        void** _Handlers = (void**)0x0075bf9c;//TODO array of 16
    };

    extern AssetPackageContainer AssetPackageState;

    void RegisterHandler(char ext[MAX_ASSET_PACKAGE_EXTENSION_LENGTH], void* lambda); // TODO proper typedef

    void TextureHandler(void* todo, const char* name, IO::Streams::InStreamFile* stream); // TODO
    void SoundHandler(void* todo, const char* name, IO::Streams::InStreamFile* stream);	  // TODO
}