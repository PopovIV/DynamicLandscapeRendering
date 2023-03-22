#include "input.h"

// Function to initialize interface 
bool Input::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
    // Store the screen size which will be used for positioning the mouse cursor.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // Initialize the location of the mouse on the screen.
    m_mouseX = 0;
    m_mouseY = 0;
    // Initialize the main direct input interface.
    HRESULT result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
    if (FAILED(result)) {
        return false;
    }

    // Initialize the direct input interface for the keyboard.
    result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
    if (FAILED(result)) {
        return false;
    }

    // Set the data format.  In this case since it is a keyboard we can use the predefined data format.
    result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(result)) {
        return false;
    }

    // Set the cooperative level of the keyboard to not share with other programs.
    result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    if (FAILED(result)) {
        return false;
    }

    // Now acquire the keyboard.
    result = m_keyboard->Acquire();
    if (FAILED(result)) {
        return false;
    }

    // Initialize the direct input interface for the mouse.
    result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
    if (FAILED(result)) {
        return false;
    }

    // Set the data format for the mouse using the pre-defined mouse data format.
    result = m_mouse->SetDataFormat(&c_dfDIMouse);
    if (FAILED(result)) {
        return false;
    }

    // Set the cooperative level of the mouse to share with other programs.
    result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(result)) {
        return false;
    }

    // Acquire the mouse.
    result = m_mouse->Acquire();
    if (FAILED(result)) {
        return false;
    }

    return true;
}

// Resize function
void Input::Resize(int width, int height) {
    m_screenWidth = width;
    m_screenHeight = height;
}

// Function to realese interface
void Input::Shutdown() {
    // Release the mouse.
    if (m_mouse) {
        m_mouse->Unacquire();
        m_mouse->Release();
        m_mouse = nullptr;
    }

    // Release the keyboard.
    if (m_keyboard) {
        m_keyboard->Unacquire();
        m_keyboard->Release();
        m_keyboard = nullptr;
    }

    // Release the main interface to direct input.
    if (m_directInput) {
        m_directInput->Release();
        m_directInput = nullptr;
    }
}

// Function to read the current state into state buffers
bool Input::Frame() {
    // Read the current state of the keyboard.
    bool result = ReadKeyboard();
    if (!result) {
        return false;
    }

    // Read the current state of the mouse.
    result = ReadMouse();
    if (!result) {
        return false;
    }

    // Process the changes in the mouse and keyboard.
    ProcessInput();

    return true;
}

// Function to read the state of keyboard
bool Input::ReadKeyboard() {
    // Read the keyboard device.
    HRESULT result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
    if (FAILED(result)) {
        // If the keyboard lost focus or was not acquired then try to get control back.
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
            m_keyboard->Acquire();
        }
        else {
            return false;
        }
    }

    return true;
}

// Function to read the state of mouse
bool Input::ReadMouse() {
    // Read the mouse device.
    HRESULT result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
    if (FAILED(result)) {
        // If the mouse lost focus or was not acquired then try to get control back.
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
            m_mouse->Acquire();
        }
        else {
            return false;
        }
    }

    return true;
}

// Function to deal with the changes that happened in the input device
void Input::ProcessInput() {
    // Update the location of the mouse cursor based on the change of the mouse location during the frame.
    m_mouseX += m_mouseState.lX;
    m_mouseY += m_mouseState.lY;

    // Ensure the mouse location doesn't exceed the screen width or height.
    if (m_mouseX < 0) { m_mouseX = 0; }
    if (m_mouseY < 0) { m_mouseY = 0; }

    if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
    if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
}

// Function to check if ESC key is pressed
bool Input::IsEscapePressed() {
    // Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
    if (m_keyboardState[DIK_ESCAPE] & 0x80) {
        return true;
    }

    return false;
}

// Return position of mouse
void Input::GetMouseLocation(int& mouseX, int& mouseY) {
    mouseX = m_mouseX;
    mouseY = m_mouseY;
}

// Function to check if left key is pressed
bool Input::IsLeftPressed() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_LEFT] || m_keyboardState[DIK_A] & 0x80) {
        return true;
    }

    return false;
}

// Function to check if right key is pressed
bool Input::IsRightPressed() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_RIGHT] || m_keyboardState[DIK_D] & 0x80) {
        return true;
    }

    return false;
}

// Function to check if up key is pressed
bool Input::IsUpPressed() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_UP] || m_keyboardState[DIK_W] & 0x80) {
        return true;
    }

    return false;
}

// Function to check if down key is pressed
bool Input::IsDownPressed() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_DOWN] || m_keyboardState[DIK_S] & 0x80) {
        return true;
    }

    return false;
}

// Function to check if PgUP key is pressed
bool Input::IsPgUpPressed() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_PGUP] & 0x80) {
        return true;
    }

    return false;
}

// Function to check if PgDown key is pressed
bool Input::IsPgDownPressed() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_PGDN] & 0x80) {
        return true;
    }

    return false;
}

// Function to check if mouse is used
XMFLOAT2 Input::IsMouseUsed() {
    if (m_mouseState.rgbButtons[1] || m_mouseState.rgbButtons[2] & 0x80) {
        return XMFLOAT2(m_mouseState.lX, m_mouseState.lY);
    }

    return XMFLOAT2(0.0f, 0.0f);
};

// Function to check if F1 key is pressed
bool Input::IsF1Toggled() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_F1] & 0x80) {
        if (m_F1_released) {
            m_F1_released = false;
            return true;
        }
    }
    else {
        m_F1_released = true;
    }

    return false;
}

// Function to check if F2 key is pressed
bool Input::IsF2Toggled() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_F2] & 0x80) {
        if (m_F2_released) {
            m_F2_released = false;
            return true;
        }
    }
    else {
        m_F2_released = true;
    }

    return false;
}

// Function to check if F3 key is pressed
bool Input::IsF3Toggled() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_F3] & 0x80) {
        if (m_F3_released) {
            m_F3_released = false;
            return true;
        }
    }
    else {
        m_F3_released = true;
    }

    return false;
}

// Function to check if F3 key is pressed
bool Input::IsF4Toggled() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_F4] & 0x80) {
        if (m_F4_released) {
            m_F4_released = false;
            return true;
        }
    }
    else {
        m_F4_released = true;
    }

    return false;
}

// Function to check if Space key is pressed
bool Input::IsSpaceToggled() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_SPACE] & 0x80) {
        if (m_Space_released) {
            m_Space_released = false;
            return true;
        }
    }
    else {
        m_Space_released = true;
    }

    return false;
}
