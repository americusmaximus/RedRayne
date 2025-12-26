#pragma once

#include "Assets.hxx"
#include "IO.Streams.hxx"

#define TEXTURE_VERSION 2

#define MAX_TEXTURE_ERROR_MESSAGE_LENGTH 512

#define MAX_SUPPORTED_TEXTURE_DIMENSION 1024

#define TEXTURE_FILE_EXTENSION ".TEX"
#define TEXTURE_DIRECTORY_NAME "ART"

#define SMALL_TEXTURE_FILE_SIZE 4096
#define SMALL_TEXTURE_DIMENSION_SIZE 64

#define MEDIUM_TEXTURE_FILE_SIZE 16384
#define MEDIUM_TEXTURE_DIMENSION_SIZE 128

#define LARGE_TEXTURE_FILE_SIZE 65536
#define LARGE_TEXTURE_DIMENSION_SIZE 256

// ACT is Adobe Color Table
// Stores 256-color palette data for RAW textures.
#define ACT_TEXTURE_FILE_EXTENSION ".ACT"

// OPA is Opacity Texture
#define OPA_TEXTURE_FILE_EXTENSION ".OPA"

namespace Assets::Textures
{
    struct TextureDescriptor
    {
        u32 Unk; //todo
        s32 TextureCacheIndex; // TODO: name
        char Name[MAX_TEXTURE_NAME_LENGTH];
    };

    struct TextureDescriptorContainer
    {
        TextureDescriptor* _MENU_BACKGROUND_INGAME = (TextureDescriptor*)0x00620d8c;//TODO
        TextureDescriptor* _OB_AR_TARGETFILE = (TextureDescriptor*)0x00620f3c;//TODO
        TextureDescriptor* _OB_DE_TARGETFILE = (TextureDescriptor*)0x00620f84;//TODO
        TextureDescriptor* _OB_STAMP = (TextureDescriptor*)0x00620fcc;//TODO
        TextureDescriptor* _OB_LA_NEWTOWN_MAP = (TextureDescriptor*)0x00620ef4;//TODO
        TextureDescriptor* _EAX_LOGO = (TextureDescriptor*)0x0062105c;//TODO
        TextureDescriptor* _Blood = (TextureDescriptor*)0x0061f9b0;// TODO, default value
        TextureDescriptor* _BloodPool = (TextureDescriptor*)0x0061fa88;// TODO, default value

        TextureDescriptor* _BBWHITE_RAW = (TextureDescriptor*)0x0062806c;//TODO

        s32* _Unknown101 = (s32*)0x0068cdac;//TODO
        BOOL* _Unknown102 = (BOOL*)0x008fd780;//TODO
    };

    extern TextureDescriptorContainer TextureDescriptorState;

    struct TextureCache
    {
        // TODO 396 bytes size
    };

    struct TextureCacheContainer
    {
        BOOL* _Unknown2 = (BOOL*)0x008fd9c4; // TODO, looks unused. remove
        u32* _Count = (u32*)0x008fd990; // TODO
        u32* _CurrentIndex = (u32*)0x008fd9a0; // TODO

        // First available texture index.
        u32* _NextIndex = (u32*)0x008fd994; // TODO

        Assets::Texture** _Textures = (Assets::Texture**)0x008fd99c; // TODO

        u32* _SelectedTextureIndexes = (u32*)0x008fd9a4;// TODO Array of 8

        TextureCache* _TextureCache = (TextureCache*)0x0075ba88;//TODO
        TextureCache** _TextureCachePointer = (TextureCache**)0x00624214;//TODO
    };

    extern TextureCacheContainer TextureCacheState;

    struct TextureContainer
    {
        BOOL* _IsNTSC = (BOOL*)0x008fd788;//TODO
    };

    extern TextureContainer TextureState;

    void LoadTextureDescriptor123(TextureDescriptor* desc, const char* file);
    void LoadTextureDescriptor124(TextureDescriptor* desc, const u32 indx);
    void LoadTextureDescriptor125(TextureDescriptor* desc, const u32 dim, u8* indexes, u8* colors, u8* alphas);
    void LoadTextureDescriptor126(const u32 indx);
    BOOL LoadTexture1(Assets::Texture* tex, const char* name);
    BOOL LoadLegacyTexture(Assets::Texture* tex, const char* name, char* outMessage);
    BOOL LoadTexture2(Assets::Texture* tex, const char* name, IO::Streams::InStreamFile* stream);
    BOOL LoadTexture3(const char* name, IO::Streams::InStreamFile* stream);
    BOOL TextureLoadHeader(Assets::Texture* tex, IO::Streams::InStreamFile* stream);

    struct TextureAsset
    {
        IO::FileDescriptor Descriptor;
        
        void* Format;// TODO type, name

        s32 Width;
        s32 Height;
        u32 BitsPerPixel;

        s32 Unknown2;//TODO

        u32* ARGB;
        u8* Alphas;

        BOOL IsInitialized;

        u32 Unknown32Bit;//TODO
        u32 Unknown8Bit;//TODO

        u8 Colors[TEXTURE_DATA_COLOR_BUFFER_TOTAL_SIZE];
    };

    void TextureAssetInitialize1(TextureAsset* self);
    void TextureAssetInitialize2(TextureAsset* self);
    void TextureAssetSelectName(TextureAsset* self, const char* dir, const char* file);
    void TextureAssetRelease1(TextureAsset* self);
    void* TextureAssetDispose(TextureAsset* self, const Memory::DisposeMode mode);
    BOOL LoadTextureAsset1(TextureAsset* self, char* outMessage);
    BOOL AcquireTextureAssetFormat(TextureAsset* self);
    BOOL IsTextureAssetValid(Assets::Texture* tex, TextureAsset* asset); // TODO name
    BOOL IsTextureAssetHasAlpha(TextureAsset* self);                     // TODO name
}