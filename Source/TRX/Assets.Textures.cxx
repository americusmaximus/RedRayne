#include "Assets.Textures.hxx"
#include "Renderer.hxx"
#include "Strings.hxx"
#include "Logger.hxx"
#include "IO.hxx"
#include "Memory.hxx"
#include "App.Options.hxx"

#include <math.h>

#define TEXTURE_PALETTED_ELEMENT_SIZE 4
#define TEXTURE_COLOR_ELEMENT_SIZE 16

#define NTSC_LUMINANCE_RED 0.3f
#define NTSC_LUMINANCE_GREEN 0.58999997f
#define NTSC_LUMINANCE_BLUE 0.11f

using namespace App::Options;
using namespace Memory;
using namespace Logger;
using namespace Strings;
using namespace Renderer;
using namespace IO;
using namespace IO::Path;
using namespace IO::Streams;
using namespace Renderers;

namespace Assets::Textures
{
    TextureContainer TextureState;
    TextureDescriptorContainer TextureDescriptorState;
    TextureCacheContainer TextureCacheState;

    void LoadTextureDescriptor123(TextureDescriptor* desc, const char* file)
    {
        if (IsNullOrEmpty(file))
        {
            LoadTextureDescriptor124(desc, 0);

            return;
        }

        if (*RendererState.Options._AcceleratedGraphicsPort == RendererAcceleratedGraphicsPortMode::Auto)
        {
            u32 dim = 0;
            const auto size = AcquireFileSize(TEXTURE_DIRECTORY_NAME, file);

            switch (size)
            {
            case SMALL_TEXTURE_FILE_SIZE:
            {
                dim = SMALL_TEXTURE_DIMENSION_SIZE;
                break;
            }
            case MEDIUM_TEXTURE_FILE_SIZE:
            {
                dim = MEDIUM_TEXTURE_DIMENSION_SIZE;
                break;
            }
            case LARGE_TEXTURE_FILE_SIZE:
            {
                dim = LARGE_TEXTURE_DIMENSION_SIZE;
                break;
            }
            default:
            {
                LogError("Resolution of texture %s does not match supported dimensions of 64, 128, or 256 pixels.", file);
            }
            }

            auto indexes = (u8*)AllocateMemory(dim * dim);

            if (indexes == NULL)
            {
                LogError("Unable to allocate texture memory.");
            }

            auto colors = (u8*)AllocateMemory(TEXTURE_DATA_COLOR_BUFFER_RGB_SIZE);

            if (colors == NULL)
            {
                LogError("Unable to allocate texture memory.");
            }

            InStreamFile stream;

            InStreamFileInitialize(&stream);

            if (!InStreamFileOwn(&stream, TEXTURE_DIRECTORY_NAME, file, TRUE))
            {
                LogError("Unable to open texture %s.", file);
            }

            InStreamFileReadBlock(&stream, indexes, dim * dim);
            InStreamFileRelease(&stream);

            char name[STANDARD_IO_FILE_NAME_LENGTH];
            SplitPath(file, NULL, NULL, name, NULL);

            char act[STANDARD_IO_FILE_NAME_LENGTH];
            MakePath(act, NULL, NULL, name, ACT_TEXTURE_FILE_EXTENSION);

            char opa[STANDARD_IO_FILE_NAME_LENGTH];
            MakePath(opa, NULL, NULL, name, OPA_TEXTURE_FILE_EXTENSION);

            if (!InStreamFileOwn(&stream, TEXTURE_DIRECTORY_NAME, act, TRUE))
            {
                LogError("Unable to open texture %s.", act);
            }

            InStreamFileReadBlock(&stream, colors, TEXTURE_DATA_COLOR_BUFFER_RGB_SIZE);
            InStreamFileRelease(&stream);

            u8* opacity = NULL;

            if (InStreamFileOwn(&stream, TEXTURE_DIRECTORY_NAME, opa, TRUE))
            {
                opacity = (u8*)AllocateMemory(dim * dim);

                if (opacity == NULL)
                {
                    LogError("Unable to allocate texture memory.");
                }

                InStreamFileReadBlock(&stream, opacity, dim * dim);
                InStreamFileRelease(&stream);
            }

            u8* values = (u8*)AllocateMemory(dim * dim * sizeof(u32));

            if (values == NULL)
            {
                LogError("Unable to allocate texture memory.");
            }

            u8* alphas = NULL;

            if (opacity != NULL)
            {
                alphas = (u8*)AllocateMemory(dim * dim * sizeof(u32));

                if (alphas == NULL)
                {
                    LogError("Unable to allocate texture memory.");
                }
            }

            // DEBUG
            LogError("Not implemented!");// TODO


            // HUGE TODO
            // TODO: conversion
            //for (u32 x = 0; x < dim; x++)
            //{
            //    auto valuerow = &values[x * dim * sizeof(u32)];
            //    auto alpharow = &alphas[x * dim * sizeof(u32)];
            //    auto indexrow = &indexes[x * dim];
            //    auto opacityrow = &opacity[x * dim];
            //
            //    for (u32 xx = 0; xx < dim; xx++)
            //    {
            //
            //        if (opacity != NULL)
            //        {
            //
            //        }
            //    }
            //}

            ReleaseMemory1(indexes);

            if (opacity != NULL)
            {
                ReleaseMemory1(opacity);
            }

            LoadTextureDescriptor125(desc, dim * 2, values, colors, alphas);
            InStreamFileDispose(&stream, DisposeMode::None);

            return;
        }

        strcpy(desc->Name, file);

        LoadTextureDescriptor124(desc, 0);
    }

