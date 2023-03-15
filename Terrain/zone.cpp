#include "zone.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"

// Function to initialize user interface, camera, position and grid
bool Zone::Initialize(D3DClass* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth) {
    // Create the camera object.
    m_Camera = new Camera;
    if (!m_Camera) {
        return false;
    }

    // Set the initial position of the camera and build the matrices needed for rendering.
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();

    // Create the light object.
    m_Light = new Light;
    if (!m_Light) {
        return false;
    }

    // Initialize the light object.
    m_Light->SetAmbientColor(0.7f, 0.7f, 0.7f, 1.0f);
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetDirection(27.0f, -56.0f, 10.0f);
    m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetSpecularPower(300.0f);

    m_RenderTexture = new RenderTexture();
    if (!m_RenderTexture) {
        return false;
    }

    bool result = m_RenderTexture->Initialize(Direct3D->GetDevice(), screenWidth, screenHeight);
    if (!result) {
        return false;
    }

    // Create the position object.
    m_Position = new Position;
    if (!m_Position) {
        return false;
    }

    // Set the initial position and rotation.
    m_Position->SetPosition(230.51f, 218.17f, 222.842f);
    m_Position->SetRotation(1.58f, 288.344f, 0.0f);

    // Create the frustum object.
    m_Frustum = new Frustum;
    if (!m_Frustum) {
        return false;
    }

    // Initialize the frustum object.
    m_Frustum->Initialize(screenDepth);

    // Create the sky dome object.
    m_SkyDome = new SkyDome;
    if (!m_SkyDome) {
        return false;
    }

    // Initialize the sky dome object.
    result = m_SkyDome->Initialize(Direct3D->GetDevice());
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the sky dome object.", L"Error", MB_OK);
        return false;
    }

    // Create the terrain object.
    m_Terrain = new Terrain;
    if (!m_Terrain) {
        return false;
    }

    // Initialize toneMap object.
    m_ToneMap = new ToneMap;
    if (!m_ToneMap) {
        return false;
    }
    result = m_ToneMap->Initialize(Direct3D->GetDevice(), hwnd, screenWidth, screenHeight);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the tone map object.", L"Error", MB_OK);
        return false;
    }

    // Initialize the terrain object.
    result = m_Terrain->Initialize(Direct3D->GetDevice(), (char*)"data/setup.txt");
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
        return false;
    }

    // Create the profiler object.
    m_Profiler = new CGpuProfiler;
    if (!m_Profiler) {
        return false;
    }

    // Initialize the profiler object.
    result = m_Profiler->Init(Direct3D->GetDevice());
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the profiler object.", L"Error", MB_OK);
        return false;
    }

    lightDir = m_Light->GetDirection();
    
    // Set the UI to display by default.
    m_displayUI = false;
    m_wireFrame = false;
    m_dayNightCycle = false;
    m_cellLines = false;
    m_culling = true;
    // Set the user locked to the terrain height for movement.
    m_heightLocked = true;

    return true;
}
// Function to clear all stuff that was created in initialize function
void Zone::Shutdown() {
    // Release the terrain object.
    if (m_Terrain) {
        m_Terrain->Shutdown();
        delete m_Terrain;
        m_Terrain = nullptr;
    }

    // Release the sky dome object.
    if (m_SkyDome) {
        m_SkyDome->Shutdown();
        delete m_SkyDome;
        m_SkyDome = nullptr;
    }

    if (m_Frustum) {
        delete m_Frustum;
        m_Frustum = nullptr;
    }

    // Release the render to texture object.
    if (m_RenderTexture) {
        m_RenderTexture->Shutdown();
        delete m_RenderTexture;
        m_RenderTexture = nullptr;
    }

    // Release the position object.
    if (m_Position) {
        delete m_Position;
        m_Position = nullptr;
    }

    // Release the light object.
    if (m_Light) {
        delete m_Light;
        m_Light = nullptr;
    }

    // Release the camera object.
    if (m_Camera) {
        delete m_Camera;
        m_Camera = nullptr;
    }

    if (m_ToneMap) {
        m_ToneMap->Shutdown();
        delete m_ToneMap;
        m_ToneMap = nullptr;
    }

    if (m_Profiler) {
        m_Profiler->Shutdown();
        delete m_Profiler;
        m_Profiler = nullptr;
    }
}

// Function to update frame each second
bool Zone::Frame(D3DClass* Direct3D, Input* Input, ShaderManager* ShaderManager, TextureManager* TextureManager, float frameTime, int fps, XMFLOAT4 scales, float detailScale, XMFLOAT3 lightDir) {
    // Do the frame input processing.
    HandleMovementInput(Input, frameTime);

    // Get the view point position/rotation.
    float posX, posY, posZ, rotX, rotY, rotZ, height;
    m_Position->GetPosition(posX, posY, posZ);
    m_Position->GetRotation(rotX, rotY, rotZ);
    this->scales = scales;
    this->detailScale = detailScale;
    m_Light->SetDirection(lightDir.x, lightDir.y, lightDir.z);

    m_Terrain->Frame();

    // If the height is locked to the terrain then position the camera on top of it.
    if (m_heightLocked) {
        // Get the height of the triangle that is directly underneath the given camera position.
        bool foundHeight = m_Terrain->GetHeightAtPosition(posX, posZ, height);
        if (foundHeight) {
            // If there was a triangle under the camera then position the camera just above it by one meter.
            m_Position->SetPosition(posX, height + 5.0f, posZ);
            m_Camera->SetPosition(posX, height + 5.0f, posZ);
        }
    }

    // Render the graphics.
    bool result = Render(Direct3D, ShaderManager, TextureManager);
    if (!result) {
        return false;
    }

    return true;
}

