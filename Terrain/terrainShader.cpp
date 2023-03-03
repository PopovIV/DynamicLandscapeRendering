#include "terrainshader.h"

// Function to initialize shader
bool TerrainShader::Initialize(ID3D11Device* device, HWND hwnd) {
    // Initialize the vertex and pixel shaders.
    bool result = InitializeShader(device, hwnd, L"TerrainVertexShader.hlsl", L"TerrainPixelShader.hlsl", L"TerrainHullShader.hlsl", L"TerrainDomainShader.hlsl");
    if (!result) {
        return false;
    }

    return true;
}

// Render function
bool TerrainShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], ID3D11ShaderResourceView* normalMaps[], ID3D11ShaderResourceView* roughMaps[], ID3D11ShaderResourceView* aoMaps[], Light* light, XMFLOAT4 scales, float detailScale) {
    // Set the shader parameters that it will use for rendering.
    bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, cameraPos, textures, normalMaps, roughMaps, aoMaps, light, scales, detailScale);
    if (!result) {
        return false;
    }

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount);

    return true;
}

// Function to initialize shader
bool TerrainShader::InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilename, const wchar_t* hsFilename, const wchar_t* dsFilename) {
    // Initialize the pointers this function will use to null.
    ID3D10Blob* errorMessage = nullptr;
    ID3D10Blob* vertexShaderBuffer = nullptr;
    ID3D10Blob* hullShaderBuffer = nullptr;
    ID3D10Blob* domainShaderBuffer = nullptr;
    ID3D10Blob* pixelShaderBuffer = nullptr;

    int flags = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;

    // Compile the vertex shader code.
    HRESULT result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", flags, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage) {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        // If there was nothing in the error message then it simply could not find the shader file itself.
        else {
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }
        return false;
    }

    // Compile the hull shader code.
    result = D3DCompileFromFile(hsFilename, NULL, NULL, "main", "hs_5_0", flags, 0, &hullShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage) {
            OutputShaderErrorMessage(errorMessage, hwnd, hsFilename);
        }
        // If there was nothing in the error message then it simply could not find the file itself.
        else {
            MessageBox(hwnd, hsFilename, L"Missing Shader File", MB_OK);
        }
        return false;
    }

    // Compile the domain shader code.
    result = D3DCompileFromFile(dsFilename, NULL, NULL, "main", "ds_5_0", flags, 0, &domainShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage) {
            OutputShaderErrorMessage(errorMessage, hwnd, dsFilename);
        }
        // If there was nothing in the error message then it simply could not find the file itself.
        else {
            MessageBox(hwnd, dsFilename, L"Missing Shader File", MB_OK);
        }
        return false;
    }

    // Compile the pixel shader code.
    result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", flags, 0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage) {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        // If there was nothing in the error message then it simply could not find the file itself.
        else {
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }
        return false;
    }

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if (FAILED(result)) {
        return false;
    }

    // Create the hull shader from the buffer.
    result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader);
    if (FAILED(result)) {
        return false;
    }

    // Create the domain shader from the buffer.
    result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader);
    if (FAILED(result)) {
        return false;
    }

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(result)) {
        return false;
    }

    // Create the vertex input layout description.
    D3D11_INPUT_ELEMENT_DESC polygonLayout[6];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    polygonLayout[3].SemanticName = "TANGENT";
    polygonLayout[3].SemanticIndex = 0;
    polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[3].InputSlot = 0;
    polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[3].InstanceDataStepRate = 0;

    polygonLayout[4].SemanticName = "BINORMAL";
    polygonLayout[4].SemanticIndex = 0;
    polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[4].InputSlot = 0;
    polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[4].InstanceDataStepRate = 0;

    polygonLayout[5].SemanticName = "TEXCOORD";
    polygonLayout[5].SemanticIndex = 1;
    polygonLayout[5].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[5].InputSlot = 0;
    polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[5].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result)) {
        return false;
    }

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(result)) {
        return false;
    }

    // Create a texture sampler state description.
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 1;
    samplerDesc.BorderColor[1] = 1;
    samplerDesc.BorderColor[2] = 1;
    samplerDesc.BorderColor[3] = 1;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = 0;

    // Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_samplerStateNoMips);
    if (FAILED(result)) {
        return false;
    }

    // Create a texture sampler state description.
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 1;
    samplerDesc.BorderColor[1] = 1;
    samplerDesc.BorderColor[2] = 1;
    samplerDesc.BorderColor[3] = 1;
    samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_samplerState);
    if (FAILED(result)) {
        return false;
    }

    // Setup the description of the light dynamic constant buffer that is in the pixel shader.
    D3D11_BUFFER_DESC lightBufferDesc;
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
    if (FAILED(result)) {
        return false;
    }

    // Setup the description of the scale dynamic constant buffer that is in the pixel shader.
    D3D11_BUFFER_DESC scaleBufferDesc;
    scaleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    scaleBufferDesc.ByteWidth = sizeof(ScaleBufferType);
    scaleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    scaleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    scaleBufferDesc.MiscFlags = 0;
    scaleBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&scaleBufferDesc, NULL, &m_scaleBuffer);
    if (FAILED(result)) {
        return false;
    }

    return true;
}

