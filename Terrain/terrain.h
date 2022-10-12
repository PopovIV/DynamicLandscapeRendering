//--------------------------------------------------------------------------------------
// Terrain class encapsulate the model data and rendering functionality for drawing 256x256 line grid
//--------------------------------------------------------------------------------------

#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <stdio.h>
#include "DDSTextureLoader.h"
#include "stringConverter.h"
using namespace std;
using namespace DirectX;

class Terrain {
private:

    struct VertexType {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

    struct HeightMapType {
        float x, y, z;
    };

    struct ModelType {
        float x, y, z;
        float tu, tv;
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
    // Function to load height map
    bool LoadBitmapHeightMap();
    // Release the height map.
    void ShutdownHeightMap();
    // Function to set x and z coords of terrain
    void SetTerrainCoordinates();
    // function to create model of terrain
    bool BuildTerrainModel();
    // Release the terrain model
    void ShutdownTerrainModel();

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