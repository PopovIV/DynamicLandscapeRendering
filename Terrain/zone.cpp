#include "zone.h"

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
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetDirection(-0.5f, -1.0f, -0.5f);

    // Create the position object.
    m_Position = new Position;
    if (!m_Position)
        return false;

    // Set the initial position and rotation.
    m_Position->SetPosition(128.0f, 10.0f, -10.0f);
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

    // Set the UI to display by default.
    m_displayUI = true;
    m_wireFrame = true;

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
bool Zone::Frame(D3DClass* Direct3D, Input * Input, ShaderManager * ShaderManager, TextureManager* TextureManager, float frameTime, int fps) {

    bool result;
    float posX, posY, posZ, rotX, rotY, rotZ;

    // Do the frame input processing.
    HandleMovementInput(Input, frameTime);

    // Get the view point position/rotation.
    m_Position->GetPosition(posX, posY, posZ);
    m_Position->GetRotation(rotX, rotY, rotZ);

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
void Zone::HandleMovementInput(Input * Input, float frameTime) {

    bool keyDown;
    float posX, posY, posZ, rotX, rotY, rotZ;

    // Set the frame time for calculating the updated position.
    m_Position->SetFrameTime(frameTime);

    // Handle the input.
    keyDown = Input->IsLeftPressed();
    m_Position->TurnLeft(keyDown);

    keyDown = Input->IsRightPressed();
    m_Position->TurnRight(keyDown);

    keyDown = Input->IsUpPressed();
    m_Position->MoveForward(keyDown);

    keyDown = Input->IsDownPressed();
    m_Position->MoveBackward(keyDown);

    keyDown = Input->IsAPressed();
    m_Position->MoveUpward(keyDown);

    keyDown = Input->IsZPressed();
    m_Position->MoveDownward(keyDown);

    keyDown = Input->IsPgUpPressed();
    m_Position->LookUpward(keyDown);

    keyDown = Input->IsPgDownPressed();
    m_Position->LookDownward(keyDown);

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
    Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Determine if the terrain should be rendered in wireframe or not.
    if (m_wireFrame)
        Direct3D->EnableWireframe();

    float posX, posY, posZ;
    m_Position->GetPosition(posX, posY, posZ);
    // Render the terrain grid using the color shader.
    m_Terrain->Render(Direct3D->GetDeviceContext());
    result = ShaderManager->RenderTerrainShader(Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, XMFLOAT3(posX, posY, posZ), TextureManager->GetTexture(1), m_Light->GetDirection(), m_Light->GetDiffuseColor());
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

    // Present the rendered scene to the screen.
    Direct3D->EndScene();

    return true;

}