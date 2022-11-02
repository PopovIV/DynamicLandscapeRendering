#include "shadermanager.h"

ShaderManager::ShaderManager() {

    m_FontShader = nullptr;
    m_TerrainShader = nullptr;

}

// Function to initialize ShaderManager
bool ShaderManager::Initialize(ID3D11Device* device, HWND hwnd) {

    bool result;

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

}

// Function to render for Font shader
bool ShaderManager::RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 color) {
    return m_FontShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, color);
}

// Function to render for Terrain shader
bool ShaderManager::RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], ID3D11ShaderResourceView* normalMaps[], XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor) {
    return m_TerrainShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, cameraPos, textures, normalMaps, lightDirection, diffuseColor);
}