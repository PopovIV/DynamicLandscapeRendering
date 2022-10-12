#include "terrainshader.h"

TerrainShader::TerrainShader() {

    m_vertexShader = 0;
    m_pixelShader = 0;
    m_hullShader = 0;
    m_domainShader = 0;
    m_layout = 0;
    m_matrixBuffer = 0;

}

// Function to initialize shader
bool TerrainShader::Initialize(ID3D11Device* device, HWND hwnd) {

    bool result;

    // Initialize the vertex and pixel shaders.
    result = InitializeShader(device, hwnd, L"TerrainVertexShader.hlsl", L"TerrainPixelShader.hlsl", L"TerrainHullShader.hlsl", L"TerrainDomainShader.hlsl");
    if (!result)
        return false;

    return true;

}

// Render function
bool TerrainShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {

    bool result;

    // Set the shader parameters that it will use for rendering.
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, cameraPos);
    if (!result)
        return false;

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount);

    return true;

}

// Function to initialize shader
bool TerrainShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, WCHAR* hsFilename, WCHAR* dsFilename) {

    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* hullShaderBuffer;
    ID3D10Blob* domainShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc; 

    // Initialize the pointers this function will use to null.
    errorMessage = 0;
    vertexShaderBuffer = 0;
    hullShaderBuffer = 0;
    domainShaderBuffer = 0;
    pixelShaderBuffer = 0;

    // Compile the vertex shader code.
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        // If there was nothing in the error message then it simply could not find the shader file itself.
        else
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);

        return false;
    }

    // Compile the hull shader code.
    result = D3DCompileFromFile(hsFilename, NULL, NULL, "main", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &hullShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
            OutputShaderErrorMessage(errorMessage, hwnd, hsFilename);

        // If there was nothing in the error message then it simply could not find the file itself.
        else
            MessageBox(hwnd, hsFilename, L"Missing Shader File", MB_OK);

        return false;
    }

    // Compile the domain shader code.
    result = D3DCompileFromFile(dsFilename, NULL, NULL, "main", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &domainShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
            OutputShaderErrorMessage(errorMessage, hwnd, dsFilename);

        // If there was nothing in the error message then it simply could not find the file itself.
        else
            MessageBox(hwnd, dsFilename, L"Missing Shader File", MB_OK);

        return false;
    }

    // Compile the pixel shader code.
    result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);

        // If there was nothing in the error message then it simply could not find the file itself.
        else
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);

        return false;
    }

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if (FAILED(result))
        return false;

    // Create the hull shader from the buffer.
    result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &m_hullShader);
    if (FAILED(result))
        return false;

    // Create the domain shader from the buffer.
    result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &m_domainShader);
    if (FAILED(result))
        return false;

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(result))
        return false;


    // Create the vertex input layout description.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result))
        return false;

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    hullShaderBuffer->Release();
    hullShaderBuffer = 0;

    domainShaderBuffer->Release();
    domainShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(result))
        return false;

    return true;

}

// Function to release shader
void TerrainShader::ShutdownShader() {

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
void TerrainShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {

    char* compileErrors;
    unsigned long long bufferSize, i;
    ofstream fout;


    // Get a pointer to the error message text buffer.
    compileErrors = (char*)(errorMessage->GetBufferPointer());

    // Get the length of the message.
    bufferSize = errorMessage->GetBufferSize();

    // Open a file to write the error message to.
    fout.open("shader-error.txt");

    // Write out the error message.
    for (i = 0; i < bufferSize; i++)
        fout << compileErrors[i];

    // Close the file.
    fout.close();

    // Release the error message.
    errorMessage->Release();
    errorMessage = 0;

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

}

// Function to fill shader buffers and params
bool TerrainShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos) {

    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
   unsigned int bufferNumber;


    // Transpose the matrices to prepare them for the shader.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // Lock the matrix constant buffer so it can be written to.
    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
        return false;

    // Get a pointer to the data in the matrix constant buffer.
    dataPtr = (MatrixBufferType*)mappedResource.pData;

    // Copy the matrices into the matrix constant buffer.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;
    dataPtr->cameraPos = cameraPos;

    // Unlock the matrix constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // Set the position of the matrix constant buffer in the vertex shader.
    bufferNumber = 0;

    // Finanly set the matrix constant buffer in the vertex shader with the updated values.
    //deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
    deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
    deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
    return true;

}

// Render function
void TerrainShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {

    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to do the rendering.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->HSSetShader(m_hullShader, NULL, 0);
    deviceContext->DSSetShader(m_domainShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // Render the data.
    deviceContext->DrawIndexed(indexCount, 0, 0);

}