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
using namespace std;
using namespace DirectX;

class Terrain {
private:

    struct HeightMapType {
        float x, y, z;
        float nx, ny, nz;
        float r, g, b;
    };

    struct ModelType {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
        float tx, ty, tz;
        float bx, by, bz;
        float r, g, b;
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
    // Render cell function
    bool RenderCell(ID3D11DeviceContext* deviceContext, int cellId);
    // Render cell bounding box function
    void RenderCellLines(ID3D11DeviceContext* deviceContext, int cellId) { m_TerrainCells[cellId].RenderLineBuffers(deviceContext); };

    int GetCellIndexCount(int cellId) { return m_TerrainCells[cellId].GetIndexCount(); };
    int GetCellLinesIndexCount(int cellId) { return m_TerrainCells[cellId].GetLineBuffersIndexCount(); };
    int GetCellCount() { return m_cellCount; };

private:
    // Function to read setup file
    bool LoadSetupFile(char* filename);
    // Function to load height map
    bool LoadBitmapHeightMap();
    // Function to load raw height map
    bool LoadRawHeightMap();
    // Release the height map.
    void ShutdownHeightMap();
    // Function to set x and z coords of terrain
    void SetTerrainCoordinates();
    // Function to calculate normals from our height map
    bool CalculateNormals();
    // Function to load color bitmap
    bool LoadColorMap();
    // function to create model of terrain
    bool BuildTerrainModel();
    // Release the terrain model
    void ShutdownTerrainModel();

    // Function to calculate terrain vectors
    void CalculateTerrainVectors();
    // Function to calculate vertexes tangent and binormal
    void CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal);

    // Function to create the array of cell objects
    bool LoadTerrainCells(ID3D11Device* device);
    // Function to release cell's data
    void ShutdownTerrainCells();

private:
    int m_terrainHeight, m_terrainWidth, m_vertexCount;
    float m_heightScale;
    char* m_terrainFilename, * m_colorMapFilename;
    HeightMapType* m_heightMap;
    ModelType* m_terrainModel;

    TerrainCell* m_TerrainCells;
    int m_cellCount;

};

#endif