    void LoadTextureDescriptor125(TextureDescriptor* desc, const u32 dim, u8* indexes, u8* colors, u8* alphas)
    {
        if (*TextureDescriptorState._Unknown101 != 0 && IsNullOrEmpty(desc->Name)) { return; }

        LogMessage(alphas == NULL
            ? "[TEXTURE] Texture %s is %d x %d 8-bit.\n"
            : "[TEXTURE] Texture %s is %d x %d 8-bit with alpha.\n", desc->Name, dim, dim);

        const auto format = alphas == NULL ? TextureFormat::Palette24Bit : TextureFormat::OpacityMap8Bit;

        auto tex = FindRendererTexture1(desc);

        if (tex == NULL)
        {
            desc->TextureCacheIndex = AcquireAvailableRendererTextureIndex();
            tex = TextureCacheState._Textures[desc->TextureCacheIndex];
        }

        if (format != tex->Definition.Format || tex->Definition.Options != 0
            || dim != tex->Definition.Width || dim != tex->Definition.Height)
        {
            ReleaseRendererTexture2(tex);

            tex->Definition.Width = dim;
            tex->Definition.Height = dim;
            tex->Definition.Format = format;
            tex->Definition.Options = 0;

            strcpy(tex->Name, desc->Name);
        }

        ReleaseRendererTexture3(tex);

        tex->Data.Indexes = indexes;
        tex->Data.Colors = colors;
        tex->Data.Alphas = alphas;

        UploadRendererTexture(tex);
        LoadTextureDescriptor124(desc, 0);
    }

    typedef const void(CDECLAPI* DAT_008FD78C) (TextureDescriptor* desc);//TODO
    DAT_008FD78C DAT_008fd78c = (DAT_008FD78C)0x008fd78c;//TODO

    typedef const void(CDECLAPI* TEXTUREUNKNOWN321) (Texture* tex, const u32 indx);//TODO
    TEXTUREUNKNOWN321 TextureUnknown321 = (TEXTUREUNKNOWN321)0x005a5190;//TODO

    void LoadTextureDescriptor126(const u32 indx)
    {
        LoadTextureDescriptor124(TextureDescriptorState._BBWHITE_RAW, indx);
    }

