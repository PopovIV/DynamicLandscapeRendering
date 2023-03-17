//--------------------------------------------------------------------------------------
// TerrainCell class is used for splitting terrain in cells
//--------------------------------------------------------------------------------------
#ifndef _TERRAINCELLC_H_
#define _TERRAINCELL_H_

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class TerrainCell {
  private:
    struct VertexType {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
        XMFLOAT3 tangent;
        XMFLOAT3 binormal;
    };

    struct ModelType {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
        float tx, ty, tz;
        float bx, by, bz;
    };

    struct VectorType {
        float x, y, z;
    };

    struct ColorVertexType {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

  public:
    bool Initialize(ID3D11Device* device, void* terrainModelPtr, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth);
    void Shutdown();
    void Render(ID3D11DeviceContext* deviceContext) { RenderBuffers(deviceContext); };
    void RenderLineBuffers(ID3D11DeviceContext* deviceContext);

    int GetVertexCount() { return m_vertexCount; };
    int GetIndexCount() { return m_indexCount; };
    int GetLineBuffersIndexCount() { return m_lineIndexCount; };
    void GetCellDimensions(float& maxWidth, float& maxHeight, float& maxDepth, float& minWidth, float& minHeight, float& minDepth);

  private:
    bool InitializeBuffers(ID3D11Device* device, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth, ModelType* terrainModel);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* deviceContext);
    void CalculateCellDimensions();
    bool BuildLineBuffers(ID3D11Device* device);
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
