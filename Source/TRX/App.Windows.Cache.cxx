#include "App.Windows.hxx"
#include "Logger.hxx"

using namespace Logger;

namespace App::Windows::Cache
{
	AppWindowCacheContainer AppWindowCacheState;

    void AppWindowCacheInitialize1(AppWindowCache* cache, ApplicationWindow* window)
    {
        if (MAX_APPLICATION_WINDOW_CACHE_WINDOW_COUNT <= cache->Count)
        {
            LogError("Unable to register a window. Maximum is %d.", MAX_APPLICATION_WINDOW_CACHE_WINDOW_COUNT);
        }

        cache->Windows[cache->Count] = window;
        cache->Count = cache->Count + 1;
    }

    BOOL AppWindowCacheHandle1(AppWindowCache* cache, const HWND hwnd, const UINT msg, const WPARAM wp, const LPARAM lp)
    {
        for (u32 x = 0; x < cache->Count; x++)
        {
            if (AppWindowCacheHandle2(cache->Windows[x], hwnd, msg, wp, lp)) { return TRUE; }
        }

        return FALSE;
    }

    BOOL AppWindowCacheHandle2(ApplicationWindow* window, const HWND hwnd, const UINT msg, const WPARAM wp, const LPARAM lp)
    {
        if (window->HWND != hwnd) { return FALSE; }

        if (msg == WM_SIZE && !window->IsResizing && lp != NULL)
        {
            ResizeAppWindow2(window);
        }
        else if (msg == WM_ENTERSIZEMOVE)
        {
            window->IsResizing = TRUE;

            ResizeAppWindow1(window);
        }
        else if (msg == WM_EXITSIZEMOVE)
        {
            window->IsResizing = FALSE;

            ResizeAppWindow2(window);
        }

        if (hwnd != *AppWindowState._HWND)
        {
            if (msg != WM_KEYDOWN && msg != WM_CHAR) { return FALSE; }

            SendMessageA(*AppWindowState._HWND, msg, wp, lp);
        }

        return FALSE;
    }
}
