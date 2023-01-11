#include "zone.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"

Zone::Zone() {

    m_UserInterface = nullptr;
    m_Camera = nullptr;
    m_Light = nullptr;
    m_Position = nullptr;
    m_Terrain = nullptr;

}

// Function to initialize user interface, camera, position and grid
bool Zone::Initialize(D3DClass* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth) {
    bool result;

    // Create the user interface object.
    m_UserInterface = new UserInterface;
    if (!m_UserInterface)
        return false;

    // Initialize the user interface object.
    result = m_UserInterface->Initialize(Direct3D, screenHeight, screenWidth);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the user interface object.", L"Error", MB_OK);
        return false;
    }

    // Create the camera object.
    m_Camera = new Camera;
    if (!m_Camera)
        return false;

    // Set the initial position of the camera and build the matrices needed for rendering.
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();

    // Create the light object.
    m_Light = new Light;
    if (!m_Light)
        return false;

    // Initialize the light object.
    m_Light->SetAmbientColor(0.8f, 0.8f, 0.8f, 1.0f);
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetDirection(0.0f, -10000.0f, 0.0f);
    m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetSpecularPower(300.0f);

    // Create the position object.
    m_Position = new Position;
    if (!m_Position)
        return false;

    // Set the initial position and rotation.
    m_Position->SetPosition(512.0f, 200.0f, 250.0f);
    m_Position->SetRotation(0.0f, 0.0f, 0.0f);

    // Create the terrain object.
    m_Terrain = new Terrain;
    if (!m_Terrain)
        return false;
    // Initialize the terrain object.

    result = m_Terrain->Initialize(Direct3D->GetDevice(), "data/setup.txt");
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
        return false;
    }

    lightDir = m_Light->GetDirection();
    
    // Set the UI to display by default.
    m_displayUI = false;
    m_wireFrame = false;
    m_dayNightCycle = false;
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

    // Release the user interface object.
    if (m_UserInterface) {
        m_UserInterface->Shutdown();
        delete m_UserInterface;
        m_UserInterface = nullptr;
    }

}
// Function to update frame each second
bool Zone::Frame(D3DClass* Direct3D, Input* Input, ShaderManager* ShaderManager, TextureManager* TextureManager, float frameTime, int fps, XMFLOAT4 scales, XMFLOAT3 lightDir) {

    bool result;
    float posX, posY, posZ, rotX, rotY, rotZ;

    // Do the frame input processing.
    HandleMovementInput(Input, frameTime);

    // Get the view point position/rotation.
    m_Position->GetPosition(posX, posY, posZ);
    m_Position->GetRotation(rotX, rotY, rotZ);
    this->scales = scales;
    m_Light->SetDirection(lightDir.x, lightDir.y, lightDir.z);

    // Do the frame processing for the user interface.
    result = m_UserInterface->Frame(Direct3D->GetDeviceContext(), fps, posX, posY, posZ, rotX, rotY, rotZ);
    if (!result)
        return false;


    // Render the graphics.
    result = Render(Direct3D, ShaderManager, TextureManager);
    if (!result)
        return false;

    return true;

}
// Function to handle user input from keyboard/mouse
void Zone::HandleMovementInput(Input* Input, float frameTime) {

    bool keyDown;
    float posX, posY, posZ, rotX, rotY, rotZ;

    // Set the frame time for calculating the updated position.
    m_Position->SetFrameTime(frameTime);

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

    // Get the view point position/rotation.
    m_Position->GetPosition(posX, posY, posZ);
    m_Position->GetRotation(rotX, rotY, rotZ);

    // Set the position of the camera.
    m_Camera->SetPosition(posX, posY, posZ);
    m_Camera->SetRotation(rotX, rotY, rotZ);

    // Determine if the user interface should be displayed or not.
    if (Input->IsF1Toggled())
        m_displayUI = !m_displayUI;

    // Determine if the terrain should be rendered in wireframe or not.
    if (Input->IsF2Toggled())
        m_wireFrame = !m_wireFrame;

    // Determine day/night cycle off/on.
    if (Input->IsF3Toggled())
        m_dayNightCycle = !m_dayNightCycle;

}

// Render function
bool Zone::Render(D3DClass* Direct3D, ShaderManager* ShaderManager, TextureManager* TextureManager) {

    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
    bool result;

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Get the world, view, and projection matrices from the camera and d3d objects.
    Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    Direct3D->GetProjectionMatrix(projectionMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);
    Direct3D->GetOrthoMatrix(orthoMatrix);

    // Clear the buffers to begin the scene.
    Direct3D->BeginScene(0.30f, 0.59f, 0.71f, 1.0f);

    // Determine if the terrain should be rendered in wireframe or not.
    if (m_wireFrame)
        Direct3D->EnableWireframe();

    // Render the terrain grid using the color shader.
    float posX, posY, posZ;
    m_Position->GetPosition(posX, posY, posZ);
    m_Terrain->Render(Direct3D->GetDeviceContext());
    ID3D11ShaderResourceView* textures[] = { TextureManager->GetTexture(0), TextureManager->GetTexture(4), TextureManager->GetTexture(8), TextureManager->GetTexture(12), TextureManager->GetTexture(16), TextureManager->GetTexture(20) };
    ID3D11ShaderResourceView* normalMaps[] = { TextureManager->GetTexture(1), TextureManager->GetTexture(5), TextureManager->GetTexture(9), TextureManager->GetTexture(13), TextureManager->GetTexture(17), TextureManager->GetTexture(21) };
    ID3D11ShaderResourceView* roughMaps[] = { TextureManager->GetTexture(2), TextureManager->GetTexture(6), TextureManager->GetTexture(10), TextureManager->GetTexture(14), TextureManager->GetTexture(18) };
    ID3D11ShaderResourceView* aoMaps[] = { TextureManager->GetTexture(3), TextureManager->GetTexture(7), TextureManager->GetTexture(11), TextureManager->GetTexture(15), TextureManager->GetTexture(19) };

    // Update our time
    static float t = 0.0f;
    static ULONGLONG timeStart = 0;
    if (m_dayNightCycle) {
        ULONGLONG timeCur = GetTickCount64();
        if (timeStart == 0)
            timeStart = timeCur;
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

    // Render the cell buffers using the terrain shader.
    result = ShaderManager->RenderTerrainShader(Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, XMFLOAT3(posX, posY, posZ), textures, normalMaps, roughMaps, aoMaps, m_Light, scales);
    if (!result)
        return false;

    // Determine if the terrain should be rendered in wireframe or not.
    if (m_wireFrame)
        Direct3D->DisableWireframe();

    // Render the user interface.
    if (m_displayUI) {
        result = m_UserInterface->Render(Direct3D, ShaderManager, worldMatrix, baseViewMatrix, orthoMatrix);
        if (!result)
            return false;
    }

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present the rendered scene to the screen.
    Direct3D->EndScene();

    return true;

}