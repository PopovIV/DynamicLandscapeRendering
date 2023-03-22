#include "d3dclass.h"

// Function to initialize DX11
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear) {
    // Store the vsync setting.
    m_vsync_enabled = vsync;

    // Create a DirectX graphics interface factory.
    IDXGIFactory* factory;
    HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result)) {
        return false;
    }

    // Use the factory to create an adapter for the primary graphics interface (video card).
    IDXGIAdapter* adapter;
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result)) {
        return false;
    }

    // Enumerate the primary adapter output (monitor).
    IDXGIOutput* adapterOutput;
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result)) {
        return false;
    }

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
    unsigned int numModes = 0;
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result)) {
        return false;
    }

    // Create a list to hold all the possible display modes for this monitor/video card combination.
    DXGI_MODE_DESC* displayModeList;
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList) {
        return false;
    }

    // Now fill the display mode list structures.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result)) {
        return false;
    }

    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    unsigned int numerator, denominator;
    for (unsigned int i = 0; i < numModes; i++) {
        if (displayModeList[i].Width == (unsigned int)screenWidth) {
            if (displayModeList[i].Height == (unsigned int)screenHeight) {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    // Get the adapter (video card) description.
    DXGI_ADAPTER_DESC adapterDesc;
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result)) {
        return false;
    }

    // Store the dedicated video card memory in megabytes.
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // Release the display mode list.
    delete[] displayModeList;
    displayModeList = nullptr;

    // Release the adapter output.
    adapterOutput->Release();
    adapterOutput = nullptr;

    // Release the adapter.
    adapter->Release();
    adapter = nullptr;

    // Release the factory.
    factory->Release();
    factory = nullptr;

    // Initialize the swap chain description.
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // Set to a single back buffer.
    swapChainDesc.BufferCount = 2;

    // Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    // Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Set the refresh rate of the back buffer.
    if (m_vsync_enabled) {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Set the handle for the window to render to.
    swapChainDesc.OutputWindow = hwnd;

    // Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // Set to full screen or windowed mode.
    if (fullscreen) {
        swapChainDesc.Windowed = false;
    }
    else {
        swapChainDesc.Windowed = true;
    }

    // Set the scan line ordering and scaling to unspecified.
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Discard the back buffer contents after presenting.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    // Don't set the advanced flags.
    swapChainDesc.Flags = 0;

    // Set the feature level to DirectX 11.
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    // Create the swap chain, Direct3D device, and Direct3D device context.
    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
    if (FAILED(result)) {
        return false;
    }

    result = Resize(screenWidth, screenHeight, screenDepth, screenNear);
    if (FAILED(result)) {
        return false;
    }

    // Setup the raster description which will determine how and what polygons will be drawn.
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state from the description we just filled out.
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
    if (FAILED(result)) {
        return false;
    }

    // Now set the rasterizer state.
    m_deviceContext->RSSetState(m_rasterState);

    // Setup a raster description which turns off back face culling.
    rasterDesc.CullMode = D3D11_CULL_NONE;

    // Create the no culling rasterizer state.
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateNoCulling);
    if (FAILED(result)) {
        return false;
    }

    // Setup a raster description which enables wire frame rendering.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    // Create the wire frame rasterizer state.
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateWireframe);
    if (FAILED(result)) {
        return false;
    }

    // Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
    // that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
    depthDisabledStencilDesc.DepthEnable = false;
    depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDisabledStencilDesc.StencilEnable = true;
    depthDisabledStencilDesc.StencilReadMask = 0xFF;
    depthDisabledStencilDesc.StencilWriteMask = 0xFF;
    depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the state using the device.
    result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
    if (FAILED(result)) {
        return false;
    }

    // Create depth stancil state for depth prepass
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
    depthStencilDesc.StencilEnable = false;

    // Create the state using the device.
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStatePass);
    if (FAILED(result)) {
        return false;
    }

    // Clear the blend state description.
    D3D11_BLEND_DESC blendStateDescription;
    ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

    // Create an alpha enabled blend state description.
    blendStateDescription.AlphaToCoverageEnable = FALSE;
    blendStateDescription.IndependentBlendEnable = false;
    blendStateDescription.RenderTarget[0].BlendEnable = true;
    blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    // Create the blend state using the description.
    result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
    if (FAILED(result)) {
        return false;
    }

    // Modify the description to create an alpha disabled blend state description.
    blendStateDescription.RenderTarget[0].BlendEnable = false;
    blendStateDescription.AlphaToCoverageEnable = false;

    // Create the blend state using the description.
    result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
    if (FAILED(result)) {
        return false;
    }

    // Create a blend state description for the alpha-to-coverage blending mode.
    blendStateDescription.AlphaToCoverageEnable = true;
    blendStateDescription.IndependentBlendEnable = false;
    blendStateDescription.RenderTarget[0].BlendEnable = true;
    blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    // Create the blend state using the description.
    result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState2);
    if (FAILED(result)) {
        return false;
    }

    return true;

}

