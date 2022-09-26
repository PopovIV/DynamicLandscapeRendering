#include "application.h"

Application::Application() {
    m_Input = nullptr;
    m_Direct3D = nullptr;
    m_Timer = nullptr;
    m_Fps = nullptr;
    m_ShaderManager = nullptr;
    m_TextureManager = nullptr;
    m_Zone = nullptr;
}

// Function to initialize application instance and all it's subclasses
bool Application::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {

    bool result;

    // Create the input object.
    m_Input = new Input;
    if (!m_Input)
        return false;

    // Initialize the input object.
    result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
        return false;
    }

    // Create the Direct3D object.
    m_Direct3D = new D3DClass;
    if (!m_Direct3D)
        return false;

    // Initialize the Direct3D object.
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    // Create the shader manager object.
    m_ShaderManager = new ShaderManager;
    if (!m_ShaderManager)
        return false;

    // Initialize the shader manager object.
    result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the shader manager object.", L"Error", MB_OK);
        return false;
    }

    // Create the texture manager object.
    m_TextureManager = new TextureManager;
    if (!m_TextureManager)
        return false;

    // Initialize the texture manager object.
    result = m_TextureManager->Initialize(10);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the texture manager object.", L"Error", MB_OK);
        return false;
    }

    // Load textures into the texture manager.
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), (char*)"data/textures/test.tga", 0);
    if (!result)
        return false;

    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), (char*)"data/textures/dirt01d.tga", 1);
    if (!result)
        return false;

    // Create the timer object.
    m_Timer = new Timer;
    if (!m_Timer)
        return false;

    // Initialize the timer object.
    result = m_Timer->Initialize();
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
        return false;
    }

    // Create the fps object.
    m_Fps = new Fps;
    if (!m_Fps)
        return false;

    // Initialize the fps object.
    m_Fps->Initialize();

    // Create the zone object.
    m_Zone = new Zone;
    if (!m_Zone)
        return false;

    // Initialize the zone object.
    result = m_Zone->Initialize(m_Direct3D, hwnd, screenWidth, screenHeight, SCREEN_DEPTH);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the zone object.", L"Error", MB_OK);
        return false;
    }

    return true;

}

// Function to clear all stuff that was created in initialize function
void Application::Shutdown() {

    // Release the zone object.
    if (m_Zone) {
        m_Zone->Shutdown();
        delete m_Zone;
        m_Zone = nullptr;
    }

    // Release the fps object.
    if (m_Fps) {
        delete m_Fps;
        m_Fps = nullptr;
    }

    // Release the timer object.
    if (m_Timer) {
        delete m_Timer;
        m_Timer = nullptr;
    }

    // Release the texture manager object.
    if (m_TextureManager) {
        m_TextureManager->Shutdown();
        delete m_TextureManager;
        m_TextureManager = nullptr;
    }

    // Release the shader manager object.
    if (m_ShaderManager) {
        m_ShaderManager->Shutdown();
        delete m_ShaderManager;
        m_ShaderManager = nullptr;
    }

    // Release the Direct3D object.
    if (m_Direct3D) {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = nullptr;
    }

    // Release the input object.
    if (m_Input) {
        m_Input->Shutdown();
        delete m_Input;
        m_Input = nullptr;
    }

}

// Function to update frame each second
bool Application::Frame()
{
    bool result;

    // Update the system stats.
    m_Fps->Frame();
    m_Timer->Frame();

    // Do the input frame processing.
    result = m_Input->Frame();
    if (!result)
        return false;

    // Check if the user pressed escape and wants to exit the application.
    if (m_Input->IsEscapePressed() == true)
        return false;

    // Do the zone frame processing.
    result = m_Zone->Frame(m_Direct3D, m_Input, m_ShaderManager, m_TextureManager, m_Timer->GetTime(), m_Fps->GetFps());
    if (!result)
        return false;

    return result;

}