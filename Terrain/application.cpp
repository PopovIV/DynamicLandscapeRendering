#include "application.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// Function to initialize application instance and all it's subclasses
bool Application::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
    // Create the input object.
    m_Input = new Input;
    if (!m_Input) {
        return false;
    }

    // Initialize the input object.
    bool result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
        return false;
    }

    // Create the Direct3D object.
    m_Direct3D = new D3DClass;
    if (!m_Direct3D) {
        return false;
    }

    // Initialize the Direct3D object.
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    // Create the shader manager object.
    m_ShaderManager = new ShaderManager;
    if (!m_ShaderManager) {
        return false;
    }

    // Initialize the shader manager object.
    result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the shader manager object.", L"Error", MB_OK);
        return false;
    }

    // Create the texture manager object.
    m_TextureManager = new TextureManager;
    if (!m_TextureManager) {
        return false;
    }

    // Initialize the texture manager object.
    result = m_TextureManager->Initialize(30);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the texture manager object.", L"Error", MB_OK);
        return false;
    }

    // Load textures into the texture manager.
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_diffuse.dds", 0, Texture::DDS, true);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_normal.dds", 1, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_rough.dds", 2, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_ao.dds", 3, Texture::DDS);
    if (!result) {
        return false;
    }

    // Load textures into the texture manager.
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_diffuse2.dds", 4, Texture::DDS, true);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_normal2.dds", 5, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_rough2.dds", 6, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/grass_ao2.dds", 7, Texture::DDS);
    if (!result) {
        return false;
    }

    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/rock_diffuse.dds", 8, Texture::DDS, true);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/rock_normal.dds", 9, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/rock_rough.dds", 10, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/rock_ao.dds", 11, Texture::DDS);
    if (!result) {
        return false;
    }

    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/slope_diffuse.dds", 12, Texture::DDS, true);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/slope_normal.dds", 13, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/slope_rough.dds", 14, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/slope_ao.dds", 15, Texture::DDS);
    if (!result) {
        return false;
    }

    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/snow_diffuse.dds", 16, Texture::DDS, true);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/snow_normal.dds", 17, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/snow_rough.dds", 18, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/snow_ao.dds", 19, Texture::DDS);
    if (!result) {
        return false;
    }

    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/detailNormalMap.dds", 20, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/noise.dds", 21, Texture::DDS);
    if (!result) {
        return false;
    }
    result = m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"data/textures/HM.r32", 22, Texture::R32);
    if (!result) {
        return false;
    }

    // Create the timer object.
    m_Timer = new Timer;
    if (!m_Timer) {
        return false;
    }

    // Initialize the timer object.
    result = m_Timer->Initialize();
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
        return false;
    }

    // Create the fps object.
    m_Fps = new Fps;
    if (!m_Fps) {
        return false;
    }

    // Initialize the fps object.
    m_Fps->Initialize();

    // Create the zone object.
    m_Zone = new Zone;
    if (!m_Zone) {
        return false;
    }

    // Initialize the zone object.
    result = m_Zone->Initialize(m_Direct3D, hwnd, m_TextureManager, screenWidth, screenHeight, SCREEN_NEAR);
    if (!result) {
        MessageBox(hwnd, L"Could not initialize the zone object.", L"Error", MB_OK);
        return false;
    }

    m_width = screenWidth;
    m_height = screenHeight;

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