    void LoadTextureDescriptor124(TextureDescriptor* desc, const u32 indx)
    {
        if (*TextureDescriptorState._Unknown101 == 0)
        {
            if (IsNullOrEmpty(desc->Name))
            {
                LoadTextureDescriptor126(4);
            }
            else if (*RendererState._IsActive || indx == 0)
            {
                auto tex = FindRendererTexture1(desc);

                if (tex == NULL)
                {
                    const auto ti = AcquireAvailableRendererTextureIndex();

                    tex = &(*TextureCacheState._Textures)[ti];

                    if (!LoadTexture1(tex, desc->Name))
                    {
                        char buffer[MAX_TEXTURE_ERROR_MESSAGE_LENGTH];

                        if (!LoadLegacyTexture(tex, desc->Name, buffer))
                        {
                            ReleaseRendererTexture2(tex);

                            LogMessage("[ERROR] [TEXTURE] Unable to load texture %s. %s\n", desc->Name, buffer);

                            if (*TextureDescriptorState._Unknown102 || DAT_008fd78c == NULL)
                            {
                                LogError("Unable to load texture %s. %s.", desc->Name, buffer);
                            }

                            *TextureDescriptorState._Unknown102 = TRUE;
                            DAT_008fd78c(desc);//TODO it looks like it is always null.
                            *TextureDescriptorState._Unknown102 = FALSE;
                        }

                        UploadRendererTexture(tex);
                    }

                    desc->TextureCacheIndex = ti;
                }

                TextureCacheState._SelectedTextureIndexes[indx] = desc->TextureCacheIndex;

                TextureUnknown321(tex, indx);
            }
        }
    }

    BOOL LoadTexture1(Texture* tex, const char* name)
    {
        ReleaseRendererTexture2(tex);

        char file[STANDARD_IO_FILE_NAME_LENGTH];

        strcpy(file, name);

        ChangeFileExtension(file, TEXTURE_FILE_EXTENSION);

        InStreamFile stream;

        InStreamFileInitialize(&stream);

        if (InStreamFileOwn(&stream, TEXTURE_DIRECTORY_NAME, file, TRUE))
        {
            const auto result = LoadTexture2(tex, name, &stream);

            InStreamFileDispose(&stream, DisposeMode::None);

            return result;
        }

        InStreamFileDispose(&stream, DisposeMode::None);

        return FALSE;
    }

