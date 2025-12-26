#pragma once

#include "Basic.hxx"

#define CHUNK_NAME_LENGTH 4

#define RIFF_HEADER_VALUE "RIFF"
#define RIFF_HEADER_VALUE_LENGTH CHUNK_NAME_LENGTH

#define WAVE_HEADER_VALUE "WAVE"
#define WAVE_HEADER_VALUE_LENGTH CHUNK_NAME_LENGTH

#define WAVE_FORMAT_HEADER_VALUE "fmt "
#define WAVE_FORMAT_HEADER_VALUE_LENGTH CHUNK_NAME_LENGTH

#define WAVE_DATA_HEADER_VALUE "data"
#define WAVE_DATA_HEADER_VALUE_LENGTH CHUNK_NAME_LENGTH

namespace Assets::Sounds::Wave
{
    struct WaveChunk
    {
        char Name[CHUNK_NAME_LENGTH];

        u32 Size;
    };

    struct WaveDefinition
    {
        u16 AudioFormat;
        u16 Channels;
        u32 SampleRate;
        u32 ByteRate;
        u16 BlockAlign;
        u16 BitsPerSample;
    };
}