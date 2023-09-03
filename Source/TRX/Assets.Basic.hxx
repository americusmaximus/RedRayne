/*
Copyright (c) 2023 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Basic.hxx"

#define TEXTURE_OPTION_NONE 0
#define TEXTURE_OPTION_UNKNOWN2 2
#define TEXTURE_OPTION_IGNORE_ALPHA 4

#define MAX_TEXTURE_NAME_LENGTH 64

#define TEXTURE_DATA_COLOR_BUFFER_TOTAL_SIZE 1024
#define TEXTURE_DATA_COLOR_BUFFER_RGB_SIZE 768
#define TEXTURE_DATA_COLOR_BUFFER_ALPHA_SIZE 256

namespace Assets
{
    enum class TextureFormat : u32
    {
        Palette24Bit = 1,
        OpacityMap8Bit = 2,
        Color32Bit = 3,
        Color16Bit565 = 4,
        Color16Bit4444 = 5,
        PS2Palette24Bit = 6,
        PS2Color32Bit = 7,
        Palette32Bit = 8,
        PS2Palette32Bit = 9,
        PS2Palette4Bit = 10,
        GameCubeCompressed = 11,
        XBoxDXT1 = 12,
        XBoxColor8Bit256Colors = 13,
        XBoxColor8BitPalette32Bit = 14,
        XBoxTrueColor = 15
    };

    struct TextureDefinition
    {
        Assets::TextureFormat Format;

        u32 Width;
        u32 Height;

        u32 Options; // TODO: enumeration, see top of the file?

        u32 MipLevelCount;
    };

    struct TextureData
    {
        union
        {
            /// <summary>
            /// An array with texture color indexes for individual pixels.
            /// </summary>
            u8* Indexes;

            /// <summary>
            /// An array with ARGB colors.
            /// </summary>
            u32* ARGB;
        };

        /// <summary>
        /// Alpha values for some texture formats.
        /// </summary>
        u8* Alphas;

        /// <summary>
        /// An array with texture colors, consists of two sections:
        ///     1. First section -- 768 bytes contain RGB colors, 24 bits per color (8-bit per R, G, and B respectively).
        ///     2. Second section -- 256 bytes either unused, or contain alpha values for 32-bit textures.
        /// Note: TEXTURE_DATA_COLOR_BUFFER_TOTAL_SIZE
        /// </summary>
        u8* Colors;
    };

    struct TextureIndexes
    {
        /// <summary>
        /// Game's internal texture index.
        /// </summary>
        u32 CacheIndex;

        /// <summary>
        /// Renderer's internal texture index.
        /// </summary>
        u32 RendererIndex;

        u32 Unk2; // TODO

        /// <summary>
        /// Game's frame index on which the texture was uploaded to the renderer.
        /// </summary>
        u32 FrameIndex;

        /// <summary>
        /// Selected index used in texture selection.
        /// </summary>
        u32 SelectedIndex;
    };

    struct Texture
    {
        TextureDefinition Definition;

        TextureData Data;

        char Name[MAX_TEXTURE_NAME_LENGTH];

        TextureIndexes Indexes;
    };
}