// Function to handle user input from keyboard/mouse
void Zone::HandleMovementInput(Input* Input, float frameTime) {
    // Set the frame time for calculating the updated position.
    m_Position->SetFrameTime(frameTime);
    bool keyDown;
    // Handle the input.
    keyDown = Input->IsLeftPressed();
    m_Position->MoveLeft(keyDown);
    keyDown = Input->IsRightPressed();
    m_Position->MoveRight(keyDown);
    keyDown = Input->IsUpPressed();
    m_Position->MoveForward(keyDown);
    keyDown = Input->IsDownPressed();
    m_Position->MoveBackward(keyDown);
    keyDown = Input->IsPgUpPressed();
    m_Position->LookUpward(keyDown);
    keyDown = Input->IsPgDownPressed();
    m_Position->LookDownward(keyDown);
    XMFLOAT2 mouseMove = Input->IsMouseUsed();
    m_Position->TurnMouse(mouseMove);

    float posX, posY, posZ, rotX, rotY, rotZ;
    // Get the view point position/rotation.
    m_Position->GetPosition(posX, posY, posZ);
    m_Position->GetRotation(rotX, rotY, rotZ);

    // Set the position of the camera.
    m_Camera->SetPosition(posX, posY, posZ);
    m_Camera->SetRotation(rotX, rotY, rotZ);

    // Determine if the user interface should be displayed or not.
    if (Input->IsF1Toggled()) {
        m_displayUI = !m_displayUI;
    }

    // Determine if the terrain should be rendered in wireframe or not.
    if (Input->IsF2Toggled()) {
        m_wireFrame = !m_wireFrame;
    }

    // Determine day/night cycle off/on.
    if (Input->IsF3Toggled()) {
        m_dayNightCycle = !m_dayNightCycle;
    }

    // Determine if we should render the lines around each terrain cell.
    if (Input->IsF4Toggled()) {
        m_cellLines = !m_cellLines;
    }

    if (Input->IsSpaceToggled()) {
        m_heightLocked = !m_heightLocked;
    }
}

void Zone::GetCulling(float& polygons, float& rendered, float& culled) {
    polygons = (float)m_Terrain->GetRenderCount();
    rendered = (float)m_Terrain->GetCellsDrawn();
    culled = (float)m_Terrain->GetCellsCulled();
};

