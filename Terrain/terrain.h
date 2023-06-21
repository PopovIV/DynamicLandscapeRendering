//--------------------------------------------------------------------------------------
// Terrain class encapsulate the model data and rendering functionality for drawing 256x256 line grid
//--------------------------------------------------------------------------------------

#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <d3d11.h>
#include <directxmath.h>
#include "frustum.h"
#include "defines.h"

using namespace std;
using namespace DirectX;

class Terrain {
  private:
    struct VertexType {
        XMFLOAT3 position;
    };

    struct HeightMapType {
        float x, y, z;
    };

    struct ModelType {
        float x, y, z;
    };

  public:
    // Function to initialize the vertex and index buffers
    bool Initialize(ID3D11Device* device);
    // Function to clear all data from vertex and index buffers
    void Shutdown();
    // Render Function
    void Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); };
    int GetIndexCount() { return m_indexCount; };

  private:
    void ShutdownHeightMap();
    void SetTerrainCoordinates();
    void ShutdownTerrainModel();

    // Function to initialize terrain vertex and index buffer
    bool InitializeBuffers(ID3D11Device* device);
    // Function to shutdown buffers
    void ShutdownBuffers();
    // Function to render buffers
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

    ID3D11Buffer* m_vertexBuffer = nullptr;
    ID3D11Buffer* m_indexBuffer = nullptr;
    int m_vertexCount, m_indexCount;
    HeightMapType* m_heightMap = nullptr;
    ModelType* m_terrainModel = nullptr;
};

#endif
