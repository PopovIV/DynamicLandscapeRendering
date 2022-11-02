//--------------------------------------------------------------------------------------
// ShaderManager class manages multiple shaders
//--------------------------------------------------------------------------------------
#ifndef _SHADERMANAGER_H_
#define _SHADERMANAGER_H_

#include "d3dclass.h"
#include "fontshader.h"
#include "terrainshader.h"

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

    // Function to render for Font shader
    bool RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 color);
    // Function to render for Terrain shader
    bool RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], ID3D11ShaderResourceView* normalMaps[], XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

private:

    FontShader* m_FontShader;
    TerrainShader* m_TerrainShader;

};

#endif