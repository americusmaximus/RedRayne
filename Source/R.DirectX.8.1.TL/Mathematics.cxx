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

#include "Mathematics.Basic.hxx"

namespace Mathematics
{
    void InvertMatrix(Matrix4x4* dest, const Matrix4x4* m)
    {
        auto a2323 = m->M33 * m->M44 - m->M34 * m->M43;
        auto a1323 = m->M32 * m->M44 - m->M34 * m->M42;
        auto a1223 = m->M32 * m->M43 - m->M33 * m->M42;
        auto a0323 = m->M31 * m->M44 - m->M34 * m->M41;
        auto a0223 = m->M31 * m->M43 - m->M33 * m->M41;
        auto a0123 = m->M31 * m->M42 - m->M32 * m->M41;
        auto a2313 = m->M23 * m->M44 - m->M24 * m->M43;
        auto a1313 = m->M22 * m->M44 - m->M24 * m->M42;
        auto a1213 = m->M22 * m->M43 - m->M23 * m->M42;
        auto a2312 = m->M23 * m->M34 - m->M24 * m->M33;
        auto a1312 = m->M22 * m->M34 - m->M24 * m->M32;
        auto a1212 = m->M22 * m->M33 - m->M23 * m->M32;
        auto a0313 = m->M21 * m->M44 - m->M24 * m->M41;
        auto a0213 = m->M21 * m->M43 - m->M23 * m->M41;
        auto a0312 = m->M21 * m->M34 - m->M24 * m->M31;
        auto a0212 = m->M21 * m->M33 - m->M23 * m->M31;
        auto a0113 = m->M21 * m->M42 - m->M22 * m->M41;
        auto a0112 = m->M21 * m->M32 - m->M22 * m->M31;

        auto det = 1.0f / (m->M11 * (m->M22 * a2323 - m->M23 * a1323 + m->M24 * a1223)
            - m->M12 * (m->M21 * a2323 - m->M23 * a0323 + m->M24 * a0223)
            + m->M13 * (m->M21 * a1323 - m->M22 * a0323 + m->M24 * a0123)
            - m->M14 * (m->M21 * a1223 - m->M22 * a0223 + m->M23 * a0123));

        dest->M11 = det * (m->M22 * a2323 - m->M23 * a1323 + m->M24 * a1223);
        dest->M12 = det * -(m->M12 * a2323 - m->M13 * a1323 + m->M14 * a1223);
        dest->M13 = det * (m->M12 * a2313 - m->M13 * a1313 + m->M14 * a1213);
        dest->M14 = det * -(m->M12 * a2312 - m->M13 * a1312 + m->M14 * a1212);
        dest->M21 = det * -(m->M21 * a2323 - m->M23 * a0323 + m->M24 * a0223);
        dest->M22 = det * (m->M11 * a2323 - m->M13 * a0323 + m->M14 * a0223);
        dest->M23 = det * -(m->M11 * a2313 - m->M13 * a0313 + m->M14 * a0213);
        dest->M24 = det * (m->M11 * a2312 - m->M13 * a0312 + m->M14 * a0212);
        dest->M31 = det * (m->M21 * a1323 - m->M22 * a0323 + m->M24 * a0123);
        dest->M32 = det * -(m->M11 * a1323 - m->M12 * a0323 + m->M14 * a0123);
        dest->M33 = det * (m->M11 * a1313 - m->M12 * a0313 + m->M14 * a0113);
        dest->M34 = det * -(m->M11 * a1312 - m->M12 * a0312 + m->M14 * a0112);
        dest->M41 = det * -(m->M21 * a1223 - m->M22 * a0223 + m->M23 * a0123);
        dest->M42 = det * (m->M11 * a1223 - m->M12 * a0223 + m->M13 * a0123);
        dest->M43 = det * -(m->M11 * a1213 - m->M12 * a0213 + m->M13 * a0113);
        dest->M44 = det * (m->M11 * a1212 - m->M12 * a0212 + m->M13 * a0112);
    }

    void MultiplyMatrix(Matrix4x4* dest, const Matrix4x4* m1, const Matrix4x4* m2)
    {
        dest->M11 = m1->M14 * m2->M41 + m1->M11 * m2->M11 + m1->M12 * m2->M21 + m2->M31 * m1->M13;
        dest->M12 = m2->M42 * m1->M14 + m1->M11 * m2->M12 + m1->M12 * m2->M22 + m2->M32 * m1->M13;
        dest->M13 = m2->M43 * m1->M14 + m2->M13 * m1->M11 + m1->M12 * m2->M23 + m2->M33 * m1->M13;
        dest->M14 = m2->M14 * m1->M11 + m2->M34 * m1->M13 + m1->M12 * m2->M24 + m1->M14 * m2->M44;
        dest->M21 = m1->M21 * m2->M11 + m2->M31 * m1->M23 + m1->M24 * m2->M41 + m1->M22 * m2->M21;
        dest->M22 = m2->M32 * m1->M23 + m2->M42 * m1->M24 + m1->M21 * m2->M12 + m2->M22 * m1->M22;
        dest->M23 = m1->M21 * m2->M13 + m2->M33 * m1->M23 + m2->M43 * m1->M24 + m1->M22 * m2->M23;
        dest->M24 = m1->M22 * m2->M24 + m1->M23 * m2->M34 + m1->M24 * m2->M44 + m1->M21 * m2->M14;
        dest->M31 = m1->M31 * m2->M11 + m2->M31 * m1->M33 + m1->M34 * m2->M41 + m1->M32 * m2->M21;
        dest->M32 = m2->M32 * m1->M33 + m2->M42 * m1->M34 + m1->M31 * m2->M12 + m2->M22 * m1->M32;
        dest->M33 = m1->M31 * m2->M13 + m2->M33 * m1->M33 + m2->M43 * m1->M34 + m1->M32 * m2->M23;
        dest->M34 = m1->M32 * m2->M24 + m1->M33 * m2->M34 + m1->M34 * m2->M44 + m1->M31 * m2->M14;
        dest->M41 = m1->M41 * m2->M11 + m2->M31 * m1->M43 + m1->M44 * m2->M41 + m1->M42 * m2->M21;
        dest->M42 = m2->M32 * m1->M43 + m2->M42 * m1->M44 + m1->M41 * m2->M12 + m2->M22 * m1->M42;
        dest->M43 = m1->M41 * m2->M13 + m2->M33 * m1->M43 + m2->M43 * m1->M44 + m1->M42 * m2->M23;
        dest->M44 = m1->M42 * m2->M24 + m1->M43 * m2->M34 + m1->M44 * m2->M44 + m1->M41 * m2->M14;
    }
}