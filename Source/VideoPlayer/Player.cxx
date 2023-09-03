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

#include "App.Resources.hxx"
#include "Player.hxx"

#include <dshow.h>
#include <shellapi.h>

#define PLAYER_WINDOW_CLASS_NAME "Video Player"
#define PLAYER_WINDOW_TITLE_NAME "Video Player"

#define PLAYER_WINDOW_HEIGHT 480
#define PLAYER_WINDOW_WIDTH 640

#define COM_OBJECT_CHECK(result) \
if (FAILED(result))              \
{                                \
    ReleaseClasses();            \
    return result;               \
}

struct State
{
    HWND HWND;
    BOOL IsRunning;

    IGraphBuilder* GraphBuilder;
    IMediaControl* MediaControl;
    IVideoWindow* VideoWindow;
    IMediaEventEx* MediaEvent;

    IBaseFilter* VideoFilter;
    IBaseFilter* VideoSplitter;
    IBaseFilter* VideoDecoder;
    IBaseFilter* VideoRenderer;

    IBaseFilter* AudioFilter;
    IBaseFilter* AudioSplitter;
    IBaseFilter* AudioDecoder;
    IBaseFilter* AudioRenderer;
} State;

s32 Run(const HINSTANCE instance)
{
    s32 count;
    auto arguments = CommandLineToArgvW(GetCommandLineW(), &count);

    if (count < 2 || arguments == NULL)
    {
        MessageBoxA(NULL, "Cutscene is an application that plays a movie.\r\nPlease provide a valid filename(s) as the command line arguments.\r\nUsage: <name>.exe <filename> <optional-filename>", PLAYER_WINDOW_TITLE_NAME, MB_ICONERROR | MB_OK);
        return EXIT_FAILURE;
    }

    SHOWCURSOR(0);

    if (InitializeWindow(instance) != 0) { return EXIT_FAILURE; }

    auto result = count == 2
        ? Execute(arguments[1], arguments[1])
        : Execute(arguments[1], arguments[2]);

    if (State.VideoWindow != NULL)
    {
        DestroyWindow(State.HWND);
    }

    UnregisterClassA(PLAYER_WINDOW_CLASS_NAME, instance);
    LocalFree(arguments);

    return result;
}

s32 Execute(const LPCWSTR video, const LPCWSTR audio)
{
    auto result = CoInitialize(NULL);

    if (SUCCEEDED(result))
    {
        result = InitializeClasses();

        if (SUCCEEDED(result))
        {
            result = InitializePlayer(video, audio);

            if (SUCCEEDED(result))
            {
                result = InitializeVideoWindow();

                if (SUCCEEDED(result))
                {
                    result = Play();
                }
            }

            ReleaseClasses();
        }

        CoUninitialize();
    }

    return result;
}

LRESULT WindowMessageHandler(const HWND window, const UINT message, const WPARAM wp, const LPARAM lp)
{
    if (message == WM_DESTROY)
    {
        PostQuitMessage(0);

        return 0;
    }

    if (message == WM_KEYDOWN)
    {
        if (wp == VK_RETURN || wp == VK_ESCAPE || wp == VK_SPACE)
        {
            if (State.MediaControl != NULL)
            {
                State.MediaControl->Stop();
            }

            State.IsRunning = FALSE;

            PostMessageA(State.HWND, WM_CLOSE, 0, 0);
        }
    }

    return DefWindowProcA(window, message, wp, lp);
}

s32 InitializeWindow(const HINSTANCE instance)
{
    const WNDCLASSA wc =
    {
        .lpfnWndProc = (WNDPROC)WindowMessageHandler,
        .hInstance = instance,
        .hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1)),
        .lpszClassName = PLAYER_WINDOW_CLASS_NAME
    };

    if (!RegisterClassA(&wc)) { return -1; }

    State.HWND = CreateWindowExA(WS_OVERLAPPED,
        PLAYER_WINDOW_CLASS_NAME, PLAYER_WINDOW_TITLE_NAME,
        WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0, PLAYER_WINDOW_WIDTH, PLAYER_WINDOW_HEIGHT,
        NULL, NULL, instance, NULL);

    return State.HWND == NULL ? -1 : 0;
}

