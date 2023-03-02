#include "averageLuminance.h"

// Constructor
AverageLuminance::AverageLuminance() {
    m_vertexShader = nullptr;
    m_copyPixelShader = nullptr;
    m_pixelShader = nullptr;
    m_sampleState = nullptr;
    m_luminanceTexture = nullptr;

    QueryPerformanceFrequency(&m_qpcFrequency);
    QueryPerformanceCounter(&m_qpcLastTime);

    m_adaptedLuminance = 0.0f;
}

// Function to initialize
bool AverageLuminance::Initialize(ID3D11Device* device, HWND hwnd, int textureWidth, int textureHeight) {
    bool result;
    // Initialize the vertex and pixel shaders.
    result = InitializeShader(device, hwnd, L"ToneMapVertexShader.hlsl", L"CopyPixelShader.hlsl", L"LuminancePixelShader.hlsl");
    if (!result)
        return false;

    return CreateTextures(device, textureWidth, textureHeight);
}

bool AverageLuminance::InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psCopyFilename, const wchar_t* psFilename) {
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* copyPixelShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
 
    // Initialize the pointers this function will use to null.
    errorMessage = nullptr;
    vertexShaderBuffer = nullptr;
    copyPixelShaderBuffer = nullptr;
    pixelShaderBuffer = nullptr;

    int flags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;

    // Compile the vertex shader code.
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", flags, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        // If there was nothing in the error message then it simply could not find the shader file itself.
        else
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        return false;
    }

    // Compile the copy pixel shader code.
    result = D3DCompileFromFile(psCopyFilename, NULL, NULL, "main", "ps_5_0", flags, 0, &copyPixelShaderBuffer, &errorMessage);
    if (FAILED(result)) {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        // If there was nothing in the error message then it simply could not find the file itself.
        else
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);

        return false;
    }

    // Compile the pixel shader code.
    result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", flags, 0, &pixelShaderBuffer, &errorMessage);
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

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(copyPixelShaderBuffer->GetBufferPointer(), copyPixelShaderBuffer->GetBufferSize(), NULL, &m_copyPixelShader);
    if (FAILED(result))
        return false;

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(result))
        return false;

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    copyPixelShaderBuffer->Release();
    copyPixelShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    // Create the sampler state
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;

    // Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    if (FAILED(result))
        return false;

    CD3D11_TEXTURE2D_DESC ltd(
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        1,
        1,
        1,
        1,
        0,
        D3D11_USAGE_STAGING,
        D3D11_CPU_ACCESS_READ
    );

    result = device->CreateTexture2D(&ltd, nullptr, &m_luminanceTexture);
    if (FAILED(result))
        return false;

    return true;
}

void AverageLuminance::CopyTexture(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture, RenderTexture& dst, ID3D11PixelShader* pixelShader) {
    ID3D11RenderTargetView* renderTarget = dst.GetRenderTargetView();

    D3D11_VIEWPORT viewport = dst.GetViewPort();

    deviceContext->OMSetRenderTargets(1, &renderTarget, nullptr);
    deviceContext->RSSetViewports(1, &viewport);

    deviceContext->PSSetShader(pixelShader, nullptr, 0);
    deviceContext->PSSetShaderResources(0, 1, &sourceTexture);

    deviceContext->Draw(4, 0);

    ID3D11ShaderResourceView* nullsrv[] = { nullptr };
    deviceContext->PSSetShaderResources(0, 1, nullsrv);
}

bool AverageLuminance::CreateTextures(ID3D11Device* device, int width, int height) {
    int minSize = (min(width, height));
    int numTextures = std::log2(minSize);
    for (auto t : m_renderTextures) {
        delete t;
    }
    try
    {
        for (int i = 0; i <= numTextures; i++) {
            int texture_size = 1 << (numTextures - i);
            m_renderTextures.emplace_back(new RenderTexture(device, texture_size, texture_size));
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

float AverageLuminance::Process(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture) {

    float backgroundColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    for (size_t i = 0; i < m_renderTextures.size(); i++)
        deviceContext->ClearRenderTargetView(m_renderTextures[i]->GetRenderTargetView(), backgroundColour);

    deviceContext->IASetInputLayout(nullptr);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    deviceContext->HSSetShader(nullptr, nullptr, 0);
    deviceContext->DSSetShader(nullptr, nullptr, 0);

    deviceContext->PSSetSamplers(0, 1, &m_sampleState);


    CopyTexture(deviceContext, sourceTexture, *m_renderTextures[0], m_copyPixelShader);
    CopyTexture(deviceContext, m_renderTextures[0]->GetShaderResourceView(), *m_renderTextures[1], m_pixelShader);
    for (int i = 2; i < m_renderTextures.size(); i++) {
        CopyTexture(deviceContext, m_renderTextures[i - 1]->GetShaderResourceView(), *m_renderTextures[i], m_copyPixelShader);
    }

    ID3D11ShaderResourceView* nullsrv[] = { nullptr };
    deviceContext->PSSetShaderResources(0, 1, nullsrv);

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    size_t timeDelta = currentTime.QuadPart - m_qpcLastTime.QuadPart;
    m_qpcLastTime = currentTime;
    double delta = (double)(timeDelta) / m_qpcFrequency.QuadPart;

    D3D11_MAPPED_SUBRESOURCE luminanceAccessor;
    deviceContext->CopyResource(m_luminanceTexture, m_renderTextures[m_renderTextures.size() - 1]->GetRenderTarget());
    deviceContext->Map(m_luminanceTexture, 0, D3D11_MAP_READ, 0, &luminanceAccessor);
    float luminance = *(float*)luminanceAccessor.pData;
    deviceContext->Unmap(m_luminanceTexture, 0);

    float sigma = 0.04f / (0.04f + luminance);
    float tau = sigma * 0.4f + (1 - sigma) * 0.1f;
    m_adaptedLuminance += (luminance - m_adaptedLuminance) * (float)(1 - std::exp(-delta * tau));
    return m_adaptedLuminance;
}

// Function to print errors to file
void AverageLuminance::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename) {

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

// Function to release
void AverageLuminance::Shutdown() {
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

    // Release the pixel shader.
    if (m_copyPixelShader) {
        m_copyPixelShader->Release();
        m_copyPixelShader = nullptr;
    }

    // Release the vertex shader.
    if (m_vertexShader) {
        m_vertexShader->Release();
        m_vertexShader = nullptr;
    }

    for (auto t : m_renderTextures) {
        delete t;
    }

    if (m_luminanceTexture) {
        m_luminanceTexture->Release();
        m_luminanceTexture = nullptr;
    }

}
