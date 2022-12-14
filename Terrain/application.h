//--------------------------------------------------------------------------------------
// Application class handles all the graphics, input, sound, processing, and so forth
//--------------------------------------------------------------------------------------
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

// GLOBALS
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

#include "input.h"
#include "d3dclass.h"
#include "shadermanager.h"
#include "texturemanager.h"
#include "timer.h"
#include "fps.h"
#include "zone.h"

class Application {
public:
    // constructors
    Application();
    Application(const Application& other) {};
    // destructors
    ~Application() {};

    // Function to initialize application instance and all it's subclasses
    bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
    // Function to clear all stuff that was created in initialize function
    void Shutdown();
    // Function to update frame each second
    bool Frame();

private:
    Input* m_Input;
    D3DClass* m_Direct3D;
    ShaderManager* m_ShaderManager;
    TextureManager* m_TextureManager;
    Timer* m_Timer;
    Fps* m_Fps;
    Zone* m_Zone;
    XMFLOAT4 scales;
};

#endif