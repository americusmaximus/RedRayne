#pragma once

#include "Basic.hxx"
#include "App.Options.hxx"

#define APP_NAME "BloodRayne"

namespace App
{
    struct Application
    {
        App::Options::ApplicationOptions Options;
        // TODO
    };

    struct ApplicationContainer
    {
        BOOL* _IsQuit = (BOOL*)0x00903370;//TODO
        Application** _AppPointer = (Application**)0x0061f7ac;// TODO
        Application* _App = (Application*)0x006c391c;// TODO
    };

    extern ApplicationContainer AppState;

    void StartApplication(void);

    typedef const void (CDECLAPI* INITIALIZEAPPLICATION) (void);// TODO
    static INITIALIZEAPPLICATION InitializeApplication = (INITIALIZEAPPLICATION)0x004abdf0;// TODO
    
    typedef const void (CDECLAPI* INITIALIZEAPPLICATION1) (void);// TODO
    static INITIALIZEAPPLICATION1 InitializeApplication1 = (INITIALIZEAPPLICATION1)0x004ff260;// TODO

    typedef const void (CDECLAPI* RELEASEAPPLICATION) (void);// TODO
    static RELEASEAPPLICATION ReleaseApplication = (RELEASEAPPLICATION)0x004ac480;// TODO
}