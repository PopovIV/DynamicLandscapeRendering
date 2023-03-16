#include "system.h"
#include "imgui_impl_win32.h"

// Function to initialize aplication instanc and window class
bool System::Initialize() {
    // Initialize the width and height of the screen to zero before sending the variables into the function.
    int screenWidth = 0;
    int screenHeight = 0;

    // Initialize the windows api.
    InitializeWindows(screenWidth, screenHeight);

    // Create the application wrapper object.
    m_Application = new Application;
    if (!m_Application) {
        return false;
    }

    // Initialize the application wrapper object.
    bool result = m_Application->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
    if (!result) {
        return false;
    }

    return true;
}

// Function to clear all stuff that was created in initialize function
void System::Shutdown() {
    // Release the application wrapper object.
    if (m_Application) {
        m_Application->Shutdown();
        delete m_Application;
        m_Application = nullptr;
    }

    // Shutdown the window.
    ShutdownWindows();
}

// Main cycle function. MSG handling happens here
void System::Run() {
    // Initialize the message structure.
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    // Loop until there is a quit message from the window or the user.
    bool done = false;
    while (!done) {
        // Handle the windows messages.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // If windows signals to end the application then exit out.
        if (msg.message == WM_QUIT) {
            done = true;
        }
        else if (msg.message == WM_SIZE) {
            if (m_Application != nullptr) {
                RECT rc;
                GetClientRect(m_hwnd, &rc);
                m_Application->Resize(rc.right - rc.left, rc.bottom - rc.top);
            }
        }
        else{
            // Otherwise do the frame processing.
           bool result = Frame();
           if (!result)
               done = true;
        }
    }
}

// Function to update frame each second
bool System::Frame() {
    // Do the frame processing for the application object.
    bool result = m_Application->Frame();
    if (!result) {
        return false;
    }

    return true;
}


LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
    return DefWindowProc(hwnd, umsg, wparam, lparam);
}

// Function to clear all stuff that was created in initialize windows function
void System::InitializeWindows(int& screenWidth, int& screenHeight) {
    // Get an external pointer to this object.	
    ApplicationHandle = this;

    // Get the instance of this application.
    m_hinstance = GetModuleHandle(NULL);

    // Give the application a name.
    m_applicationName = L"Terrain renderer";

    // Setup the windows class with default settings.
    WNDCLASSEX wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    // Determine the resolution of the clients desktop screen.
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Setup the screen settings depending on whether it is running in full screen or in windowed mode.
    DEVMODE dmScreenSettings;
    int posX, posY;
    if (FULL_SCREEN) {
        // If full screen set the screen to maximum size of the users desktop and 32bit.
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
        dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change the display settings to full screen.
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        // Set the position of the window to the top left corner.
        posX = posY = 0;
    }
    else {
        screenWidth = 1920;
        screenHeight = 1080;

        // Place the window in the middle of the screen.
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // Create the window with the screen settings and get the handle to it.
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr,
        nullptr, m_hinstance, nullptr);

    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
    UpdateWindow(m_hwnd);

    {
        RECT rc;
        rc.left = 0;
        rc.right = screenWidth;
        rc.top = 0;
        rc.bottom = screenHeight;

        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);

        MoveWindow(m_hwnd, 100, 100, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    }
}

// Function to clear all window data
void System::ShutdownWindows() {
    // Fix the display settings if leaving full screen mode.
    if (FULL_SCREEN) {
        ChangeDisplaySettings(NULL, 0);
    }

    // Remove the window.
    DestroyWindow(m_hwnd);
    m_hwnd = NULL;

    // Remove the application instance.
    UnregisterClass(m_applicationName, m_hinstance);
    m_hinstance = NULL;

    // Release the pointer to this class.
    ApplicationHandle = NULL;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam)) {
        return true;
    }

    switch (umessage) {
        // Check if the window is being destroyed.
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        // Check if the window is being closed.
       case WM_CLOSE:
           PostQuitMessage(0);
           return 0;

       // All other messages pass to the message handler in the system class.
       default:
           return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
    }
}
