#include "terrainshader.h"

// Function to initialize shader
bool TerrainShader::Initialize(ID3D11Device* device, HWND hwnd) {
    // Initialize the vertex and pixel shaders.
    bool result = InitializeShader(device, hwnd, L"TerrainVertexShader.hlsl", L"TerrainPixelShader.hlsl", L"TerrainHullShader.hlsl", L"TerrainDomainShader.hlsl", L"CullShader.hlsl");
    if (!result) {
        return false;
    }

    return true;
}

// Render function
bool TerrainShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMFLOAT4* frustumPlanes, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], Light* light, ScaleBufferType scaleBuffer, bool normalPass) {
    // Set the shader parameters that it will use for rendering.
    bool result = SetShaderParameters(deviceContext, frustumPlanes, worldMatrix, viewMatrix, projectionMatrix, cameraPos, textures, light, scaleBuffer);
    if (!result) {
        return false;
    }

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount, normalPass);

    return true;
}

// Function to initialize shader
bool TerrainShader::InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* csFilename) {
    // Initialize the pointers this function will use to null.
    ID3D10Blob* errorMessage = nullptr;
    ID3D10Blob* vertexShaderBuffer = nullptr;
    ID3D10Blob* hullShaderBuffer = nullptr;
    ID3D10Blob* domainShaderBuffer = nullptr;
    ID3D10Blob* pixelShaderBuffer = nullptr;
    ID3D10Blob* computeShaderBuffer = nullptr;

    int flags = 0;
#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    D3DInclude includeObj;

    // Compile the vertex shader code.
    HRESULT result = D3DCompileFromFile(vsFilename, NULL, &includeObj, "main", "vs_5_0", flags, 0, &vertexShaderBuffer, &errorMessage);
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
    result = D3DCompileFromFile(hsFilename, NULL, &includeObj, "main", "hs_5_0", flags, 0, &hullShaderBuffer, &errorMessage);
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
    result = D3DCompileFromFile(dsFilename, NULL, &includeObj, "main", "ds_5_0", flags, 0, &domainShaderBuffer, &errorMessage);
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
    result = D3DCompileFromFile(psFilename, NULL, &includeObj, "main", "ps_5_0", flags, 0, &pixelShaderBuffer, &errorMessage);
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

    // Compile the compute shader code.
    result = D3DCompileFromFile(csFilename, NULL, &includeObj, "main", "cs_5_0", flags, 0, &computeShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage) {
            OutputShaderErrorMessage(errorMessage, hwnd, csFilename);
        }
        // If there was nothing in the error message then it simply could not find the file itself.
        else {
            MessageBox(hwnd, csFilename, L"Missing Shader File", MB_OK);
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

    result = device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), NULL, &m_pCullShader);
    if (FAILED(result)) {
        return false;
    }

    // Create the vertex input layout description.
    D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result)) {
        return false;
    }

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    hullShaderBuffer->Release();
    hullShaderBuffer = nullptr;

    domainShaderBuffer->Release();
    domainShaderBuffer = nullptr;

    computeShaderBuffer->Release();
    computeShaderBuffer = nullptr;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Set constant buffers
    if (SUCCEEDED(result)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(MatrixBufferType) * TERRAIN_CHUNK_COUNT_WIDTH * TERRAIN_CHUNK_COUNT_HEIGHT;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        int index = 0;
        MatrixBufferType MatrixBufferInst[TERRAIN_CHUNK_COUNT_WIDTH * TERRAIN_CHUNK_COUNT_HEIGHT];
        for (int j = 0; j < TERRAIN_CHUNK_COUNT_WIDTH; j++) {
            for (int i = 0; i < TERRAIN_CHUNK_COUNT_HEIGHT; i++) {
                MatrixBufferInst[index++].worldMatrix = XMMatrixTranspose(XMMatrixTranslation(i * (TERRAIN_CHUNK_WIDTH - 1), 0, j * (TERRAIN_CHUNK_HEIGHT - 1)));
            }
        }

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &MatrixBufferInst;
        data.SysMemPitch = sizeof(MatrixBufferInst);
        data.SysMemSlicePitch = 0;

        result = device->CreateBuffer(&desc, &data, &m_worldMatrixBuffer);
        assert(SUCCEEDED(result));
    }
    if (SUCCEEDED(result)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(SceneProjectionBufferType);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        result = device->CreateBuffer(&desc, nullptr, &m_viewProjectionMatrixBuffer);
        assert(SUCCEEDED(result));
    }
    if (SUCCEEDED(result)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride = sizeof(UINT);

        result = device->CreateBuffer(&desc, nullptr, &m_pInderectArgsSrc);
        if (SUCCEEDED(result)) {
            result = device->CreateUnorderedAccessView(m_pInderectArgsSrc, nullptr, &m_pInderectArgsUAV);
        }
        assert(SUCCEEDED(result));
    }
    if (SUCCEEDED(result)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
        desc.StructureByteStride = 0;

        result = device->CreateBuffer(&desc, nullptr, &m_pInderectArgs);
        assert(SUCCEEDED(result));
    }

    if (SUCCEEDED(result)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(XMINT4) * TERRAIN_CHUNK_COUNT_HEIGHT * TERRAIN_CHUNK_COUNT_WIDTH;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride = sizeof(XMINT4);

        result = device->CreateBuffer(&desc, nullptr, &m_pGeomBufferInstVisGpu);
        if (SUCCEEDED(result)) {
            result = device->CreateUnorderedAccessView(m_pGeomBufferInstVisGpu, nullptr, &m_pGeomBufferInstVisGpu_UAV);
        }
        assert(SUCCEEDED(result));
    }

    if (SUCCEEDED(result)) {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(XMINT4) * TERRAIN_CHUNK_COUNT_HEIGHT * TERRAIN_CHUNK_COUNT_WIDTH;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        result = device->CreateBuffer(&desc, nullptr, &m_pGeomBufferInstVis);
        assert(SUCCEEDED(result));
    }

    // Create a texture sampler state description.
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
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
    if (m_worldMatrixBuffer) {
        m_worldMatrixBuffer->Release();
        m_worldMatrixBuffer = nullptr;
    }

    if (m_viewProjectionMatrixBuffer) {
        m_viewProjectionMatrixBuffer->Release();
        m_viewProjectionMatrixBuffer = nullptr;
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

    if (m_pCullParams) {
        m_pCullParams->Release();
        m_pCullParams = nullptr;
    }

    if (m_pCullShader) {
        m_pCullShader->Release();
        m_pCullShader = nullptr;
    }

    if (m_pInderectArgsSrc) {
        m_pInderectArgsSrc->Release();
        m_pInderectArgsSrc = nullptr;
    }

    if (m_pInderectArgs) {
        m_pInderectArgs->Release();
        m_pInderectArgs = nullptr;
    }

    if (m_pGeomBufferInstVisGpu) {
        m_pGeomBufferInstVisGpu->Release();
        m_pGeomBufferInstVisGpu = nullptr;
    }

    if (m_pGeomBufferInstVisGpu_UAV) {
        m_pGeomBufferInstVisGpu_UAV->Release();
        m_pGeomBufferInstVisGpu_UAV = nullptr;
    }

    if (m_pGeomBufferInstVis) {
        m_pGeomBufferInstVis->Release();
        m_pGeomBufferInstVis = nullptr;
    }

    if (m_pInderectArgsUAV) {
        m_pInderectArgsUAV->Release();
        m_pInderectArgsUAV = nullptr;
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
bool TerrainShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4* frustumPlanes, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], Light* light, ScaleBufferType scaleBuffer) {
    // Transpose the matrices to prepare them for the shader.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // Lock the constant buffer so it can be written to.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = deviceContext->Map(m_viewProjectionMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
        return false;
    }

    // Get a pointer to the data in the constant buffer.
    SceneProjectionBufferType* dataPtr = (SceneProjectionBufferType*)mappedResource.pData;

    // Copy the matrices into the constant buffer.
    dataPtr->viewProjectionMatrix = XMMatrixMultiply(projectionMatrix, viewMatrix);
    for (int i = 0; i < 6; i++) {
        dataPtr->planes[i] = frustumPlanes[i];
    }
    dataPtr->cameraPos = cameraPos;

    // Unlock the constant buffer.
    deviceContext->Unmap(m_viewProjectionMatrixBuffer, 0);

    // Finanly set the constant buffer in the vertex shader with the updated values.
    deviceContext->DSSetConstantBuffers(0, 1, &m_worldMatrixBuffer);
    deviceContext->DSSetConstantBuffers(1, 1, &m_viewProjectionMatrixBuffer);
    deviceContext->HSSetConstantBuffers(0, 1, &m_worldMatrixBuffer);
    deviceContext->HSSetConstantBuffers(1, 1, &m_viewProjectionMatrixBuffer);
    deviceContext->VSSetConstantBuffers(0, 1, &m_worldMatrixBuffer);
    deviceContext->VSSetConstantBuffers(1, 1, &m_viewProjectionMatrixBuffer);
    deviceContext->VSSetConstantBuffers(2, 1, &m_pGeomBufferInstVis);

    deviceContext->VSSetShaderResources(0, 1, &textures[22]);
    deviceContext->VSSetSamplers(0, 1, &m_samplerStateNoMips);

    deviceContext->DSSetShaderResources(0, 1, &textures[22]);
    deviceContext->DSSetSamplers(0, 1, &m_samplerStateNoMips);

    // Set shader texture resource in the pixel shader.
    deviceContext->PSSetShaderResources(0, 1, &textures[0]);
    deviceContext->PSSetShaderResources(1, 1, &textures[1]);
    deviceContext->PSSetShaderResources(2, 1, &textures[2]);
    deviceContext->PSSetShaderResources(3, 1, &textures[3]);
    deviceContext->PSSetShaderResources(4, 1, &textures[4]);
    deviceContext->PSSetShaderResources(5, 1, &textures[5]);
    deviceContext->PSSetShaderResources(6, 1, &textures[6]);
    deviceContext->PSSetShaderResources(7, 1, &textures[7]);
    deviceContext->PSSetShaderResources(8, 1, &textures[8]);
    deviceContext->PSSetShaderResources(9, 1, &textures[9]);
    deviceContext->PSSetShaderResources(10, 1, &textures[10]);
    deviceContext->PSSetShaderResources(11, 1, &textures[11]);
    deviceContext->PSSetShaderResources(12, 1, &textures[12]);
    deviceContext->PSSetShaderResources(13, 1, &textures[13]);
    deviceContext->PSSetShaderResources(14, 1, &textures[14]);
    deviceContext->PSSetShaderResources(15, 1, &textures[15]);
    deviceContext->PSSetShaderResources(16, 1, &textures[16]);
    deviceContext->PSSetShaderResources(17, 1, &textures[17]);;
    deviceContext->PSSetShaderResources(18, 1, &textures[18]);
    deviceContext->PSSetShaderResources(19, 1, &textures[19]);
    deviceContext->PSSetShaderResources(20, 1, &textures[20]);
    deviceContext->PSSetShaderResources(21, 1, &textures[21]);

    // GPU CULLING
    D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS args;
    args.IndexCountPerInstance = (TERRAIN_CHUNK_WIDTH - 1) * (TERRAIN_CHUNK_HEIGHT - 1) * 6;;
    args.InstanceCount = 0;
    args.StartInstanceLocation = 0;
    args.BaseVertexLocation = 0;
    args.StartIndexLocation = 0;
    deviceContext->UpdateSubresource(m_pInderectArgsSrc, 0, nullptr, &args, 0, 0);
    UINT groupNumber = TERRAIN_CHUNK_COUNT_WIDTH * TERRAIN_CHUNK_COUNT_HEIGHT / 64u + !!(TERRAIN_CHUNK_COUNT_WIDTH * TERRAIN_CHUNK_COUNT_HEIGHT % 64u);
    deviceContext->CSSetConstantBuffers(0, 1, &m_worldMatrixBuffer);
    deviceContext->CSSetConstantBuffers(1, 1, &m_viewProjectionMatrixBuffer);
    deviceContext->CSSetUnorderedAccessViews(0, 1, &m_pInderectArgsUAV, nullptr);
    deviceContext->CSSetUnorderedAccessViews(1, 1, &m_pGeomBufferInstVisGpu_UAV, nullptr);
    deviceContext->CSSetShader(m_pCullShader, nullptr, 0);
    deviceContext->Dispatch(groupNumber, 1, 1);

    deviceContext->CopyResource(m_pGeomBufferInstVis, m_pGeomBufferInstVisGpu);
    deviceContext->CopyResource(m_pInderectArgs, m_pInderectArgsSrc);
    deviceContext->CSSetShaderResources(0, 1, &textures[22]);
    deviceContext->CSSetSamplers(0, 1, &m_samplerStateNoMips);

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
    dataPtr3->scales = scaleBuffer.scales;
    dataPtr3->detailScale = scaleBuffer.detailScale;

    // Unlock the light constant buffer.
    deviceContext->Unmap(m_scaleBuffer, 0);

    // Finally set the light constant buffer in the pixel shader with the updated values.
    deviceContext->PSSetConstantBuffers(0, 1, &m_lightBuffer);
    deviceContext->PSSetConstantBuffers(1, 1, &m_scaleBuffer);

    return true;
}

// Render function
void TerrainShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, bool normalPass) {
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->HSSetShader(m_hullShader, NULL, 0);
    deviceContext->DSSetShader(m_domainShader, NULL, 0);
    if (normalPass) {
        deviceContext->PSSetShader(m_pixelShader, NULL, 0);
        // Set the sampler state in the pixel shader.
        deviceContext->PSSetSamplers(0, 1, &m_samplerState);
        deviceContext->PSSetSamplers(1, 1, &m_samplerStateNoMips);
    }
    else {
        deviceContext->PSSetShader(NULL, NULL, 0);
    }

    // Render the triangle
    deviceContext->DrawIndexedInstancedIndirect(m_pInderectArgs, 0);
    //deviceContext->DrawIndexedInstanced(indexCount, TERRAIN_CHUNK_COUNT_WIDTH * TERRAIN_CHUNK_COUNT_HEIGHT, 0, 0, 0);
}