// Resize function
HRESULT D3DClass::Resize(int width, int height, float screenDepth, float screenNear) {

    if (m_renderTargetView) {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }

    HRESULT hr = m_swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    assert(SUCCEEDED(hr));

    // Get the pointer to the back buffer.
    ID3D11Texture2D* backBufferPtr;
    HRESULT result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result)) {
        return result;
    }

    // Create the render target view with the back buffer pointer.
    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if (FAILED(result)) {
        return result;
    }

    // Release pointer to the back buffer as we no longer need it.
    backBufferPtr->Release();
    backBufferPtr = 0;

    if (m_depthStencilView) {
        m_depthStencilView->Release();
        m_depthStencilView = nullptr;
    }

    if (m_depthStencilState) {
        m_depthStencilState->Release();
        m_depthStencilState = nullptr;
    }

    if (m_depthStencilBuffer) {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = nullptr;
    }

    // Initialize the description of the depth buffer.
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if (FAILED(result)) {
        return result;
    }

    // Initialize the description of the stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    depthStencilDesc.StencilEnable = false;

    // Create the depth stencil state.
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if (FAILED(result)) {
        return result;
    }

    // Set the depth stencil state.
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

    // Create the depth stencil view.
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, NULL, &m_depthStencilView);
    if (FAILED(result)) {
        return result;
    }

    // Bind the render target view and depth stencil buffer to the output render pipeline.
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    // Setup the viewport for rendering.
    D3D11_VIEWPORT viewport;
    viewport.Width = (float)width;
    viewport.Height = (float)height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    m_viewport = viewport;

    // Create the viewport.
    m_deviceContext->RSSetViewports(1, &viewport);

    // Setup the projection matrix.
    float fieldOfView, screenAspect;
    fieldOfView = XM_PIDIV4;
    screenAspect = (float)width / (float)height;

    // Create the projection matrix for 3D rendering.
    m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenDepth, screenNear);

    // Initialize the world matrix to the identity matrix.
    m_worldMatrix = XMMatrixIdentity();

    // Create an orthographic projection matrix for 2D rendering.
    m_orthoMatrix = XMMatrixOrthographicLH((float)width, (float)height, screenDepth, screenNear);

    return S_OK;
}

// Function to realese DX11 data
void D3DClass::Shutdown() {
    // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
    if (m_swapChain) {
        m_swapChain->SetFullscreenState(false, NULL);
    }

    if (m_alphaEnableBlendingState2) {
        m_alphaEnableBlendingState2->Release();
        m_alphaEnableBlendingState2 = nullptr;
    }

    if (m_alphaDisableBlendingState) {
        m_alphaDisableBlendingState->Release();
        m_alphaDisableBlendingState = nullptr;
    }

    if (m_alphaEnableBlendingState) {
        m_alphaEnableBlendingState->Release();
        m_alphaEnableBlendingState = nullptr;
    }

    if (m_depthDisabledStencilState) {
        m_depthDisabledStencilState->Release(); 
        m_depthDisabledStencilState = nullptr;
    }

    if (m_rasterStateWireframe) {
        m_rasterStateWireframe->Release();
        m_rasterStateWireframe = nullptr;
    }

    if (m_rasterStateNoCulling) {
        m_rasterStateNoCulling->Release();
        m_rasterStateNoCulling = nullptr;
    }

    if (m_rasterState) {
        m_rasterState->Release();
        m_rasterState = nullptr;
     }

    if (m_depthStencilView) {
        m_depthStencilView->Release();
        m_depthStencilView = nullptr;
    }

    if (m_depthStencilState) {
        m_depthStencilState->Release();
        m_depthStencilState = nullptr;
    }

    if (m_depthStencilStatePass) {
        m_depthStencilStatePass->Release();
        m_depthStencilStatePass = nullptr;
    }

    if (m_depthStencilBuffer) {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = nullptr;
    }

    if (m_renderTargetView) {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }

    if (m_deviceContext) {
        m_deviceContext->Release();
        m_deviceContext = nullptr;
    }

    if (m_device) {
        m_device->Release();
        m_device = nullptr;
    }

    if (m_swapChain) {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }
}

// Function to prepare scene
void D3DClass::BeginScene(float red, float green, float blue, float alpha) {
    // Setup the color to clear the buffer to.
    float color[4];
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // Clear the back buffer.
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

    // Clear the depth buffer.
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 0.0f, 0);
}

// Function to present scene
void D3DClass::EndScene() {
    // Present the back buffer to the screen since rendering is complete.
    if (m_vsync_enabled) {
        // Lock to screen refresh rate.
        m_swapChain->Present(1, 0);
    }
    else {
        // Present as fast as possible.
        m_swapChain->Present(0, 0);
    }
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory) {
    strcpy_s(cardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
}

void D3DClass::EnableAlphaBlending() {
    // Setup the blend factor.
    float blendFactor[4];
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    // Turn on the alpha blending.
    m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void D3DClass::DisableAlphaBlending() {
    // Setup the blend factor.
    float blendFactor[4];
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    // Turn off the alpha blending.
    m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
}

void D3DClass::EnableAlphaToCoverageBlending() {
    // Setup the blend factor.
    float blendFactor[4];
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    // Turn on the alpha blending.
    m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState2, blendFactor, 0xffffffff);
}
