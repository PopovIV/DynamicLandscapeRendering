#include "skydome.h"

bool SkyDome::Initialize(ID3D11Device* device) {
    // Load in the sky dome model.
    bool result = LoadSkyDomeModel((char*)"data/skydome.txt");
    if (!result) {
        return false;
    }

    // Load the sky dome into a vertex and index buffer for rendering.
    result = InitializeBuffers(device);
    if (!result) {
        return false;
    }

    // Set the color at the top of the sky dome.
    m_apexColor = XMFLOAT4(0.0f, 0.05f, 0.6f, 1.0f);

    // Set the color at the center of the sky dome.
    m_centerColor = XMFLOAT4(0.0f, 0.5f, 0.8f, 1.0f);

    return true;
}

void SkyDome::Shutdown() {
    // Release the vertex and index buffer that were used for rendering the sky dome.
    ReleaseBuffers();

    // Release the sky dome model.
    ReleaseSkyDomeModel();
}

bool SkyDome::LoadSkyDomeModel(char* filename) {
    // Open the model file.
    ifstream fin;
    fin.open(filename);

    // If it could not open the file then exit.
    if (fin.fail())
        return false;

    // Read up to the value of vertex count.
    char input;
    fin.get(input);
    while (input != ':')
        fin.get(input);

    // Read in the vertex count.
    fin >> m_vertexCount;

    // Set the number of indices to be the same as the vertex count.
    m_indexCount = m_vertexCount;

    // Create the model using the vertex count that was read in.
    m_model = new ModelType[m_vertexCount];
    if (!m_model)
        return false;

    // Read up to the beginning of the data.
    fin.get(input);
    while (input != ':')
        fin.get(input);
    fin.get(input);
    fin.get(input);

    // Read in the vertex data.
    for (int i = 0; i < m_vertexCount; i++) {
        fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
        fin >> m_model[i].tu >> m_model[i].tv;
        fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
    }

    // Close the model file.
    fin.close();

    return true;
}

void SkyDome::ReleaseSkyDomeModel() {
    if (m_model) {
        delete[] m_model;
        m_model = nullptr;
    }
}

bool SkyDome::InitializeBuffers(ID3D11Device* device) {
    // Create the vertex array.
    VertexType* vertices = new VertexType[m_vertexCount];
    if (!vertices) {
        return false;
    }

    // Create the index array.
    unsigned long* indices = new unsigned long[m_indexCount];
    if (!indices) {
        return false;
    }

    // Load the vertex array and index array with data.
    for (int i = 0; i < m_vertexCount; i++) {
        vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
        indices[i] = i;
    }

    // Set up the description of the vertex buffer.
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now finally create the vertex buffer.
    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result)) {
        return false;
    }

    // Set up the description of the index buffer.
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    D3D11_SUBRESOURCE_DATA  indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result)) {
        return false;
    }

    // Release the arrays now that the vertex and index buffers have been created and loaded.
    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void SkyDome::ReleaseBuffers() {
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

void SkyDome::RenderBuffers(ID3D11DeviceContext* deviceContext) {
    // Set vertex buffer stride and offset.
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
