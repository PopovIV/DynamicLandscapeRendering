//--------------------------------------------------------------------------------------
// Terrain class encapsulate the model data and rendering functionality for drawing 256x256 line grid
//--------------------------------------------------------------------------------------

#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <stdio.h>

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

    struct ModelType {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
        float tx, ty, tz;
        float bx, by, bz;
        float tu2, tv2;
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
    // Render function
    bool Render(ID3D11DeviceContext* deviceContex);
    // Function to get nimber of indexes in the terrain grid
    int GetIndexCount() { return m_indexCount; };

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

    // Function to calculate terrain vectors
    void CalculateTerrainVectors();
    // Function to calculate vertexes tangent and binormal
    void CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal);

    // Function to initialize buffers
    bool InitializeBuffers(ID3D11Device* device);
    // Function to realese data from buffers
    void ShutdownBuffers();
    // Function to put data on pipeline
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
    ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
    int m_vertexCount, m_indexCount;

    int m_terrainHeight, m_terrainWidth;
    float m_heightScale;
    char* m_terrainFilename;
    HeightMapType* m_heightMap;
    ModelType* m_terrainModel;

};

#endif