s32 InitializeClasses(void)
{
    COM_OBJECT_CHECK(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&State.GraphBuilder));

    COM_OBJECT_CHECK(State.GraphBuilder->QueryInterface(IID_IMediaControl, (void**)&State.MediaControl));

    COM_OBJECT_CHECK(State.GraphBuilder->QueryInterface(IID_IVideoWindow, (void**)&State.VideoWindow));

    COM_OBJECT_CHECK(State.GraphBuilder->QueryInterface(IID_IMediaEventEx, (void**)&State.MediaEvent));

    return 0;
}

void ReleaseClasses(void)
{
    if (State.MediaEvent != NULL)
    {
        State.MediaEvent->Release();
        State.MediaEvent = NULL;
    }

    if (State.VideoWindow != NULL)
    {
        State.VideoWindow->Release();
        State.VideoWindow = NULL;
    }

    if (State.MediaControl != NULL)
    {
        State.MediaControl->Release();
        State.MediaControl = NULL;
    }

    if (State.GraphBuilder != NULL)
    {
        State.GraphBuilder->Release();
        State.GraphBuilder = NULL;
    }
}

s32 InitializeVideoWindow(void)
{
    if (State.VideoWindow == NULL) { return 1; }

    State.VideoWindow->put_Owner((OAHWND)State.HWND);

    auto mode = 0L;
    auto result = State.VideoWindow->get_FullScreenMode(&mode);

    if (mode == OAFALSE)
    {
        result = State.VideoWindow->put_MessageDrain((OAHWND)State.HWND);

        if (FAILED(result)) { return -1; }

        result = State.VideoWindow->put_FullScreenMode(OATRUE);

        if (FAILED(result)) { return -1; }
    }

    ShowWindow(State.HWND, SW_SHOWMAXIMIZED);

    const auto width = GetSystemMetrics(SM_CXSCREEN);
    const auto height = GetSystemMetrics(SM_CYSCREEN);

    State.VideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
    State.VideoWindow->put_Left(0);
    State.VideoWindow->put_Top(0);
    State.VideoWindow->put_Width(width);
    State.VideoWindow->put_Height(height);

    return 0;
}

s32 InitializePlayerVideo(const LPCWSTR file)
{
    {
        auto result = State.GraphBuilder->AddSourceFilter(file, L"MPEG-I Stream Video Filter", &State.VideoFilter);

        if (FAILED(result)) { return result; }

        result = CoCreateInstance(CLSID_MPEG1Splitter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&State.VideoSplitter);

        if (FAILED(result)) { return result; }

        result = State.GraphBuilder->AddFilter(State.VideoSplitter, L"MPEG-I Video Splitter");

        if (FAILED(result)) { return result; }

        result = CoCreateInstance(CLSID_CMpegVideoCodec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&State.VideoDecoder);

        if (FAILED(result)) { return result; }

        result = State.GraphBuilder->AddFilter(State.VideoDecoder, L"MPEG-I Video Decoder");

        if (FAILED(result)) { return result; }

        result = CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&State.VideoRenderer);

        if (FAILED(result)) { return result; }

        result = State.GraphBuilder->AddFilter(State.VideoRenderer, L"MPEG-I Video Renderer");

        if (FAILED(result)) { return result; }
    }

    {
        {
            IPin* videoFilterOutPin = NULL;
            auto result = State.VideoFilter->FindPin(L"Output", &videoFilterOutPin);

            if (FAILED(result)) { return result; }

            IPin* videoSplitterInPin = NULL;
            result = State.VideoSplitter->FindPin(L"Input", &videoSplitterInPin);

            if (FAILED(result)) { return result; }

            result = State.GraphBuilder->Connect(videoFilterOutPin, videoSplitterInPin);

            if (FAILED(result)) { return result; }
        }

        {
            IPin* videoSplitterOutPin = NULL;
            auto result = State.VideoSplitter->FindPin(L"Video", &videoSplitterOutPin);

            if (FAILED(result)) { return result; }

            IPin* videoDecoderInPin = NULL;
            result = State.VideoDecoder->FindPin(L"In", &videoDecoderInPin);

            if (FAILED(result)) { return result; }

            result = State.GraphBuilder->Connect(videoSplitterOutPin, videoDecoderInPin);

            if (FAILED(result)) { return result; }
        }

        {
            IPin* videoDecoderOutPin = NULL;
            auto result = State.VideoDecoder->FindPin(L"Out", &videoDecoderOutPin);

            if (FAILED(result)) { return result; }

            IPin* videoRendererInPin = NULL;
            result = State.VideoRenderer->FindPin(L"In", &videoRendererInPin);

            if (FAILED(result)) { return result; }

            result = State.GraphBuilder->Connect(videoDecoderOutPin, videoRendererInPin);

            if (FAILED(result)) { return result; }
        }
    }

    return 0;
}

