#include "shadermanager.h"

ShaderManager::ShaderManager() {

    m_ColorShader = nullptr;
    m_FontShader = nullptr;

}

// Function to initialize ShaderManager
bool ShaderManager::Initialize(ID3D11Device* device, HWND hwnd) {

    bool result;

    // Create the color shader object.
    m_ColorShader = new ColorShader;
    if (!m_ColorShader)
        return false;

    // Initialize the color shader object.
    result = m_ColorShader->Initialize(device, hwnd);
    if (!result)
       return false;

    // Create the font shader object.
    m_FontShader = new FontShader;
    if (!m_FontShader)
       return false;

    // Initialize the font shader object.
    result = m_FontShader->Initialize(device, hwnd);
    if (!result)
        return false;

    return true;

}

// Function to realese all stuff in ShaderManager
void ShaderManager::Shutdown() {

    // Release the font shader object.
    if (m_FontShader) {
    m_FontShader->Shutdown();
        delete m_FontShader;
        m_FontShader = nullptr;
    }

    // Release the color shader object.
    if (m_ColorShader) {
        m_ColorShader->Shutdown();
        delete m_ColorShader;
        m_ColorShader = nullptr;
    }

}

// Function to render for Color shader
bool ShaderManager::RenderColorShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
    return m_ColorShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix);
}

// Function to render for Font shader
bool ShaderManager::RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 color) {
    return m_FontShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, color);
}