#include "lightshader.h"

// Function to initialize shader
bool LightShader::Initialize(ID3D11Device* device, HWND hwnd) {
    // Initialize the vertex and pixel shaders.
    bool result = InitializeShader(device, hwnd, L"LightVertexShader.hlsl", L"LightPixelShader.hlsl");
    if (!result) {
        return false;
    }

    return true;
}

// Render function
bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor) {
    // Set the shader parameters that it will use for rendering.
    bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
    if (!result) {
        return false;
    }

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount);

    return true;
}

// Function to initialize shader
bool LightShader::InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilename) {
    // Initialize the pointers this function will use to null.
    ID3D10Blob* errorMessage = 0;
    ID3D10Blob* vertexShaderBuffer = 0;
    ID3D10Blob* pixelShaderBuffer = 0;

    int flags = 0;
#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
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

    // Compile the pixel shader code.
    result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
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
    if (FAILED(result))
        return false;

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(result))
        return false;

    // Create the vertex input layout description.
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
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
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
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

    return true;
}

// Function to release shader
void LightShader::ShutdownShader() {
    // Release the light constant buffer.
    if (m_lightBuffer) {
        m_lightBuffer->Release();
        m_lightBuffer = nullptr;
    }

    // Release the sampler state.
    if (m_sampleState) {
        m_sampleState->Release();
        m_sampleState = nullptr;
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

    // Release the vertex shader.
    if (m_vertexShader) {
        m_vertexShader->Release();
        m_vertexShader = nullptr;
    }
}

// Function to print errors to file
void LightShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename) {
    // Get a pointer to the error message text buffer.
    char* compileErrors = (char*)(errorMessage->GetBufferPointer());

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
bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor) {
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
    MatrixBufferType*  dataPtr = (MatrixBufferType*)mappedResource.pData;

    // Copy the matrices into the constant buffer.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // Set the position of the constant buffer in the vertex shader.
    unsigned int bufferNumber = 0;

    // Finanly set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    // Set shader texture resource in the pixel shader.
    deviceContext->PSSetShaderResources(0, 1, &texture);

    // Lock the light constant buffer so it can be written to.
    result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
        return false;
    }

    // Get a pointer to the data in the light constant buffer.
    LightBufferType* dataPtr2 = (LightBufferType*)mappedResource.pData;

    // Copy the lighting variables into the constant buffer.
    dataPtr2->diffuseColor = diffuseColor;
    dataPtr2->lightDirection = lightDirection;
    dataPtr2->padding = 0.0f;

    // Unlock the light constant buffer.
    deviceContext->Unmap(m_lightBuffer, 0);

    // Set the position of the light constant buffer in the pixel shader.
    bufferNumber = 0;

    // Finally set the light constant buffer in the pixel shader with the updated values.
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

    return true;
}

// Render function
void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->HSSetShader(NULL, NULL, 0);
    deviceContext->DSSetShader(NULL, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // Set the sampler state in the pixel shader.
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);

    // Render the triangle.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
