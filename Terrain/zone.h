//--------------------------------------------------------------------------------------
// Zone class handles all the camera movment and terrain grid rendering
//--------------------------------------------------------------------------------------

#ifndef _ZONE_H_
#define _ZONE_H_

#include "d3dclass.h"
#include "input.h"
#include "shadermanager.h"
#include "timer.h"
#include "userinterface.h"
#include "camera.h"
#include "position.h"
#include "terrain.h"

class Zone {
public:
    // constructors
    Zone();
    Zone(const Zone& other) {};
    // destructor
    ~Zone() {};

    // Function to initialize user interface, camera, position and grid
    bool Initialize(D3DClass* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth);
    // Function to clear all stuff that was created in initialize function
    void Shutdown();
    // Function to update frame each second
    bool Zone::Frame(D3DClass* Direct3D, Input* Input, ShaderManager* ShaderManager, float frameTime, int fps);

private:
    // Function to process all user's input
    void HandleMovementInput(Input* Input, float frameTime);
    // Render function
    bool Render(D3DClass* Direct3D, ShaderManager* ShaderManager);

private:
    UserInterface* m_UserInterface;
    Camera* m_Camera;
    Position* m_Position;
    Terrain* m_Terrain;
    bool m_displayUI, m_wireFrame;
};

#endif