s32 InitializePlayerAudio(const LPCWSTR file)
{
    {
        auto result = State.GraphBuilder->AddSourceFilter(file, L"MPEG-I Stream Audio Filter", &State.AudioFilter);

        if (FAILED(result)) { return result; }

        result = CoCreateInstance(CLSID_MPEG1Splitter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&State.AudioSplitter);

        if (FAILED(result)) { return result; }

        result = State.GraphBuilder->AddFilter(State.AudioSplitter, L"MPEG-I Audio Splitter");

        if (FAILED(result)) { return result; }

        result = CoCreateInstance(CLSID_CMpegAudioCodec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&State.AudioDecoder);

        if (FAILED(result)) { return result; }

        result = State.GraphBuilder->AddFilter(State.AudioDecoder, L"MPEG-I Audio Decoder");

        if (FAILED(result)) { return result; }

        result = CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&State.AudioRenderer);

        if (FAILED(result)) { return result; }

        result = State.GraphBuilder->AddFilter(State.AudioRenderer, L"MPEG-I Audio Renderer");

        if (FAILED(result)) { return result; }
    }

    {
        {
            IPin* AudioFilterOutPin = NULL;
            auto result = State.AudioFilter->FindPin(L"Output", &AudioFilterOutPin);

            if (FAILED(result)) { return result; }

            IPin* AudioSplitterInPin = NULL;
            result = State.AudioSplitter->FindPin(L"Input", &AudioSplitterInPin);

            if (FAILED(result)) { return result; }

            result = State.GraphBuilder->Connect(AudioFilterOutPin, AudioSplitterInPin);

            if (FAILED(result)) { return result; }
        }

        {
            IPin* AudioSplitterOutPin = NULL;
            auto result = State.AudioSplitter->FindPin(L"Audio", &AudioSplitterOutPin);

            if (FAILED(result)) { return result; }

            IPin* AudioDecoderInPin = NULL;
            result = State.AudioDecoder->FindPin(L"In", &AudioDecoderInPin);

            if (FAILED(result)) { return result; }

            result = State.GraphBuilder->Connect(AudioSplitterOutPin, AudioDecoderInPin);

            if (FAILED(result)) { return result; }
        }

        {
            IPin* AudioDecoderOutPin = NULL;
            auto result = State.AudioDecoder->FindPin(L"Out", &AudioDecoderOutPin);

            if (FAILED(result)) { return result; }

            IPin* AudioRendererInPin = NULL;
            result = State.AudioRenderer->FindPin(L"Audio Input pin (rendered)", &AudioRendererInPin);

            if (FAILED(result)) { return result; }

            result = State.GraphBuilder->Connect(AudioDecoderOutPin, AudioRendererInPin);

            if (FAILED(result)) { return result; }
        }
    }

    return 0;
}

s32 InitializePlayer(const LPCWSTR video, const LPCWSTR audio)
{
    auto result = InitializePlayerVideo(video);

    if (FAILED(result)) { return result; }

    // There is a need to support video-only files,
    // therefore, no error checks performed on audio initialization.

    InitializePlayerAudio(audio);

    return 0;
}

s32 Play(void)
{
    State.IsRunning = TRUE;

    auto result = State.MediaControl->Pause();

    if (FAILED(result)) { return result; }

    result = State.MediaControl->Run();

    if (FAILED(result)) { return result; }

    long eventCode = 0;
    LONG_PTR a = NULL;
    LONG_PTR b = NULL;

    do
    {
        State.MediaEvent->GetEvent(&eventCode, &a, &b, 0);
        State.MediaEvent->FreeEventParams(eventCode, a, b);

        if (eventCode == EC_COMPLETE)
        {
            State.IsRunning = FALSE;
            break;
        }

        Sleep(100);

        HandleWindowMessages();

        if (!State.IsRunning) { break; }
    } while (TRUE);

    return 0;
}

void HandleWindowMessages(void)
{
    MSG msg = { 0 };
    auto exists = PeekMessageA(&msg, State.HWND, 0, 0, PM_REMOVE);

    while (exists)
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
        exists = PeekMessageA(&msg, State.HWND, 0, 0, PM_REMOVE);
    }
}