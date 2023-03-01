//--------------------------------------------------------------------------------------
// Input class provides high speed method of interfacing with input devics
//--------------------------------------------------------------------------------------
#ifndef _INPUT_H_
#define _INPUT_H_

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>

#include <directxmath.h>
using namespace DirectX;

class Input {
public:
    // constructors
    Input();
    Input(const Input& other) {};
    // destructors
    ~Input() {};

    // Function to initialize interface 
    bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
    // Function to realese interface
    void Shutdown();
    // Function to read the current state into state buffers
    bool Frame();

    // Return position of mouse
    void GetMouseLocation(int&, int&);

    // Function to check if ESC key is pressed
    bool IsEscapePressed();

    // Function to check if left key is pressed
    bool IsLeftPressed();
    // Function to check if right key is pressed
    bool IsRightPressed();
    // Function to check if up key is pressed
    bool IsUpPressed();
    // Function to check if down key is pressed
    bool IsDownPressed();
    // Function to check if A key is pressed
    bool IsPgUpPressed();
    // Function to check if PgDown key is pressed
    bool IsPgDownPressed();
    // Function to check if mouse is used
    XMFLOAT2 IsMouseUsed();
    // Function to check if F1 key is pressed
    bool IsF1Toggled();
    // Function to check if F2 key is pressed
    bool IsF2Toggled();
    // Function to check if F3 key is pressed
    bool IsF3Toggled();
    // Function to check if F4 key is pressed
    bool IsF4Toggled();

private:
    // Function to read the state of keyboard
    bool ReadKeyboard();
    // Function to read the state of mouse
    bool ReadMouse();
    // Function to deal with the changes that happened in the input device
    void ProcessInput();

private:
    IDirectInput8* m_directInput;
    IDirectInputDevice8* m_keyboard;
    IDirectInputDevice8* m_mouse;

    unsigned char m_keyboardState[256];
    DIMOUSESTATE m_mouseState;

    int m_screenWidth, m_screenHeight;
    int m_mouseX, m_mouseY;

    bool m_F1_released;
    bool m_F2_released;
    bool m_F3_released;
    bool m_F4_released;
};

#endif
