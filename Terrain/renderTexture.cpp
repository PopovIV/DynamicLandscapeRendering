#include "rendertexture.h"

RenderTexture::RenderTexture(ID3D11Device* device, int textureWidth, int textureHeight, DXGI_FORMAT format) {
    m_format = format;
    Initialize(device, textureWidth, textureHeight);
}

// Function to initialize render texture class
bool RenderTexture::Initialize(ID3D11Device* device, int textureWidth, int textureHeight) {
    // Initialize the render target texture description.
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.
    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = m_format;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target texture.
    HRESULT result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
    if (FAILED(result)) {
        return false;
    }

    // Setup the description of the render target view.
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Create the render target view.
    result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
    if (FAILED(result)) {
        return false;
    }

    // Setup the description of the shader resource view.
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view.
    result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
    if (FAILED(result)) {
        return false;
    }

    m_viewport.Width = (FLOAT)textureWidth;
    m_viewport.Height = (FLOAT)textureHeight;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;

    return true;
}

// Function to realese render texture class
void RenderTexture::Shutdown() {
    if (m_shaderResourceView) {
        m_shaderResourceView->Release();
        m_shaderResourceView = nullptr;
    }

    if (m_renderTargetView) {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }

    if (m_renderTargetTexture) {
        m_renderTargetTexture->Release();
        m_renderTargetTexture = nullptr;
    }

    ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
}

void RenderTexture::Resize(ID3D11Device* device, int width, int height) {
    Shutdown();
    Initialize(device, width, height);
}

// Function to clear render target
void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha) {
    // Setup the color to clear the buffer to.
    float color[4];
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // Clear the back buffer.
    deviceContext->ClearRenderTargetView(m_renderTargetView, color);

    // Clear the depth buffer.
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 0.0f, 0);
}
