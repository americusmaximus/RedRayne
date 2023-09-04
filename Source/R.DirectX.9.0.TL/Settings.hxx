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

#define RENDERER_MODULE_OPTIONS_FILE_NAME ".\\system\\render.ini"

#define RENDERER_MODULE_OPTIONS_GRAPHICS_SECTION_NAME "Graphics"

#define RENDERER_MODULE_OPTIONS_GRAPHICS_USE_FIXED_FUNCTION_PIPELINE_NAME "useFixedFunctionPipe"
#define RENDERER_MODULE_OPTIONS_GRAPHICS_SYNC_RETRACE_NAME "syncRetrace"
#define RENDERER_MODULE_OPTIONS_GRAPHICS_RENDER_TEXTURE_COUNT_NAME "renderTextureCount"
#define RENDERER_MODULE_OPTIONS_GRAPHICS_RENDER_TEXTURE_SIZE_NAME "renderTextureSize"
#define RENDERER_MODULE_OPTIONS_GRAPHICS_CUBE_MAP_COUNT_NAME "cubeMapCount"
#define RENDERER_MODULE_OPTIONS_GRAPHICS_CUBE_MAP_SIZE_NAME "cubeMapSize"
#define RENDERER_MODULE_OPTIONS_GRAPHICS_USE_W_BUFFER_NAME "useWBuffer"

namespace Settings
{
    enum class FixedPipelineOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class SyncRetraceOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    enum class BufferOptionsMode : u32
    {
        Off = 0,
        On = 1
    };

    void ReadValue(const char* section, const char* parameter, u32* value);
}