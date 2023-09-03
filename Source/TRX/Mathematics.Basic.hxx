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

namespace Mathematics
{
    template <typename T>
    inline T Clamp(T value, T min, T max)
    {
        if (max < value) { return max; }
        if (value < min) { return min; }

        return value;
    }

    template <typename T>
    inline T Min(T value1, T value2)
    {
        if (value1 < value2) { return value1; }

        return value2;
    }

    template <typename T>
    inline T Max(T value1, T value2)
    {
        if (value1 < value2) { return value2; }

        return value1;
    }

    struct Matrix4x4
    {
        union {
            struct {
                f32 M11, M12, M13, M14;
                f32 M21, M22, M23, M24;
                f32 M31, M32, M33, M34;
                f32 M41, M42, M43, M44;

            };
            f32 M[4][4];
        };
    };

    void InvertMatrix(Matrix4x4*, const Matrix4x4*);
    void MultiplyMatrix(Matrix4x4*, const Matrix4x4*, const Matrix4x4*);
}