    BOOL LoadTexture2(Texture* tex, const char* name, InStreamFile* stream)
    {
        if (!TextureLoadHeader(tex, stream))
        {
            ReleaseRendererTexture2(tex);

            LogMessage("[ERROR] [TEXTURE] Optimized version of %s texture is legacy or corrupted.\n", name);

            return FALSE;
        }

        auto mips = FALSE;

        if (0 < tex->Definition.MipLevelCount && *AppOptionsState.Graphics._TextureDetailMode == ApplicationTextureDetailOptionsMode::Normal)
        {
            mips = TRUE;

            tex->Definition.MipLevelCount = tex->Definition.MipLevelCount - 1;
            tex->Definition.Width = tex->Definition.Width / 2;
            tex->Definition.Height = tex->Definition.Height / 2;
        }

        LogMessage("[TEXTURE] Loading texture %s %d x %d ", name, tex->Definition.Width, tex->Definition.Height);

        switch (tex->Definition.Format)
        {
        case TextureFormat::Palette24Bit:
        {
            LogMessage("paletted (24-bit palette)"); break;
        }
        case TextureFormat::OpacityMap8Bit:
        {
            LogMessage("8-bit with opacity map"); break;
        }
        case TextureFormat::Color32Bit:
        {
            LogMessage("32-bit"); break;
        }
        case TextureFormat::Color16Bit565:
        {
            LogMessage("16-bit (565)"); break;
        }
        case TextureFormat::Color16Bit4444:
        {
            LogMessage("16-bit (4444)"); break;
        }
        case TextureFormat::PS2Palette24Bit:
        {
            LogMessage("Play Station 2 paletted (24-bit palette)"); break;
        }
        case TextureFormat::PS2Color32Bit:
        {
            LogMessage("Play Station 2 32-bit"); break;
        }
        case TextureFormat::PS2Palette32Bit:
        {
            LogMessage("Play Station 2 paletted (32-bit palette)"); break;
        }
        case TextureFormat::PS2Palette4Bit:
        {
            LogMessage("Play Station 2 paletted (4-bit palette)"); break;
        }
        case TextureFormat::GameCubeCompressed:
        {
            LogMessage("GameCube compressed"); break;
        }
        case TextureFormat::XBoxDXT1:
        {
            LogMessage("XBOX DXT1"); break;
        }
        case TextureFormat::XBoxColor8Bit256Colors:
        {
            LogMessage("XBOX 8-bit image (256 colors)"); break;
        }
        case TextureFormat::XBoxColor8BitPalette32Bit:
        {
            LogMessage("XBOX 8-bit image (32-bit palette)"); break;
        }
        case TextureFormat::XBoxTrueColor:
        {
            LogMessage("XBOX true color image"); break;
        }
        default:
        {
            LogMessage("unknown format %d", tex->Definition.Format); break;
        }
        }

        if (tex->Definition.MipLevelCount < 1)
        {
            LogMessage(".\n");
        }
        else
        {
            LogMessage(" with %d mip level(s).\n", tex->Definition.MipLevelCount);
        }

        strcpy(tex->Name, name);

        AllocateRendererTexture(tex);

        switch (tex->Definition.Format)
        {
        case TextureFormat::Palette24Bit:
        case TextureFormat::OpacityMap8Bit:
        case TextureFormat::Palette32Bit:
        {
            tex->Data.Colors = (u8*)AllocateMemory(TEXTURE_DATA_COLOR_BUFFER_TOTAL_SIZE);

            if (tex->Data.Colors == NULL)
            {
                LogError("Unable to allocate texture memory.");
            }

            if (stream->Self->ReadBlock(stream, tex->Data.Colors, TEXTURE_DATA_COLOR_BUFFER_RGB_SIZE) != TEXTURE_DATA_COLOR_BUFFER_RGB_SIZE)
            {
                ReleaseRendererTexture2(tex);

                LogMessage("[ERROR] [TEXTURE] Optimized version of %s is legacy or corrupted.\n", name);
            }

            if (tex->Definition.Format == TextureFormat::Palette32Bit)
            {
                if (stream->Self->ReadBlock(stream, &tex->Data.Colors[TEXTURE_DATA_COLOR_BUFFER_RGB_SIZE], TEXTURE_DATA_COLOR_BUFFER_ALPHA_SIZE) != TEXTURE_DATA_COLOR_BUFFER_ALPHA_SIZE)
                {
                    ReleaseRendererTexture2(tex);

                    LogMessage("[ERROR] [TEXTURE] Optimized version of %s is legacy or corrupted.\n", name);

                    return FALSE;
                }
            }
            else
            {
                memset(&tex->Data.Colors[TEXTURE_DATA_COLOR_BUFFER_RGB_SIZE], 0xff, TEXTURE_DATA_COLOR_BUFFER_ALPHA_SIZE);
            }

            if (mips)
            {
                stream->Self->Seek(stream, tex->Definition.Width * tex->Definition.Height * TEXTURE_PALETTED_ELEMENT_SIZE, StreamSeek::Current);
            }

            u32 size = 0;

            for (u32 x = 0; x <= tex->Definition.MipLevelCount; x++)
            {
                const auto divisor = (x & 0x1f);
                size = size + (tex->Definition.Width >> divisor) * (tex->Definition.Height >> divisor);
            }

            tex->Data.Indexes = (u8*)AllocateMemory(size);

            if (tex->Data.Indexes == NULL)
            {
                LogError("Unable to allocate texture memory.");
            }

            if (stream->Self->ReadBlock(stream, tex->Data.Indexes, size) != size)
            {
                ReleaseRendererTexture2(tex);

                LogMessage("[ERROR] [TEXTURE] Optimized version of %s is legacy or corrupted.\n", name);

                return FALSE;
            }

            if (tex->Definition.Format != TextureFormat::OpacityMap8Bit)
            {
                UploadRendererTexture(tex);

                return TRUE;
            }

            if (mips)
            {
                stream->Self->Seek(stream, tex->Definition.Width * tex->Definition.Height * TEXTURE_PALETTED_ELEMENT_SIZE, StreamSeek::Current);
            }

            tex->Data.Alphas = (u8*)AllocateMemory(size);

            if (tex->Data.Alphas == NULL)
            {
                LogError("Unable to allocate texture memory.");
            }

            if (stream->Self->ReadBlock(stream, tex->Data.Alphas, size) != size)
            {
                ReleaseRendererTexture2(tex);

                LogMessage("[ERROR] [TEXTURE] Optimized version of %s is legacy or corrupted.\n", name);

                return FALSE;
            }

            UploadRendererTexture(tex);

            return TRUE;
        }
        case TextureFormat::Color32Bit:
        {
            if (mips)
            {
                stream->Self->Seek(stream, tex->Definition.Width * tex->Definition.Height * TEXTURE_COLOR_ELEMENT_SIZE, StreamSeek::Current);
            }

            u32 size = 0;

            for (u32 x = 0; x <= tex->Definition.MipLevelCount; x++)
            {
                const auto divisor = (x & 0x1f);
                size = size + (tex->Definition.Width >> divisor) * (tex->Definition.Height >> divisor) * sizeof(u32);
            }

            tex->Data.ARGB = (u32*)AllocateMemory(size);

            if (tex->Data.ARGB == NULL)
            {
                LogError("Unable to allocate texture memory.");
            }

            if (stream->Self->ReadBlock(stream, tex->Data.ARGB, size) != size)
            {
                ReleaseRendererTexture2(tex);

                LogMessage("[ERROR] [TEXTURE] Optimized version of texture %s is legacy or corrupted.\n", name);

                return FALSE;
            }

            UploadRendererTexture(tex);

            return TRUE;
        }
        default:
        {
            LogMessage("[WARNING] [TEXTURE] Texture format %d for texture %s is not supported on this platform.\n", tex->Definition.Format, name);

            ReleaseRendererTexture2(tex);

            return TRUE;
        }
        }

        ReleaseRendererTexture2(tex);

        LogMessage("[ERROR] [TEXTURE] Optimized version of texture %s is legacy or corrupted.\n", name);

        return FALSE;
    }