// Function to release shader
void TerrainShader::ShutdownShader() {
    // Release the light constant buffer.
    if (m_lightBuffer) {
        m_lightBuffer->Release();
        m_lightBuffer = nullptr;
    }

    // Release the scale constant buffer.
    if (m_scaleBuffer) {
        m_scaleBuffer->Release();
        m_scaleBuffer = nullptr;
    }

    // Release the sampler state.
    if (m_samplerState) {
        m_samplerState->Release();
        m_samplerState = nullptr;
    }

    if (m_samplerStateNoMips) {
        m_samplerStateNoMips->Release();
        m_samplerState = nullptr;
    }

    // Release the matrix constant buffer.
    if (m_matrixBuffer) {
        m_matrixBuffer->Release();
        m_matrixBuffer = nullptr;
    }

    // Release the layout.
    if (m_layout) {
        m_layout->Release();
        m_layout = nullptr;
    }

    // Release the pixel shader.
    if (m_pixelShader) {
        m_pixelShader->Release();
        m_pixelShader = nullptr;
    }

    // Release the domain shader.
    if (m_domainShader) {
        m_domainShader->Release();
        m_domainShader = nullptr;
    }

    // Release the hull shader.
    if (m_hullShader) {
        m_hullShader->Release();
        m_hullShader = nullptr;
    }

    // Release the vertex shader.
    if (m_vertexShader) {
        m_vertexShader->Release();
        m_vertexShader = nullptr;
    }
}

