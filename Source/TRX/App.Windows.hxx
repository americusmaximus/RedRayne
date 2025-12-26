#pragma once

#include "App.hxx"
#include "Native.Basic.hxx"
#include "Renderers.Basic.hxx"

#define APP_WINDOW_NAME APP_NAME
#define MAX_APPLICATION_WINDOW_CACHE_WINDOW_COUNT 32

namespace App::Windows
{
    enum InternalWindowMode : u8 // TODO: refactor this out!
    {
        Screen = 0,
        Window = 1
    };

    struct ApplicationWindow
    {
        HWND HWND;
        struct
        {
            void* DepthBuffer;// TODO
            void* AlignedDepthBuffer;// TODO
            void* FrameBuffer;// TODO
        } Memory;
        HDC HDC;
        HBITMAP HBITMAP;
        HGDIOBJ HGDIOBJ;
        struct
        {
            BITMAPINFOHEADER Header;
            // NOTE: If the BitCount equals 16 or 32,
            // the optimal color palette starts immediately following the three doubleword masks.
            // This way of implementation is work around for manual BITMAPINFO creation.
            u32 Masks[3];
        } BITMAPINFO;
        s32 Unknown101[253];//TODO
        void* DIB;
        u32 WindowWidth;
        u32 WindowHeight;
        u32 WindowBitsPerPixel;
        u32 ScreenWidth;
        u32 ScreenHeight;
        u32 UnkActualWidth;// TODO
        u32 UnkActualHeight;//TODO
        u32 ScreenBitsPerPixel;
        u32 X;
        u32 Y;
        BOOL IsResizing;
        Renderers::RendererWindowMode Mode;
        u32 WindowIndex;
    };

    struct ApplicationWindowContainer
    {
        HWND* _HWND = (HWND*)0x00903368; // TODO
        const char* ClassName = APP_WINDOW_NAME;// (char**)0x006288f8; // TODO
        const char* WindowName = APP_WINDOW_NAME;// (char**)0x006288fc; //TODO
        HINSTANCE* _Instance = (HINSTANCE*)0x0090336c;//TODO

        ApplicationWindow* _ApplicationWindow = (ApplicationWindow*)0x00903d00;//TODO, also, name?
        ApplicationWindow** _ApplicationWindowPointer = (ApplicationWindow**)0x00628908;//TODO, also, name?

        struct
        {
            s32* _IsActive = (s32*)0x00903cf4;//TODO

            // NOTE: One of MCI_NOTIFY_ABORTED, MCI_NOTIFY_FAILURE, MCI_NOTIFY_SUCCESSFUL, MCI_NOTIFY_SUPERSEDED.
            s32* _Result = (s32*)0x00903cf8;//TODO

            // NOTE: The identifier of the MCI device that sent the message. 
            s32* _Device = (s32*)0x00903cfc;//TODO
        } MCI;

        s32 PreviousActiveState = -1;// = (s32*)0x00628910;//TODO, default value -1
        s32 ActiveState = -1;// = (s32*)0x00628900;//TODO, default value -1
        s32* _NextActiveState = (s32*)0x00903374;//TODO
        BOOL* _ActiveMode = (BOOL*)0x00628904;//TODO default value
    };

    extern ApplicationWindowContainer AppWindowState;

    struct InitializeApplicationWindowArguments
    {
        DWORD StyleEx;
        DWORD Style;
        const char* Class;
        const char* Name;
        s32 X;
        s32 Y;
        s32 Width;
        s32 Height;
        HMENU Menu;
        HWND Parent;
        HINSTANCE Instance;
        Renderers::RendererWindowMode Mode;
    };

    void CreateAppWindow1(void);
    HWND CreateAppWindow2(ApplicationWindow* window, const InitializeApplicationWindowArguments* args);

    void ReleaseAppWindowDeviceContext(ApplicationWindow* window);


    void ResizeAppWindow1(ApplicationWindow* window);
    void ResizeAppWindow2(ApplicationWindow* window);

    void ReleaseAppWindowMemory(ApplicationWindow* window);

    void ReleaseAppWindow1(void);
    void ReleaseAppWindow2(ApplicationWindow* window);
    
    void SelectAppWindowResolution1(ApplicationWindow* window, const u32 width, const u32 height, const u32 bpp);
    BOOL SelectAppWindowResolution2(const u32 width, const u32 height, const u32 bpp); // TODO return value unneeded
    void SelectAppWindowResolution3(ApplicationWindow* window, const u32 width, const u32 height, const u32 bpp);

    void CreateAppWindowBitMap(ApplicationWindow* window);
    void CreateAppWindowPalette1(ApplicationWindow* window);
    void CreateAppWindowPalette2(const u32 redMask, const u32 greenMask, const u32 blueMask);

    LRESULT AppWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

    void VideoPaintAppWindow2(void);
    void VideoPaintAppWindow1(ApplicationWindow* window);

    void SelectAppWindowMode1(ApplicationWindow* window, const InternalWindowMode mode);
    void SelectAppWindowMode2(InternalWindowMode mode);

    void HandleAppWindowMessages();

    void SwapWindow1(void);
    void SwapWindow2(void);
}

namespace App::Windows::Cache
{
    struct AppWindowCache
    {
        s32 Count;
        ApplicationWindow* Windows[MAX_APPLICATION_WINDOW_CACHE_WINDOW_COUNT];
    };

    struct AppWindowCacheContainer
    {
        AppWindowCache* _WindowCache = (AppWindowCache*)0x00901a18;//TODO
        AppWindowCache** _WindowCachePointer = (AppWindowCache**)0x006288d4;//TODO
    };

    extern AppWindowCacheContainer AppWindowCacheState;

    void AppWindowCacheInitialize1(AppWindowCache* cache, ApplicationWindow* window);
    BOOL AppWindowCacheHandle1(AppWindowCache* cache, const HWND hwnd, const UINT msg, const WPARAM wp, const LPARAM lp);
    BOOL AppWindowCacheHandle2(ApplicationWindow* window, const HWND hwnd, const UINT msg, const WPARAM wp, const LPARAM lp);
}