    BOOL TextureLoadHeader(Texture* tex, InStreamFile* stream)
    {
        u32 version = 0;

        if (stream->Self->ReadBlock(stream, &version, sizeof(u32)) == sizeof(u32))
        {
            if (version == TEXTURE_VERSION)
            {
                if (stream->Self->ReadBlock(stream, &tex->Definition, sizeof(TextureDefinition)) == sizeof(TextureDefinition)) { return TRUE; }
            }
        }

        return FALSE;
    }

    BOOL LoadTexture3(const char* name, InStreamFile* stream)
    {
        TextureDescriptor desc;

        ZeroMemory(&desc, sizeof(desc));

        strcpy(desc.Name, name);

        auto tex = FindRendererTexture1(&desc);

        if (tex == NULL)
        {
            desc.TextureCacheIndex = AcquireAvailableRendererTextureIndex();

            tex = TextureCacheState._Textures[desc.TextureCacheIndex];
        }

        return LoadTexture2(tex, name, stream);
    }

    BOOL LoadLegacyTexture(Texture* tex, const char* name, char* outMessage)
    {
        LogMessage("[TEXTURE] Loading %s ", name);

        if ((MAX_TEXTURE_NAME_LENGTH - 1) < strlen(name))
        {
            LogError("Texture name %s is too long.", name);
        }

        ReleaseRendererTexture2(tex);

        TextureAsset asset;
        char buffer[MAX_TEXTURE_ERROR_MESSAGE_LENGTH];

        TextureAssetInitialize1(&asset);
        TextureAssetSelectName(&asset, TEXTURE_DIRECTORY_NAME, name);

        if (!LoadTextureAsset1(&asset, buffer))
        {
            LogMessage("[TEXTURE] Unable to load texture: %s.\n", buffer);

            TextureAssetDispose(&asset, DisposeMode::None);

            return FALSE;
        }

        if (!IsTextureAssetValid(tex, &asset))
        {
            sprintf(outMessage, "Invalid texture dimensions %d x %d.", asset.Width, asset.Height);

            LogMessage("[TEXTURE] Unable to load texture: %s.\n", name, buffer);

            TextureAssetDispose(&asset, DisposeMode::None);

            return FALSE;
        }

        tex->Definition.Width = asset.Width;
        tex->Definition.Height = asset.Height;

        switch (asset.BitsPerPixel)
        {
        case GRAPHICS_BITS_PER_PIXEL_8:
        {
            tex->Data.Colors = (u8*)AllocateMemory(TEXTURE_DATA_COLOR_BUFFER_RGB_SIZE);

            if (tex->Data.Colors == NULL)
            {
                LogError("Unable to allocate texture memory.");
            }

            CopyMemory(tex->Data.Colors, asset.Colors, TEXTURE_DATA_COLOR_BUFFER_RGB_SIZE);

            if (asset.Unknown2 == 1 && asset.Alphas != NULL)
            {
                tex->Definition.Format = TextureFormat::OpacityMap8Bit;
                tex->Data.Alphas = asset.Alphas;

                asset.Alphas = NULL;

                LogMessage("%d x %d 8-bit with alpha.\n", tex->Definition.Width, tex->Definition.Height);
            }
            else
            {
                tex->Definition.Format = TextureFormat::Palette24Bit;

                LogMessage("%d x %d 8-bit.\n", tex->Definition.Width, tex->Definition.Height);
            }

            if (*TextureState._IsNTSC)
            {
                // NOTE:
                // Luminance: Our human eye does not perceive all colors as equally the same brightness.
                // Of the three, we see Green as brightest. Red is next, and Blue is least bright (to us).
                // The perceived brightness of color (luminosity) is judged to be: Green 0.59, Red 0.3, Blue 0.11.
                // That's the NTSC television formula for converting RGB scenes to grayscale TV:
                // RGB Luminance = 0.3 R + 0.59 G + 0.11 B (the three coefficients adding to 1.0 when combined).
                // Luminance is the computed equivalent brightness of color as seen in grayscale (or by B&W film) but it is NOT actual real RGB data.

                for (u32 x = 0; x < TEXTURE_DATA_COLOR_BUFFER_RGB_SIZE; x = x + 3)
                {
                    const auto r = (f32)tex->Data.Colors[x + 0] * NTSC_LUMINANCE_RED;
                    const auto g = (f32)tex->Data.Colors[x + 1] * NTSC_LUMINANCE_GREEN;
                    const auto b = (f32)tex->Data.Colors[x + 2] * NTSC_LUMINANCE_BLUE;

                    const auto luminance = (u8)roundf(r + g + b);

                    tex->Data.Colors[x + 0] = luminance;
                    tex->Data.Colors[x + 1] = luminance;
                    tex->Data.Colors[x + 2] = luminance;
                }
            }

            break;
        }
        case GRAPHICS_BITS_PER_PIXEL_32:
        {
            tex->Definition.Format = TextureFormat::Color32Bit;

            if (IsTextureAssetHasAlpha(&asset))
            {
                LogMessage("%d x %d 32-bit.\n", tex->Definition.Width, tex->Definition.Height);
            }
            else
            {
                tex->Definition.Options = tex->Definition.Options | TEXTURE_OPTION_IGNORE_ALPHA;

                LogMessage("%d x %d 32-bit (ignore alpha).\n", tex->Definition.Width, tex->Definition.Height);
            }

            break;
        }
        default:
        {
            LogError("Invalid texture bit depth of %d.", asset.BitsPerPixel);
        }
        }

        tex->Data.ARGB = asset.ARGB;

        asset.ARGB = NULL;

        TextureAssetDispose(&asset, DisposeMode::None);

        return TRUE;
    }

