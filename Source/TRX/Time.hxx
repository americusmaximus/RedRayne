#pragma once

#include "Basic.hxx"
#include "Native.Basic.hxx"

namespace Time
{
    struct TimeContainer
    {
        struct
        {
            BOOL* _IsActive = (BOOL*)0x00901ac4; // TODO
        } Timer;

        struct
        {
            struct
            {
                LARGE_INTEGER Frequency; // N/A

                f64 Divisor; // N/A
                f64 Quotient; // N/A
            } Performance;

            LARGE_INTEGER Current; // 0x00901aa0

            struct
            {
                DWORD LowPart; // 0x00901ab0
                LONG HighPart; // 0x00901ab4
            } Previous;

            struct
            {
                DWORD LowPart; // 0x00901aa8
                LONG HighPart; // 0x00901aac

                s32 Value; // N/A
            } Delta;

            s32 Time; // 0x00901ac0
        } Counter;
    };

    extern TimeContainer TimeState;

    u32 AcquireTime(void);
    void InitializeTime(void);
    void ReleaseTime(void);
}