// Function to print errors to file
void TerrainShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename) {
    // Get a pointer to the error message text buffer.
    char*  compileErrors = (char*)(errorMessage->GetBufferPointer());

    // Get the length of the message.
    unsigned long long bufferSize = errorMessage->GetBufferSize();

    // Open a file to write the error message to.
    ofstream fout;
    fout.open("shader-error.txt");

    // Write out the error message.
    for (unsigned long long i = 0; i < bufferSize; i++) {
        fout << compileErrors[i];
    }

    // Close the file.
    fout.close();

    // Release the error message.
    errorMessage->Release();
    errorMessage = 0;

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

// Function to fill shader buffers and params
bool TerrainShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], ID3D11ShaderResourceView* normalMaps[], ID3D11ShaderResourceView* roughMaps[], ID3D11ShaderResourceView* aoMaps[], Light* light, XMFLOAT4 scales, float detailScale) {
    // Transpose the matrices to prepare them for the shader.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // Lock the constant buffer so it can be written to.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
        return false;
    }

    // Get a pointer to the data in the constant buffer.
    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // Copy the matrices into the constant buffer.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;
    dataPtr->cameraPos = cameraPos;

    // Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // Set the position of the constant buffer in the vertex shader.
    unsigned int bufferNumber = 0;

    // Finanly set the constant buffer in the vertex shader with the updated values.
    deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
    deviceContext->DSSetShaderResources(0, 1, &roughMaps[5]);
    deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    // Set shader texture resource in the pixel shader.
    deviceContext->PSSetShaderResources(0, 1, &textures[0]);
    deviceContext->PSSetShaderResources(1, 1, &normalMaps[0]);
    deviceContext->PSSetShaderResources(2, 1, &roughMaps[0]);
    deviceContext->PSSetShaderResources(3, 1, &aoMaps[0]);
    deviceContext->PSSetShaderResources(4, 1, &textures[1]);
    deviceContext->PSSetShaderResources(5, 1, &normalMaps[1]);
    deviceContext->PSSetShaderResources(6, 1, &roughMaps[1]);
    deviceContext->PSSetShaderResources(7, 1, &aoMaps[1]);
    deviceContext->PSSetShaderResources(8, 1, &textures[2]);
    deviceContext->PSSetShaderResources(9, 1, &normalMaps[2]);
    deviceContext->PSSetShaderResources(10, 1, &roughMaps[2]);
    deviceContext->PSSetShaderResources(11, 1, &aoMaps[2]);
    deviceContext->PSSetShaderResources(12, 1, &textures[3]);
    deviceContext->PSSetShaderResources(13, 1, &normalMaps[3]);
    deviceContext->PSSetShaderResources(14, 1, &roughMaps[3]);
    deviceContext->PSSetShaderResources(15, 1, &aoMaps[3]);
    deviceContext->PSSetShaderResources(16, 1, &textures[4]);
    deviceContext->PSSetShaderResources(17, 1, &normalMaps[4]);
    deviceContext->PSSetShaderResources(18, 1, &roughMaps[4]);
    deviceContext->PSSetShaderResources(19, 1, &aoMaps[4]);
    deviceContext->PSSetShaderResources(20, 1, &textures[5]);
    deviceContext->PSSetShaderResources(21, 1, &normalMaps[5]);

    // Lock the light constant buffer so it can be written to.
    result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
        return false;
    }

    // Get a pointer to the data in the light constant buffer.
    LightBufferType* dataPtr2 = (LightBufferType*)mappedResource.pData;
    // Copy the lighting variables into the constant buffer.
    dataPtr2->diffuseColor = light->GetDiffuseColor();
    dataPtr2->ambientColor = light->GetAmbientColor();
    dataPtr2->specularColor = light->GetSpecularColor();
    dataPtr2->lightDirection = light->GetDirection();
    dataPtr2->specularPower = light->GetSpecularPower();

    // Unlock the light constant buffer.
    deviceContext->Unmap(m_lightBuffer, 0);

    // Lock the light constant buffer so it can be written to.
    result = deviceContext->Map(m_scaleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
        return false;
    }

    // Get a pointer to the data in the light constant buffer.

    ScaleBufferType* dataPtr3 = (ScaleBufferType*)mappedResource.pData;

    // Copy the lighting variables into the constant buffer.
    dataPtr3->grassScale = scales.x;
    dataPtr3->rockScale = scales.y;
    dataPtr3->slopeScale = scales.z;
    dataPtr3->snowScale = scales.w;
    dataPtr3->detailScale = detailScale;

    // Unlock the light constant buffer.
    deviceContext->Unmap(m_scaleBuffer, 0);

    // Set the position of the light constant buffer in the pixel shader.
    bufferNumber = 0;

    // Finally set the light constant buffer in the pixel shader with the updated values.
    deviceContext->PSSetConstantBuffers(bufferNumber++, 1, &m_lightBuffer);
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_scaleBuffer);

    return true;
}

// Render function
void TerrainShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->HSSetShader(m_hullShader, NULL, 0);
    deviceContext->DSSetShader(m_domainShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // Set the sampler state in the pixel shader.
    deviceContext->PSSetSamplers(0, 1, &m_samplerState);
    deviceContext->PSSetSamplers(1, 1, &m_samplerStateNoMips);
    deviceContext->DSSetSamplers(0, 1, &m_samplerStateNoMips);
    // Render the triangle.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
