//--------------------------------------------------------------------------------------
// System class controls message handling
//--------------------------------------------------------------------------------------
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "application.h"

class System {
  public:
    // Function to initialize aplication instanc and window class
    bool Initialize();
    // Function to clear all stuff that was created in initialize function
    void Shutdown();
    // Main cycle function. MSG handling happens here
    void Run();

    LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
  private:
    // Function to update frame each second
    bool Frame();
    // Function to initialize window
    void InitializeWindows(int& screenWidth, int& screenHeight);
    // Function to clear all stuff that was created in initialize windows function
    void ShutdownWindows();

    LPCWSTR m_applicationName = 0;
    HINSTANCE m_hinstance = 0;
    HWND m_hwnd = 0;
    Application* m_Application = nullptr;
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

// GLOBALS
static System* ApplicationHandle = 0;

#endif
