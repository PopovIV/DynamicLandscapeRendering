//--------------------------------------------------------------------------------------
// Zone class handles all the camera movment and terrain grid rendering
//--------------------------------------------------------------------------------------

#ifndef _ZONE_H_
#define _ZONE_H_

#include "d3dclass.h"
#include "input.h"
#include "shadermanager.h"
#include "texturemanager.h"
#include "timer.h"
#include "userinterface.h"
#include "camera.h"
#include "light.h"
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
    bool Frame(D3DClass* Direct3D, Input* Input, ShaderManager* ShaderManager, TextureManager* TextureManager, float frameTime, int fps, XMFLOAT4 scales, XMFLOAT3 lightDir);

    void GetPosition(float& x, float& y , float& z) { m_Position->GetPosition(x, y, z); };
    void GetRotation(float& x, float& y, float& z) { m_Position->GetRotation(x, y, z); };
    XMFLOAT3 GetLighDirection(void) { return lightDir; };
    void ToggleWireFrame() { m_wireFrame = !m_wireFrame; };
    void ToggleDayNight() { m_dayNightCycle = !m_dayNightCycle; };
    bool GetWireFrame() { return m_wireFrame; };
    bool GetDayNight() { return m_dayNightCycle; };

private:
    // Function to process all user's input
    void HandleMovementInput(Input* Input, float frameTime);
    // Render function
    bool Render(D3DClass* Direct3D, ShaderManager* ShaderManager, TextureManager* TextureManager);

private:
    UserInterface* m_UserInterface;
    Camera* m_Camera;
    Light* m_Light;
    Position* m_Position;
    Terrain* m_Terrain;
    XMFLOAT3 lightDir;
    XMFLOAT4 scales;
    bool m_displayUI, m_wireFrame, m_dayNightCycle;
};

#endif