// Resize function
void Application::Resize(int width, int height) {
    if (width != m_width || height != m_height) {
        m_Input->Resize(width, height);
        m_Direct3D->Resize(width, height, SCREEN_DEPTH, SCREEN_NEAR);
        m_Zone->Resize(m_Direct3D->GetDevice(), width, height);
        m_width = width;
        m_height = height;
    }
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

bool Application::LoadCamera(XMFLOAT3& pos, XMFLOAT3& rot) {
    try {
        std::ifstream infile(configName);
        infile >> pos.x;
        infile >> pos.y;
        infile >> pos.z;
        infile >> rot.x;
        infile >> rot.y;
        infile >> rot.z;
        infile.close();
    }
    catch (ifstream::failure) {
        return false;
    }
    return true;
}

bool Application::SaveCamera(XMFLOAT3 pos, XMFLOAT3 rot) {
    try {
        std::ofstream offile(configName);
        offile << pos.x << '\n';
        offile << pos.y << '\n';
        offile << pos.z << '\n';
        offile << rot.x << '\n';
        offile << rot.y << '\n';
        offile << rot.z << '\n';
        offile.close();
    }
    catch (ofstream::failure) {
        return false;
    }
    return true;
}

// Function to update frame each second
bool Application::Frame() {

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
    static bool lockView = true;
    static bool myWindow = true;
    static TerrainShader::ScaleBufferType scales = { {40, 32, 32, 32}, {4, 0, 0, 0} };
    static int lightX = (int)m_Zone->GetLighDirection().x;
    static int lightY = (int)m_Zone->GetLighDirection().y;
    static int lightZ = (int)m_Zone->GetLighDirection().z;
    if (demoWindow) {
        ImGui::ShowDemoWindow(&demoWindow);
    }

    if (myWindow) {
        ImGui::Begin("ImGui", &myWindow);
        
        dayNight = m_Zone->GetDayNight();
        wireframe = m_Zone->GetWireFrame();
        lockView = m_Zone->GetLockView();

        if (ImGui::Button("Open demo")) {
            demoWindow = true;
        }

        if (ImGui::Button("Save camera")) {
            float rotX, rotY, rotZ;
            float posX, posY, posZ;
            m_Zone->GetPosition(posX, posY, posZ);
            m_Zone->GetRotation(rotX, rotY, rotZ);
            bool result = SaveCamera(XMFLOAT3(posX, posY, posZ), XMFLOAT3(rotX, rotY, rotZ));
            if (!result) {
                ImGui::SameLine();
                ImGui::Text("Error saving camera");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Load camera")) {
            XMFLOAT3 pos, rot;
            bool result = LoadCamera(pos, rot);
            if (result) {
                m_Zone->SetPosition(pos.x, pos.y, pos.z);
                m_Zone->SetRotation(rot.x, rot.y, rot.z);
            }
            else {
                ImGui::Text("Error loading camera");
            }
        }

        if (ImGui::Checkbox("WireFrame Mode", &wireframe)) {
            m_Zone->ToggleWireFrame();
        }

        if (ImGui::Checkbox("Height locked", &heightLocked)) {
            m_Zone->ToggleHeightLocked();
        }

        if (ImGui::Checkbox("DayNight Cycle", &dayNight)) {
            m_Zone->ToggleDayNight();
        }

        if (ImGui::Checkbox("Culling", &culling)) {
            m_Zone->ToggleCulling();
        }

        if (culling) {
            if (ImGui::Checkbox("Lock View", &lockView)) {
                m_Zone->ToggleLockView();
            }
        }
        
        ImGui::SliderInt("Grass scales", &scales.scales.x, 1, 64, "%d", 0);
        ImGui::SliderInt("Rock scales", &scales.scales.y, 1, 64, "%d", 0);
        ImGui::SliderInt("Slope scales", &scales.scales.z, 1, 64, "%d", 0);
        ImGui::SliderInt("Snow scales", &scales.scales.w, 1, 64, "%d", 0);
        ImGui::SliderInt("Detail scales", &scales.detailScale.x, 1, 64, "%d", 0);

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

        str = "\nRendered: ";
        str += std::to_string((int)m_Zone->GetNumRendered());
        ImGui::Text(str.c_str());
        str = "\nCulled: ";
        str += std::to_string((int)m_Zone->GetNumCulled());
        ImGui::Text(str.c_str());

        str = "\nDraw To Texture Time: ";
        str += std::to_string(m_Zone->GetDrawToTextureTime()) + " ms";
        ImGui::Text(str.c_str());
        str = "\nTone Mapping Time: ";
        str += std::to_string(m_Zone->GetToneMappingTime()) + " ms";
        ImGui::Text(str.c_str());
        str = "\nGPU Time: ";
        str += std::to_string(m_Zone->GetDrawTime()) + " ms";
        ImGui::Text(str.c_str());
        str = "\nCPU Time: ";
        str += std::to_string(m_Zone->GetCPUTime()) + " ms";
        ImGui::Text(str.c_str());

        ImGui::End();
    }

    // Do the input frame processing.
    bool result = m_Input->Frame();
    if (!result) {
        return false;
    }

    // Check if the user pressed escape and wants to exit the application.
    if (m_Input->IsEscapePressed() == true) {
        return false;
    }

    ImGui::Render();

    // Do the zone frame processing.
    result = m_Zone->Frame(m_Direct3D, m_Input, m_ShaderManager, m_TextureManager, m_Timer->GetTime(), scales, XMFLOAT3((float)lightX, (float)lightY, (float)lightZ));
    if (!result) {
        return false;
    }

    return result;
}
