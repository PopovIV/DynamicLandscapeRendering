//--------------------------------------------------------------------------------------
// Terrain class encapsulate the model data and rendering functionality for drawing 256x256 line grid
//--------------------------------------------------------------------------------------

#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class Terrain {
private:

    struct VertexType {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

public:
    // constructors
    Terrain();
    Terrain(const Terrain& other) {};
    // destructor
    ~Terrain() {};

    // Function to initialize the vertex and index buffers
    bool Initialize(ID3D11Device* device);
    // Function to clear all data from vertex and index buffers
    void Shutdown();
    // Render function
    bool Render(ID3D11DeviceContext* deviceContex);

    // Function to get nimber of indexes in the terrain grid
    int GetIndexCount() { return m_indexCount; };

private:
    // Function to initialize buffers
    bool InitializeBuffers(ID3D11Device* device);
    // Function to realese data from buffers
    void ShutdownBuffers();
    // Function to put data on pipeline
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
    ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
    int m_vertexCount, m_indexCount;
};

#endif