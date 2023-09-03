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

#define GRAPHICS_BITS_PER_PIXEL_8 8
#define GRAPHICS_BITS_PER_PIXEL_16 16
#define GRAPHICS_BITS_PER_PIXEL_24 24
#define GRAPHICS_BITS_PER_PIXEL_32 32

#define MIN_GRAPHICS_RESOLUTION_WIDTH 640
#define MIN_GRAPHICS_RESOLUTION_HEIGHT 480

#define MAX_GRAPHICS_RESOLUTION_WIDTH 7680
#define MAX_GRAPHICS_RESOLUTION_HEIGHT 4320

#define GRAPHICS_COLOR_BLACK 0x00000000
#define GRAPHICS_COLOR_WHITE 0xFFFFFFFF

#define ONE_TWO_HUNDRED_FIFTY_FIFTH (0.003921569f)
#define ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH (0.0039215689f)

#define GRAPHICS_POINT_VERTEX_COUNT 1
#define GRAPHICS_LINE_VERTEX_COUNT 2
#define GRAPHICS_TRIANGLE_VERTEX_COUNT 3
#define GRAPHICS_RECTANGLE_VERTEX_COUNT 4

namespace Graphics
{
    struct f32xRGB { f32 R, G, B; };

    struct u32xRGB { u32 R, G, B; };

    struct f32xARGB { f32 A, R, G, B; };

    struct f32xRGBA { f32 R, G, B, A; };

    struct u32xRGBA { u32 R, G, B, A; };

    struct f32xBGRA { f32 B, G, R, A; };

    struct u32xBGRA { u32 B, G, R, A; };
}