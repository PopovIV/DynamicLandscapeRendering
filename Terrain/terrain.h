//--------------------------------------------------------------------------------------
// Terrain class encapsulate the model data and rendering functionality for drawing 256x256 line grid
//--------------------------------------------------------------------------------------

#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <stdio.h>
#include "terrainCell.h"
#include "frustum.h"

using namespace std;
using namespace DirectX;

class Terrain {
private:

    struct VertexType {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
        XMFLOAT3 tangent;
        XMFLOAT3 binormal;
        XMFLOAT2 texture2;
    };

    struct HeightMapType {
        float x, y, z;
        float nx, ny, nz;
    };

    // Used for calculating normal vectors
    struct VectorType {
        float x, y, z;
    };

    // Used to calculate tangent and binormal vectors
    struct TempVertexType {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

public:
    // constructors
    Terrain();
    Terrain(const Terrain& other) {};
    // destructor
    ~Terrain() {};

    // Function to initialize the vertex and index buffers
    bool Initialize(ID3D11Device* device, char* setupFilename);
    // Function to clear all data from vertex and index buffers
    void Shutdown();

    void Frame();


    bool RenderCell(ID3D11DeviceContext* deviceContext, int cellId, Frustum* Frustum, bool culling);
    void RenderCellLines(ID3D11DeviceContext* deviceContext, int cellId) { m_TerrainCells[cellId].RenderLineBuffers(deviceContext); };

    int GetCellIndexCount(int cellId) { return m_TerrainCells[cellId].GetIndexCount(); };
    int GetCellLinesIndexCount(int cellId) { return m_TerrainCells[cellId].GetLineBuffersIndexCount(); };
    int GetCellCount() { return m_cellCount; };

    int GetRenderCount() { return m_renderCount; };
    int GetCellsDrawn() { return m_cellsDrawn; };
    int GetCellsCulled() { return m_cellsCulled; };

    bool GetHeightAtPosition(float inputX, float inputZ, float& height);
private:
    // Function to read setup file
    bool LoadSetupFile(char* filename);

    // Function to load raw height map
    bool LoadRawHeightMap();
    // Release the height map.
    void ShutdownHeightMap();
    // Function to set x and z coords of terrain
    void SetTerrainCoordinates();
    // Function to calculate normals from our height map
    bool CalculateNormals();
    // function to create model of terrain
    bool BuildTerrainModel();
    // Release the terrain model
    void ShutdownTerrainModel();

    bool CheckHeightOfTriangle(float x, float z, float& height, float v0[3], float v1[3], float v2[3]);

    // Function to calculate terrain vectors
    void CalculateTerrainVectors();
    // Function to calculate vertexes tangent and binormal
    void CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal);

    bool LoadTerrainCells(ID3D11Device* device);
    void ShutdownTerrainCells();

private:
    int m_terrainHeight, m_terrainWidth, m_vertexCount;
    float m_heightScale;
    char* m_terrainFilename;
    HeightMapType* m_heightMap;
    TerrainCell::ModelType* m_terrainModel;

    TerrainCell* m_TerrainCells;
    int m_cellCount, m_renderCount, m_cellsDrawn, m_cellsCulled;
};

#endif
