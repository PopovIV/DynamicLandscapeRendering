//--------------------------------------------------------------------------------------
// ShaderManager class manages multiple shaders
//--------------------------------------------------------------------------------------
#ifndef _SHADERMANAGER_H_
#define _SHADERMANAGER_H_

#include "d3dclass.h"
#include "colorshader.h"
#include "fontshader.h"


class ShaderManager {
public:
    // constructors
    ShaderManager();
    ShaderManager(const ShaderManager& other) {};
    // destructor
    ~ShaderManager() {};

    // Function to initialize ShaderManager
    bool Initialize(ID3D11Device* device, HWND hwnd);
    // Function to realese all stuff in ShaderManager
    void Shutdown();

    // Function to render for Color shader
    bool RenderColorShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
    // Function to render for Font shader
    bool RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 color);

private:
    ColorShader* m_ColorShader;
    FontShader* m_FontShader;
};

#endif