// Render function
bool Zone::RenderToTexture(D3DClass* Direct3D, ShaderManager* ShaderManager, TextureManager* TextureManager) {
    m_RenderTexture->SetRenderTarget(Direct3D->GetDeviceContext(), Direct3D->GetDepthStencilView());
    m_RenderTexture->ClearRenderTarget(Direct3D->GetDeviceContext(), Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Get the world, view, and projection matrices from the camera and d3d objects.
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
    Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    Direct3D->GetProjectionMatrix(projectionMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);
    Direct3D->GetOrthoMatrix(orthoMatrix);

    // Get the position of the camera.
    XMFLOAT3 cameraPosition = m_Camera->GetPosition();

    // Construct the frustum.
    m_Frustum->ConstructFrustum(projectionMatrix, viewMatrix);

    // SKYDOME
    // Turn off back face culling and turn off the Z buffer.
    Direct3D->TurnOffCulling();
    Direct3D->TurnZBufferOff();

    // Translate the sky dome to be centered around the camera position.
    worldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);

    // Render the sky dome using the sky dome shader.
    m_SkyDome->Render(Direct3D->GetDeviceContext());
    bool result = ShaderManager->RenderSkyDomeShader(Direct3D->GetDeviceContext(), m_SkyDome->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor());
    if (!result)
        return false;

    // Reset the world matrix.
    Direct3D->GetWorldMatrix(worldMatrix);

    // Turn the Z buffer back and back face culling on.
    Direct3D->TurnZBufferOn();
    Direct3D->TurnOnCulling();

    // Determine if the terrain should be rendered in wireframe or not.
    if (m_wireFrame) {
        Direct3D->EnableWireframe();
    }

    // Render the terrain grid using the color shader.
    float posX, posY, posZ;
    m_Position->GetPosition(posX, posY, posZ);
    //m_Terrain->Render(Direct3D->GetDeviceContext());
    ID3D11ShaderResourceView* textures[] = { TextureManager->GetTexture(0), TextureManager->GetTexture(4), TextureManager->GetTexture(8), TextureManager->GetTexture(12), TextureManager->GetTexture(16), TextureManager->GetTexture(20) };
    ID3D11ShaderResourceView* normalMaps[] = { TextureManager->GetTexture(1), TextureManager->GetTexture(5), TextureManager->GetTexture(9), TextureManager->GetTexture(13), TextureManager->GetTexture(17), TextureManager->GetTexture(21) };
    ID3D11ShaderResourceView* roughMaps[] = { TextureManager->GetTexture(2), TextureManager->GetTexture(6), TextureManager->GetTexture(10), TextureManager->GetTexture(14), TextureManager->GetTexture(18), TextureManager->GetTexture(22) };
    ID3D11ShaderResourceView* aoMaps[] = { TextureManager->GetTexture(3), TextureManager->GetTexture(7), TextureManager->GetTexture(11), TextureManager->GetTexture(15), TextureManager->GetTexture(19) };

    // Update our time
    static float t = 0.0f;
    static ULONGLONG timeStart = 0;
    if (m_dayNightCycle) {
        ULONGLONG timeCur = GetTickCount64();
        if (timeStart == 0) {
            timeStart = timeCur;
        }
        t = (timeCur - timeStart) / 1000.0f;

        XMVECTOR vec = XMLoadFloat3(&lightDir);
        XMVECTOR direction = XMVector3Transform(vec, XMMatrixRotationZ(t));
        XMFLOAT3 fl = XMFLOAT3(1.0f, 1.0f, 1.0f);
        XMStoreFloat3(&fl, direction);
        m_Light->SetDirection(fl.x, fl.y, fl.z);
    }
    else {
        t = 0;
        timeStart = 0;
        lightDir = m_Light->GetDirection();
    }

    // Render the terrain cells (and cell lines if needed).
    for (int i = 0; i < m_Terrain->GetCellCount(); i++) {
        // Put the terrain cell buffers on the pipeline.
        m_Frustum->SetLockView(m_lockView);
        result = m_Terrain->RenderCell(Direct3D->GetDeviceContext(), i, m_Frustum, m_culling);
        if (result) {
            // Render the cell buffers using the terrain shader.
            result = ShaderManager->RenderTerrainShader(Direct3D->GetDeviceContext(), m_Terrain->GetCellIndexCount(i), worldMatrix, viewMatrix,
                projectionMatrix, XMFLOAT3(posX, posY, posZ), textures, normalMaps, roughMaps, aoMaps, m_Light, scales, detailScale);
            if (!result) {
                return false;
            }

            // If needed then render the bounding box around this terrain cell using the color shader. 
            if (m_cellLines) {
                m_Terrain->RenderCellLines(Direct3D->GetDeviceContext(), i);
                ShaderManager->RenderColorShader(Direct3D->GetDeviceContext(), m_Terrain->GetCellLinesIndexCount(i), worldMatrix, viewMatrix, projectionMatrix);
                if (!result) {
                    return false;
                }
            }
        }
    }

    // Determine if the terrain should be rendered in wireframe or not.
    if (m_wireFrame) {
        Direct3D->DisableWireframe();
    }

    Direct3D->SetBackBufferRenderTarget();
    return true;

}

void Zone::Resize(int width, int height) {
    m_RenderTexture->Resize(Direct3D->GetDevice(), width, height);
    m_ToneMap->Resize(Direct3D->GetDevice(), width, height);
}

// Render function
bool Zone::Render(D3DClass* Direct3D, ShaderManager* ShaderManager, TextureManager* TextureManager) {
    m_Profiler->BeginFrame(Direct3D->GetDeviceContext());

    LARGE_INTEGER fr, t1, t2;
    QueryPerformanceCounter(&t1);

    bool result = RenderToTexture(Direct3D, ShaderManager, TextureManager);
    if (!result) {
        return false;
    }
    m_Profiler->Timestamp(Direct3D->GetDeviceContext(), GTS_DrawToTexture);

    Direct3D->BeginScene(0.30f, 0.59f, 0.71f, 1.0f);

    // Post effect render
    m_ToneMap->Process(Direct3D->GetDeviceContext(), m_RenderTexture->GetShaderResourceView(), Direct3D->GetRenderTarget(), Direct3D->GetViewPort());
    m_Profiler->Timestamp(Direct3D->GetDeviceContext(), GTS_ToneMapping);

    m_Profiler->WaitForDataAndUpdate(Direct3D->GetDeviceContext());

    float dTDrawTotal = 0.0f;
    for (GTS gts = GTS_BeginFrame; gts < GTS_EndFrame; gts = GTS(gts + 1)) {
        dTDrawTotal += m_Profiler->DtAvg(gts);
    }

    m_drawTime = 1000.0f * dTDrawTotal;
    m_drawToTextureTime = 1000.0f * m_Profiler->DtAvg(GTS_DrawToTexture);
    m_toneMappingTime = 1000.0f * m_Profiler->DtAvg(GTS_ToneMapping);

    QueryPerformanceCounter(&t2);
    QueryPerformanceFrequency(&fr);
    m_CPUTime = 1000.0f * (t2.QuadPart - t1.QuadPart) / (float)fr.QuadPart;
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present the rendered scene to the screen.
    Direct3D->EndScene();

    m_Profiler->EndFrame(Direct3D->GetDeviceContext());

    return true;
}
