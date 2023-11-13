#include "App.hxx"
#include "App.Windows.hxx"
#include "Time.hxx"

#include <math.h>
#include <timeapi.h>

#define TIMER_PERIOD_LENGTH 1

#define TIMER_PERFORMANCE_COUNTER_MULTIPLIER (4294967296.0)

#define TIMER_PERFORMANCE_COUNTER_DIVIDEND (1179648.0)

#define TIMER_ITERATION_COUNT 5
#define TIMER_ITERATION_STEP_COUNT 100000000
#define TIMER_ITERATION_STEP_VALUE 99
#define TIMER_ITERATION_MULTIPLIER 117964800

#define TIMER_VALUE_MULTIPLIER 1180

using namespace App;
using namespace App::Windows;

namespace Time
{
    TimeContainer TimeState;

    // 0x005cc140
    void InitializeTime(void)
    {
        if (QueryPerformanceFrequency(&TimeState.Counter.Performance.Frequency))
        {
            TimeState.Counter.Performance.Divisor =
                TimeState.Counter.Performance.Frequency.LowPart +
                TimeState.Counter.Performance.Frequency.HighPart * TIMER_PERFORMANCE_COUNTER_MULTIPLIER;

            TimeState.Counter.Performance.Quotient = TIMER_PERFORMANCE_COUNTER_DIVIDEND / TimeState.Counter.Performance.Divisor;

            QueryPerformanceCounter(&TimeState.Counter.Current);
        }
        else
        {
            *TimeState.Timer._IsActive = TRUE;

            if (timeBeginPeriod(TIMER_PERIOD_LENGTH) != TIMERR_NOERROR)
            {
                if (MessageBoxA(*AppWindowState._HWND, "Unable to initialize timer.", AppWindowState.WindowName, MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
                {
                    *AppState._IsQuit = TRUE;
                }
            }
        }
    }

    // 0x005cc380
    u32 AcquireTime(void)
    {
        if (*TimeState.Timer._IsActive) { return (u32)(timeGetTime() * TIMER_VALUE_MULTIPLIER); }

        TimeState.Counter.Previous.LowPart = TimeState.Counter.Current.LowPart;
        TimeState.Counter.Previous.HighPart = TimeState.Counter.Current.HighPart;

        QueryPerformanceCounter(&TimeState.Counter.Current);

        const auto lp = TimeState.Counter.Current.LowPart - TimeState.Counter.Previous.LowPart;

        const auto hp = (TimeState.Counter.Current.HighPart - TimeState.Counter.Previous.HighPart)
            - (s32)(TimeState.Counter.Current.LowPart < TimeState.Counter.Previous.LowPart);

        const auto dh = TimeState.Counter.Current.HighPart - TimeState.Counter.Previous.HighPart;
        const auto dhc = dh - (s32)(TimeState.Counter.Current.LowPart < TimeState.Counter.Previous.LowPart);

        if ((dh != dhc) == hp < 0)
        {
            TimeState.Counter.Delta.Value = (s32)round((f64)((((s64)hp) << 32) + lp) * TimeState.Counter.Performance.Quotient);

            TimeState.Counter.Delta.LowPart = lp;
            TimeState.Counter.Delta.HighPart = hp;

            if (0 < TimeState.Counter.Delta.Value)
            {
                TimeState.Counter.Time = TimeState.Counter.Time + TimeState.Counter.Delta.Value;
            }
        }

        return TimeState.Counter.Time;
    }

    // 0x005cc370
    void ReleaseTime(void)
    {
        timeEndPeriod(TIMER_PERIOD_LENGTH);
    }
}