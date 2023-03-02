#include "application.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


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
    result = m_TextureManager->Initialize(30);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the texture manager object.", L"Error", MB_OK);
        return false;
    }

    // Load textures into the texture manager.
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_diffuse.dds", 0, Texture::DDS, true);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_normal.dds", 1, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_rough.dds", 2, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_ao.dds", 3, Texture::DDS);
    if (!result)
        return false;


    // Load textures into the texture manager.
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_diffuse2.dds", 4, Texture::DDS, true);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_normal2.dds", 5, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_rough2.dds", 6, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_ao2.dds", 7, Texture::DDS);
    if (!result)
        return false;

    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/rock_diffuse.dds", 8, Texture::DDS, true);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/rock_normal.dds", 9, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/rock_rough.dds", 10, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/rock_ao.dds", 11, Texture::DDS);
    if (!result)
        return false;

    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/slope_diffuse.dds", 12, Texture::DDS, true);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/slope_normal.dds", 13, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/slope_rough.dds", 14, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/slope_ao.dds", 15, Texture::DDS);
    if (!result)
        return false;

    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/snow_diffuse.dds", 16, Texture::DDS, true);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/snow_normal.dds", 17, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/snow_rough.dds", 18, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/snow_ao.dds", 19, Texture::DDS);
    if (!result)
        return false;

    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/detailNormalMap.dds", 20, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/noise.dds", 21, Texture::DDS);
    if (!result)
        return false;
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/HM.dds", 22, Texture::DDS);
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

    scales = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    // Setup Platform/Renderer backends
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());

    return true;

}

// Function to clear all stuff that was created in initialize function
void Application::Shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

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

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Update the system stats.
    m_Fps->Frame();
    m_Timer->Frame();

    static bool demoWindow = false;
    static bool wireframe = false;
    static bool dayNight = false;
    static bool heightLocked = true;
    static bool culling = true;
    static bool myWindow = true;
    static int grassScale = 40;
    static int rockScale = 32;
    static int slopeScale = 32;
    static int snowScale = 2;
    static int detailScale = 2;
    static int lightX = m_Zone->GetLighDirection().x;
    static int lightY = m_Zone->GetLighDirection().y;
    static int lightZ = m_Zone->GetLighDirection().z;
    if (demoWindow)
        ImGui::ShowDemoWindow(&demoWindow);

    if (myWindow)
    {
        ImGui::Begin("ImGui", &myWindow);
        
        dayNight = m_Zone->GetDayNight();
        wireframe = m_Zone->GetWireFrame();

        if (ImGui::Button("Open demo"))
            demoWindow = true;

        if (ImGui::Checkbox("WireFrame Mode", &wireframe))
            m_Zone->ToggleWireFrame();

        if (ImGui::Checkbox("Height locked", &heightLocked))
            m_Zone->ToggleHeightLocked();

        if (ImGui::Checkbox("DayNight Cycle", &dayNight))
            m_Zone->ToggleDayNight();

        if (ImGui::Checkbox("Culling", &culling))
            m_Zone->ToggleCulling();

        
        ImGui::SliderInt("Grass scales", &grassScale, 1, 64, "%d", 0);
        scales.x = grassScale;
        ImGui::SliderInt("Rock scales", &rockScale, 1, 64, "%d", 0);
        scales.y = rockScale;
        ImGui::SliderInt("Slope scales", &slopeScale, 1, 64, "%d", 0);
        scales.z = slopeScale;
        ImGui::SliderInt("Snow scales", &snowScale, 1, 64, "%d", 0);
        scales.w = snowScale;
        ImGui::SliderInt("Detail scales", &detailScale, 1, 64, "%d", 0);

        ImGui::SliderInt("Light position X", &lightX, -100, 100, "%d", 0);
        ImGui::SliderInt("Light position Y", &lightY, -100, 100, "%d", 0);
        ImGui::SliderInt("Light position Z", &lightZ, -100, 100, "%d", 0);


        std::string str = "FPS: ";
        str += std::to_string(m_Fps->GetFps());
        ImGui::Text(str.c_str());
        float x, y, z;
        m_Zone->GetPosition(x, y, z);
        str = "\nX: ";
        str += std::to_string(x);
        ImGui::Text(str.c_str());
        str = "\nY: ";
        str += std::to_string(y);
        ImGui::Text(str.c_str());
        str = "\nZ: ";
        str += std::to_string(z);
        ImGui::Text(str.c_str());

        m_Zone->GetRotation(x, y, z);
        str = "\nrX: ";
        str += std::to_string(x);
        ImGui::Text(str.c_str());
        str = "\nrY: ";
        str += std::to_string(y);
        ImGui::Text(str.c_str());
        str = "\nrZ: ";
        str += std::to_string(z);
        ImGui::Text(str.c_str());

        m_Zone->GetCulling(x, y, z);
        str = "\nPolygons: ";
        str += std::to_string((int)x);
        ImGui::Text(str.c_str());
        str = "\nRendered: ";
        str += std::to_string((int)y);
        ImGui::Text(str.c_str());
        str = "\nCulled: ";
        str += std::to_string((int)z);
        ImGui::Text(str.c_str());


        ImGui::End();
    }

    // Do the input frame processing.
    result = m_Input->Frame();
    if (!result)
        return false;

    // Check if the user pressed escape and wants to exit the application.
    if (m_Input->IsEscapePressed() == true)
        return false;

    ImGui::Render();

    // Do the zone frame processing.
    result = m_Zone->Frame(m_Direct3D, m_Input, m_ShaderManager, m_TextureManager, m_Timer->GetTime(), m_Fps->GetFps(), scales, detailScale, XMFLOAT3(lightX, lightY, lightZ));
    if (!result)
        return false;

    return result;

}
