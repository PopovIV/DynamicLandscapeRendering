#include "shadermanager.h"

ShaderManager::ShaderManager() {

    m_ColorShader = nullptr;
    m_TextureShader = nullptr;
    m_LightShader = nullptr;
    m_FontShader = nullptr;
    m_TerrainShader = nullptr;

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

    // Create the texture shader object.
    m_TextureShader = new TextureShader;
    if (!m_TextureShader)
        return false;

    // Initialize the texture shader object.
    result = m_TextureShader->Initialize(device, hwnd);
    if (!result)
        return false;

    // Create the light shader object.
    m_LightShader = new LightShader;
    if (!m_LightShader)
        return false;

    // Initialize the light shader object.
    result = m_LightShader->Initialize(device, hwnd);
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

    // Create the terrain shader object.
    m_TerrainShader = new TerrainShader;
    if (!m_TerrainShader)
        return false;

    // Initialize the terrain shader object.
    result = m_TerrainShader->Initialize(device, hwnd);
    if (!result)
        return false;

    return true;

}

// Function to realese all stuff in ShaderManager
void ShaderManager::Shutdown() {

    // Release the terrain shader object.
    if (m_TerrainShader) {
        m_TerrainShader->Shutdown();
        delete m_TerrainShader;
        m_TerrainShader = nullptr;
    }

    // Release the font shader object.
    if (m_FontShader) {
    m_FontShader->Shutdown();
        delete m_FontShader;
        m_FontShader = nullptr;
    }

    // Release the light shader object.
    if (m_LightShader) {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = nullptr;
    }

    // Release the texture shader object.
    if (m_TextureShader) {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = nullptr;
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

// Function to render for Texture shader
bool ShaderManager::RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,  XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture) {
    return m_TextureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

// Function to render for Light shader
bool ShaderManager::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor) {
    return m_LightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
}

// Function to render for Font shader
bool ShaderManager::RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 color) {
    return m_FontShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, color);
}

// Function to render for Terrain shader
bool ShaderManager::RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], ID3D11ShaderResourceView* normalMaps[], ID3D11ShaderResourceView* roughMaps[], ID3D11ShaderResourceView* aoMaps[], Light* light, XMFLOAT4 scales) {
    return m_TerrainShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, cameraPos, textures, normalMaps, roughMaps, aoMaps, light, scales);
}