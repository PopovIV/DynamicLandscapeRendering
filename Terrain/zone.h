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
#include "camera.h"
#include "light.h"
#include "position.h"
#include "skydome.h"
#include "frustum.h"
#include "terrain.h"
#include "renderTexture.h"
#include "toneMap.h"

class Zone {
  public:
    // Function to initialize user interface, camera, position and grid
    bool Initialize(D3DClass* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth);
    // Function to clear all stuff that was created in initialize function
    void Shutdown();
    // Function to update frame each second
    bool Frame(D3DClass* Direct3D, Input* Input, ShaderManager* ShaderManager, TextureManager* TextureManager, float frameTime, int fps, XMFLOAT4 scales, float detailScale, XMFLOAT3 lightDir);

    void GetPosition(float& x, float& y , float& z) { m_Position->GetPosition(x, y, z); };
    void GetRotation(float& x, float& y, float& z) { m_Position->GetRotation(x, y, z); };
    void GetCulling(float& polygons, float& rendered, float& culled);
    XMFLOAT3 GetLighDirection(void) { return lightDir; };
    void ToggleWireFrame() { m_wireFrame = !m_wireFrame; };
    void ToggleDayNight() { m_dayNightCycle = !m_dayNightCycle; };
    void ToggleCulling() { m_culling = !m_culling; };
    void ToggleHeightLocked() { m_heightLocked = !m_heightLocked; };
    void ToggleLockView() { m_lockView = !m_lockView; };
    bool GetWireFrame() { return m_wireFrame; };
    bool GetDayNight() { return m_dayNightCycle; };
    bool GetHeightLocked() { return m_heightLocked; };
    bool GetLockView() { return m_lockView; };

  private:
    bool RenderToTexture(D3DClass* Direct3D, ShaderManager* ShaderManager, TextureManager* TextureManager);
    // Function to process all user's input
    void HandleMovementInput(Input* Input, float frameTime);
    // Render function
    bool Render(D3DClass* Direct3D, ShaderManager* ShaderManager, TextureManager* TextureManager);

    RenderTexture* m_RenderTexture = nullptr;
    D3DClass* Direct3D = nullptr;
    Camera* m_Camera = nullptr;
    Light* m_Light = nullptr;
    Position* m_Position = nullptr;
    Terrain* m_Terrain = nullptr;
    SkyDome* m_SkyDome = nullptr;
    ToneMap* m_ToneMap = nullptr;
    Frustum* m_Frustum = nullptr;
    XMFLOAT3 lightDir = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT4 scales = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    float detailScale;
    bool m_displayUI, m_wireFrame, m_dayNightCycle, m_cellLines, m_culling, m_heightLocked, m_lockView;
};

#endif
