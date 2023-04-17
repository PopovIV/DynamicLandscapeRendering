#include "shadermanager.h"

// Function to initialize ShaderManager
bool ShaderManager::Initialize(ID3D11Device* device, HWND hwnd) {
    // Create the sky dome shader object.
    m_SkyDomeShader = new SkyDomeShader;
    if (!m_SkyDomeShader) {
        return false;
    }

    // Initialize the sky dome shader object.
    HRESULT result = m_SkyDomeShader->Initialize(device, hwnd);
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

}

bool ShaderManager::RenderSkyDomeShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 apexColor, XMFLOAT4 centerColor) {
    return m_SkyDomeShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, apexColor, centerColor);
}

// Function to render for Terrain shader
bool ShaderManager::RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount, XMFLOAT4* frustumPlanes, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], Light* light, TerrainShader::ScaleBufferType scaleBuffer, bool normalPass) {
    return m_TerrainShader->Render(deviceContext, indexCount, frustumPlanes, worldMatrix, viewMatrix, projectionMatrix, cameraPos, textures, light, scaleBuffer, normalPass);
}
