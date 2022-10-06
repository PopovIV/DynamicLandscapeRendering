//--------------------------------------------------------------------------------------
// TerrainCell is class for deviding trrain in cells
//--------------------------------------------------------------------------------------

#ifndef _TERRAINCELLCLASS_H_
#define _TERRAINCELLCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;


class TerrainCell {
private:
    struct ModelType {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
        float tx, ty, tz;
        float bx, by, bz;
        float r, g, b;
    };

    struct VertexType {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
        XMFLOAT3 tangent;
        XMFLOAT3 binormal;
       XMFLOAT3 color;
    };

    struct VectorType {
        float x, y, z;
    };

    struct ColorVertexType {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

public:
    // constructors
    TerrainCell();
    TerrainCell(const TerrainCell& other) {};
    // destructor
    ~TerrainCell() {};

    // Function to initialize terrain cell
    bool Initialize(ID3D11Device* device, void* terrainModelPtr, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth);
    // Function to release cell's data
    void Shutdown();
    // Function to render cell
    void Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); };
    // Function to render bouning box
    void RenderLineBuffers(ID3D11DeviceContext* deviceContext);

    int GetVertexCount() { return m_vertexCount; };
    int GetIndexCount() { return m_indexCount; };
    int GetLineBuffersIndexCount() { return m_lineIndexCount; };
    void GetCellDimensions(float& maxWidth, float& maxHeight, float& maxDepth, float& minWidth, float& minHeight, float& minDepth);

private:
    // Function to initialize vertex list
    bool InitializeBuffers(ID3D11Device* device, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth, ModelType* terrainModel);
    // Function to release buffer's data
    void ShutdownBuffers();
    // Function to render buffers
    void RenderBuffers(ID3D11DeviceContext* deviceContext);
    // Function to calculate cell dimensions
    void CalculateCellDimensions();
    // Function to build bounding box for cell
    bool BuildLineBuffers(ID3D11Device* device);
    // Function to realese bounding box
    void ShutdownLineBuffers();

public:
	VectorType* m_vertexList;

private:
	int m_vertexCount, m_indexCount, m_lineIndexCount;
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer, * m_lineVertexBuffer, * m_lineIndexBuffer;;
	float m_maxWidth, m_maxHeight, m_maxDepth, m_minWidth, m_minHeight, m_minDepth;
	float m_positionX, m_positionY, m_positionZ;
};

#endif