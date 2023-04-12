//--------------------------------------------------------------------------------------
// ShaderManager class manages multiple shaders
//--------------------------------------------------------------------------------------

#ifndef _SHADERMANAGER_H_
#define _SHADERMANAGER_H_

#include "d3dclass.h"
#include "skydomeshader.h"
#include "terrainshader.h"

class ShaderManager {
public:
    // Function to initialize ShaderManager
    bool Initialize(ID3D11Device* device, HWND hwnd);
    // Function to realese all stuff in ShaderManager
    void Shutdown();

    // Function to render for SkyDome shader
    bool RenderSkyDomeShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 apexColor, XMFLOAT4 centerColor);
    // Function to render for Terrain shader
    bool RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], ID3D11ShaderResourceView* normalMaps[], ID3D11ShaderResourceView* roughMaps[], ID3D11ShaderResourceView* aoMaps[], Light* light, XMFLOAT4 scales, float detailScale, bool normalPass = true);

private:
    SkyDomeShader* m_SkyDomeShader = nullptr;
    TerrainShader* m_TerrainShader = nullptr;
};

#endif
