#include "shadermanager.h"

// Function to initialize ShaderManager
bool ShaderManager::Initialize(ID3D11Device* device, HWND hwnd) {
    // Create the color shader object.
    m_ColorShader = new ColorShader;
    if (!m_ColorShader) {
        return false;
    }

    // Initialize the color shader object.
    bool result = m_ColorShader->Initialize(device, hwnd);
    if (!result) {
        return false;
    }

    // Create the sky dome shader object.
    m_SkyDomeShader = new SkyDomeShader;
    if (!m_SkyDomeShader) {
        return false;
    }

    // Initialize the sky dome shader object.
    result = m_SkyDomeShader->Initialize(device, hwnd);
    if (!result) {
        return false;
    }

    // Create the terrain shader object.
    m_TerrainShader = new TerrainShader;
    if (!m_TerrainShader) {
        return false;
    }

    // Initialize the terrain shader object.
    result = m_TerrainShader->Initialize(device, hwnd);
    if (!result) {
        return false;
    }

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

    // Release the sky dome shader object.
    if (m_SkyDomeShader) {
        m_SkyDomeShader->Shutdown();
        delete m_SkyDomeShader;
        m_SkyDomeShader = nullptr;
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

bool ShaderManager::RenderSkyDomeShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 apexColor, XMFLOAT4 centerColor) {
    return m_SkyDomeShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, apexColor, centerColor);
}

// Function to render for Terrain shader
bool ShaderManager::RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], ID3D11ShaderResourceView* normalMaps[], ID3D11ShaderResourceView* roughMaps[], ID3D11ShaderResourceView* aoMaps[], Light* light, XMFLOAT4 scales, float detailScale, bool normalPass) {
    return m_TerrainShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, cameraPos, textures, normalMaps, roughMaps, aoMaps, light, scales, detailScale, normalPass);
}