    BOOL IsTextureAssetHasAlpha(TextureAsset* self)
    {
        switch (self->BitsPerPixel)
        {
        case GRAPHICS_BITS_PER_PIXEL_8:
        {
            if (self->Alphas == NULL) { return FALSE; }

            for (u32 x = 0; x < self->Height; x++)
            {
                for (u32 xx = 0; xx < self->Width; xx++)
                {
                    if (self->Alphas[x * self->Height + xx] != 0xff) { return TRUE; }
                }
            }

            break;
        }
        case GRAPHICS_BITS_PER_PIXEL_32:
        {
            for (u32 x = 0; x < self->Height; x++)
            {
                for (u32 xx = 0; xx < self->Width; xx++)
                {
                    if ((self->ARGB[x * self->Height + xx] >> 24) != 0xff) { return TRUE; }
                }
            }

            break;
        }
        }

        return FALSE;
    }



    void TextureAssetInitialize1(TextureAsset* self)
    {
        TextureAssetInitialize2(self);
    }

    void TextureAssetInitialize2(TextureAsset* self)
    {
        ZeroMemory(&self->Descriptor, sizeof(FileDescriptor));

        self->Format = NULL;

        self->Width = 0;
        self->Height = 0;
        self->BitsPerPixel = 0;

        self->Unknown2 = 0;

        self->ARGB = NULL;
        self->Alphas = NULL;

        self->IsInitialized = FALSE;

        self->Unknown32Bit = 0;
        self->Unknown8Bit = 0;
    }

