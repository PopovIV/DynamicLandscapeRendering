//--------------------------------------------------------------------------------------
// User interface class
//--------------------------------------------------------------------------------------
#ifndef _USERINTERFACE_H_
#define _USERINTERFACE_H_

#include "text.h"

class UserInterface {
public:
    // constructors
    UserInterface();
    UserInterface(const UserInterface& other) {};
    // destructor
    ~UserInterface() {};

    // Function to initialize user interface
    bool Initialize(D3DClass* Direct3D, int screenHeight, int screenWidth);
    // Function to realese user interface
    void Shutdown();

    // Proccess frame functiion
    bool Frame(ID3D11DeviceContext* deviceContext, int fps, float posX, float posY, float posZ, float rotX, float rotY, float rotZ);
    // Render fucntion
    bool Render(D3DClass* Direct3D, ShaderManager* ShaderManager, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix);

private:
    // Function to update fps
    bool UpdateFpsString(ID3D11DeviceContext* deviceContext, int fps);
    // Function to updat position of text
    bool UpdatePositionStrings(ID3D11DeviceContext* deviceContext, float posX, float posY, float posZ, float rotX, float rotY, float rotZ);

private:
    Font* m_Font1;
    Text* m_FpsString, * m_VideoStrings, * m_PositionStrings;
    int m_previousFps;
    int m_previousPosition[6];
};

#endif
