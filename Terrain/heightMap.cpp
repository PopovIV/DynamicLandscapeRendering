#include "heightMap.h"

// Function to initialize
bool HeightMap::Initialize(ID3D11Device* device, HWND hwnd) {
    bool result;
    // Initialize the vertex and pixel shaders.
    result = InitializeShader(device, hwnd, L"ToneMapVertexShader.hlsl", L"ConvertPixelShader.hlsl", L"HeightMapPixelShader.hlsl");
    if (!result) {
        return false;
    }

    return true;
}

bool HeightMap::InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* convertPsFilename, const wchar_t* psFilename) {
    ID3D10Blob* errorMessage = nullptr;
    ID3D10Blob* vertexShaderBuffer = nullptr;
    ID3D10Blob* convertPixelShaderBuffer = nullptr;
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

    // Compile the copy pixel shader code.
    result = D3DCompileFromFile(convertPsFilename, NULL, NULL, "main", "ps_5_0", flags, 0, &convertPixelShaderBuffer, &errorMessage);
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
    result = device->CreatePixelShader(convertPixelShaderBuffer->GetBufferPointer(), convertPixelShaderBuffer->GetBufferSize(), NULL, &m_convertPixelShader);
    if (FAILED(result)) {
        return false;
    }

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(result)) {
        return false;
    }

    // Release the vertex and pixel shader buffer since they are no longer needed.
    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;

    convertPixelShaderBuffer->Release();
    convertPixelShaderBuffer = nullptr;

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
    result = device->CreateSamplerState(&samplerDesc, &m_samplerState);
    if (FAILED(result)) {
        return false;
    }

    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;

    // Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_samplerMinState);
    if (FAILED(result)) {
        return false;
    }

    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;
    result = device->CreateSamplerState(&samplerDesc, &m_samplerMaxState);
    if (FAILED(result)) {
        return false;
    }

    return true;
}

void HeightMap::CopyTexture(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture, RenderTexture& dst, ID3D11PixelShader* pixelShader) {
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

bool HeightMap::Process(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture) {
    std::vector<RenderTexture*> renderTextures;
    
    ID3D11Resource* res = nullptr;
    sourceTexture->GetResource(&res);
    ID3D11Texture2D* texture2d = nullptr;
    HRESULT hr = res->QueryInterface(&texture2d);

    if (FAILED(hr)) {
        return false;
    }

    float width, height;
    D3D11_TEXTURE2D_DESC desc;
    texture2d->GetDesc(&desc);
    width = static_cast<float>(desc.Width);
    height = static_cast<float>(desc.Height);

    int minSize = (min(width, height));
    int minTerrainSize = min(TERRAIN_CHUNK_COUNT_HEIGHT, TERRAIN_CHUNK_COUNT_WIDTH);

    try
    {
        while (minSize >= minTerrainSize) {
            renderTextures.emplace_back(new RenderTexture(device, minSize, minSize, DXGI_FORMAT_R32G32_FLOAT));
            minSize /= 2;
        }
    }
    catch (...)
    {
        return false;
    }

    // First thing to do is convert our height map from R32 to R32G32
    deviceContext->IASetInputLayout(nullptr);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    deviceContext->HSSetShader(nullptr, nullptr, 0);
    deviceContext->DSSetShader(nullptr, nullptr, 0);

    deviceContext->PSSetSamplers(0, 1, &m_samplerState);

    CopyTexture(deviceContext, sourceTexture, *renderTextures[0], m_convertPixelShader);

    deviceContext->PSSetSamplers(0, 1, &m_samplerMinState);
    deviceContext->PSSetSamplers(1, 1, &m_samplerMaxState);
    // Now get AABB's height from that texture
    for (int i = 1; i < renderTextures.size(); i++) {
        CopyTexture(deviceContext, renderTextures[i - 1]->GetShaderResourceView(), *renderTextures[i], m_pixelShader);
    }

    ID3D11ShaderResourceView* nullsrv[] = { nullptr };
    deviceContext->PSSetShaderResources(0, 1, nullsrv);

    renderTextures.back()->GetRenderTarget()->GetDesc(&desc);
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = -1;
    hr = device->CreateShaderResourceView(renderTextures.back()->GetRenderTarget(), &srvDesc, &m_resTextureView);

    if (FAILED(hr)) {
        return false;
    }

    texture2d->Release();
    texture2d = nullptr;
    res->Release();
    res = nullptr;
    // delete array of textures
    for (auto& t : renderTextures) {
        t->Shutdown();
        delete t;
    }
    renderTextures.clear();

    return true;
}

// Function to print errors to file
void HeightMap::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename) {
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
void HeightMap::Shutdown() {
    // Release the sampler state.
    if (m_samplerState) {
        m_samplerState->Release();
        m_samplerState = nullptr;
    }

    if (m_samplerMinState) {
        m_samplerMinState->Release();
        m_samplerMinState = nullptr;
    }

    if (m_samplerMaxState) {
        m_samplerMaxState->Release();
        m_samplerMaxState = nullptr;
    }

    if (m_resTextureView) {
        m_resTextureView->Release();
        m_resTextureView = nullptr;
    }

    // Release the shaders
    if (m_vertexShader) {
        m_vertexShader->Release();
        m_vertexShader = nullptr;
    }

    if (m_convertPixelShader) {
        m_convertPixelShader->Release();
        m_convertPixelShader = nullptr;
    }

    if (m_pixelShader) {
        m_pixelShader->Release();
        m_pixelShader = nullptr;
    }

}
