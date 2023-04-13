#include "toneMap.h"

// Function to initialize
bool ToneMap::Initialize(ID3D11Device* device, HWND hwnd, int textureWidth, int textureHeight) {
    // Initialize the vertex and pixel shaders.
    bool result = InitializeShader(device, hwnd, L"ToneMapVertexShader.hlsl", L"ToneMapPixelShader.hlsl");
    if (!result) {
        return false;
    }

    m_averageLuminance = new AverageLuminance();
    bool res = m_averageLuminance->Initialize(device, hwnd, textureWidth, textureHeight);
    if (!res) {
        return false;
    }

    return true;
}

void ToneMap::Resize(ID3D11Device* device, int width, int height) {
    m_averageLuminance->Resize(device, width, height);
}

bool ToneMap::InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilename) {
    // Initialize the pointers this function will use to null.
    ID3D10Blob* errorMessage = nullptr;
    ID3D10Blob* vertexShaderBuffer = nullptr;
    ID3D10Blob* pixelShaderBuffer = nullptr;

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

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(result)) {
        return false;
    }

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    // Create the sampler state
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;

    // Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    if (FAILED(result)) {
        return false;
    }

    CD3D11_BUFFER_DESC albd(sizeof(LuminanceConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
    result = device->CreateBuffer(&albd, nullptr, &m_luminanceBuffer);
    if (FAILED(result)) {
        return false;
    }

    return true;
}

void ToneMap::Process(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture, ID3D11RenderTargetView* renderTarget, D3D11_VIEWPORT viewport) {
    float averageLuminance = m_averageLuminance->Process(deviceContext, sourceTexture);
    LuminanceConstantBuffer luminanceBufferData = { averageLuminance };
    deviceContext->UpdateSubresource(m_luminanceBuffer, 0, nullptr, &luminanceBufferData, 0, 0);

    deviceContext->OMSetRenderTargets(1, &renderTarget, nullptr);
    deviceContext->RSSetViewports(1, &viewport);

    deviceContext->IASetInputLayout(nullptr);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    deviceContext->HSSetShader(nullptr, nullptr, 0);
    deviceContext->DSSetShader(nullptr, nullptr, 0);
    deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
    deviceContext->PSSetConstantBuffers(0, 1, &m_luminanceBuffer);
    deviceContext->PSSetShaderResources(0, 1, &sourceTexture);
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);

    deviceContext->Draw(4, 0);

    ID3D11ShaderResourceView* nullsrv[] = { nullptr };
    deviceContext->PSSetShaderResources(0, 1, nullsrv);
}

// Function to print error if error happened
void ToneMap::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename) {
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

// Function to release
void ToneMap::Shutdown() {
    // Release the sampler state.
    if (m_sampleState) {
        m_sampleState->Release();
        m_sampleState = nullptr;
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

    if (m_luminanceBuffer) {
        m_luminanceBuffer->Release();
        m_luminanceBuffer = nullptr;
    }

    if (m_averageLuminance) {
        m_averageLuminance->Shutdown();
        delete m_averageLuminance;
        m_averageLuminance = nullptr;
    }
}