    void TextureAssetSelectName(TextureAsset* self, const char* dir, const char* file)
    {
        TextureAssetRelease1(self);

        TextureAssetInitialize2(self);

        CombinePath(self->Descriptor.FileName, dir, file);
    }

    void TextureAssetRelease1(TextureAsset* self)
    {
        if (self->IsInitialized)
        {
            if (self->ARGB != NULL)
            {
                ReleaseMemory1(self->ARGB);
            }

            if (self->Alphas != NULL)
            {
                ReleaseMemory1(self->Alphas);
            }

            self->IsInitialized = 0;
        }

        self->Alphas = NULL;
        self->ARGB = NULL;
    }

    void* TextureAssetDispose(TextureAsset* self, const DisposeMode mode)
    {
        TextureAssetRelease1(self);

        TextureAssetInitialize2(self);

        return self;
    }

    typedef const BOOL(CDECLAPI* FUN_004A3D30) (TextureAsset* self, InStreamFile* stream, char* outMessage);//TODO
    FUN_004A3D30 FUN_004a3d30 = (FUN_004A3D30)0x004a3d30;//TODO

    BOOL LoadTextureAsset1(TextureAsset* self, char* outMessage)
    {
        if (IsNullOrEmpty(self->Descriptor.AbsoluteFileName))
        {
            if (!IsFileExists(&self->Descriptor, outMessage)) { return FALSE; }
        }

        if (self->Format == NULL && !AcquireTextureAssetFormat(self)) // TODO
        {
            strcpy(outMessage, "Unknown texture file format.");

            return FALSE;
        }

        InStreamFile stream;

        InStreamFileInitialize(&stream);

        if (!InStreamFileOpenFile(&stream, &self->Descriptor, TRUE))
        {
            strcpy(outMessage, "Unable to open texture file.");

            InStreamFileDispose(&stream, DisposeMode::None);

            return FALSE;
        }

        const auto result = FUN_004a3d30(self, &stream, outMessage);

        InStreamFileDispose(&stream, DisposeMode::None);

        return result;
    }

    typedef void* (CDECLAPI* FUN_004A47D0) (const char* ext);//TODO
    FUN_004A47D0 FUN_004a47d0 = (FUN_004A47D0)0x004a3970;//TODO

    BOOL AcquireTextureAssetFormat(TextureAsset* self)
    {
        char ext[STANDARD_IO_EXTENSION_NAME_LENGTH];

        SplitPath(self->Descriptor.FileName, NULL, NULL, NULL, ext);

        self->Format = FUN_004a47d0(ext); // TODO

        return self->Format != NULL;
    }

    BOOL IsTextureAssetValid(Texture* tex, TextureAsset* asset)
    {
        if (0 < asset->Width && asset->Width < (MAX_SUPPORTED_TEXTURE_DIMENSION + 1)
            && 0 < asset->Height && asset->Height < (MAX_SUPPORTED_TEXTURE_DIMENSION + 1))
        {
            if (((asset->Width - 1) & asset->Width) == 0 && ((asset->Height - 1) & asset->Height) == 0)
            {
                return TRUE;
            }
        }

        return FALSE;
    }
}