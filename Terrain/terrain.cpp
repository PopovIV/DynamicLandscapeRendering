#include "terrain.h"

Terrain::Terrain() {

    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_terrainFilename = nullptr;
    m_heightMap = nullptr;

}

// Function to initialize the vertex and index buffers
bool Terrain::Initialize(ID3D11Device * device, char* setupFilename) {

    bool result;

    result = LoadSetupFile(setupFilename);
    if (!result)
        return false;

    result = LoadHeightMap();
    if (!result)
        return false;

    // Load the rendering buffers with the terrain data.
    result = InitializeBuffers(device);
    if (!result)
        return false;


    return true;

}

// Function to read setup file
bool Terrain::LoadSetupFile(char* filename) {

    int stringLength;
    ifstream fin;
    char input;

    // Initialize the string that will hold the terrain file name.
    stringLength = 256;
    m_terrainFilename = new char[stringLength];
    if (!m_terrainFilename)
        return false;
    fin.open(filename);
    if (fin.fail())
        return false;
    // Read up to the terrain file name.
    fin.get(input);
    while (input != ':')
        fin.get(input);
    // Read in the terrain file name.
    fin >> m_terrainFilename;

    // Read up to the value of terrain height.
    fin.get(input);
    while (input != ':')
        fin.get(input);
    // Read in the terrain height.
    fin >> m_terrainHeight;

    // Read up to the value of terrain width.
    fin.get(input);
    while (input != ':')
        fin.get(input);
    // Read in the terrain width.
    fin >> m_terrainWidth;

    // Read up to the value of terrain tessellation factor.
    fin.get(input);
    while (input != ':')
        fin.get(input);
    // Read in the terrain width.
    fin >> m_tessFactor;

    // Read up to the value of terrain height scaling.
    fin.get(input);
    while (input != ':')
        fin.get(input);
    // Read in the terrain height scaling.
    fin >> m_heightScale;

    // Close the setup file.
    fin.close();

    return true;

}

// Function to load height map
bool Terrain::LoadHeightMap() {
    unsigned error = lodepng_decode32_file(&m_heightMap, &m_terrainHeight, &m_terrainWidth, m_terrainFilename);
    if (error) {
        return false;
    }
    return true;
}

// Function to clear all data from vertex and index buffers
void Terrain::Shutdown() {

    // Release the rendering buffers.
    ShutdownBuffers();
    // Release the height map.
    ShutdownHeightMap();

}

// Render function
bool Terrain::Render(ID3D11DeviceContext* deviceContext) {

    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
    RenderBuffers(deviceContext);

    return true;

}


// Function to initialize buffers
bool Terrain::InitializeBuffers(ID3D11Device* device) {

    unsigned long* indices;
    VertexType* vertices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    int i, j, terrainWidth, terrainHeight, index;
    XMFLOAT4 color;
    float positionX, positionZ;

    int scalePatchX = m_terrainWidth / m_tessFactor;
    int scalePatchY = m_terrainHeight / m_tessFactor;
    // Calculate the number of vertices in the terrain.
    m_vertexCount = (int)(scalePatchX * scalePatchY);

    // Set the index count to the same as the vertex count.
    m_indexCount = (scalePatchX - 1) * (scalePatchY - 1) * 4;

    //// Create the vertex array.
    vertices = new VertexType[m_vertexCount];
    if (!vertices)
        return false;

    for (int j = 0; j < scalePatchY; j++) {
        for (int i = 0; i < scalePatchX; i++) {
            index = j * scalePatchX + i;
            
            vertices[index].position.x = (float)i * m_tessFactor;

            vertices[index].position.z = -(float)j * m_tessFactor;
            vertices[index].position.z += (float)(m_terrainHeight - 1);

            vertices[index].position.y = m_heightMap[(j * m_terrainHeight * m_tessFactor + i * m_tessFactor) * 4];
            vertices[index].position.y *= m_heightScale;
  
        }
    }

    //// Create the index array.
    indices = new unsigned long[m_indexCount];
    if (!indices)
        return false;

    int k = 0;
    for (j = 0; j < (scalePatchY - 1); j++) {
        for (i = 0; i < (scalePatchX - 1); i++) {
            // Get the indexes to the four points of the quad.
            int vert0 = (scalePatchX * j) + i;          // Upper left.
            int vert1 = (scalePatchX * j) + (i + 1);      // Upper right.
            int vert2 = (scalePatchX * (j + 1)) + (i + 1);  // Bottom right.
            int vert3 = (scalePatchX * (j + 1)) + i;      // Bottom left.
            indices[k++] = vert3;
            indices[k++] = vert2;
            indices[k++] = vert1;
            indices[k++] = vert0;
        }
    }

    // Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
        return false;

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
        return false;

    // Release the arrays now that the buffers have been created and loaded.
    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;

}

// Function to realese data from buffers
void Terrain::ShutdownBuffers() {

    // Release the index buffer.
    if (m_indexBuffer) {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }

    // Release the vertex buffer.
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }

}

// Release the height map.
void Terrain::ShutdownHeightMap() {
    // Release the height map array.
    if (m_heightMap)
    {
        delete[] m_heightMap;
        m_heightMap = 0;
    }
}

// Function to put data on pipeline
void Terrain::RenderBuffers(ID3D11DeviceContext* deviceContext) {

    unsigned int stride;
    unsigned int offset;

    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case lines.
    deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

}