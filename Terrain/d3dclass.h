//--------------------------------------------------------------------------------------
// d3dclass to work with DX11 API
//--------------------------------------------------------------------------------------
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class D3DClass {
  public:
    // Function to initialize DX11
    bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
    // Function to realese DX11 data
    void Shutdown();
    // Resize function
    HRESULT Resize(int width, int height, float screenDepth, float screenNear);

    // Function to prepare scene
    void BeginScene(float red, float green, float blue, float alpha);
    // Function to present scene
    void EndScene();

    // Function to get device
    ID3D11Device* GetDevice() { return m_device; };
    // Function to get device context
    ID3D11DeviceContext* GetDeviceContext() { return m_deviceContext; };

    // Function to get projection matrix
    void GetProjectionMatrix(XMMATRIX& projectionMatrix) { projectionMatrix = m_projectionMatrix; };
    // Function to get world matrix
    void GetWorldMatrix(XMMATRIX& worldMatrix) { worldMatrix = m_worldMatrix; };
    // Function to get ortho matrix
    void GetOrthoMatrix(XMMATRIX& orthoMatrix) { orthoMatrix = m_orthoMatrix; };

    void GetVideoCardInfo(char* cardName, int& memory);

    void TurnZBufferOn() { m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1); };
    void TurnZBufferOff() { m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1); };

    void TurnOnCulling() { m_deviceContext->RSSetState(m_rasterState); };
    void TurnOffCulling() { m_deviceContext->RSSetState(m_rasterStateNoCulling); };

    ID3D11DepthStencilView* GetDepthStencilView() { return m_depthStencilView; };
    void SetBackBufferRenderTarget() { m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView); };
    ID3D11RenderTargetView* GetRenderTarget() { return  m_renderTargetView; };
    D3D11_VIEWPORT GetViewPort() { return m_viewport; };

    void EnableAlphaBlending();
    void EnableAlphaToCoverageBlending();
    void DisableAlphaBlending();

    void EnableWireframe() { m_deviceContext->RSSetState(m_rasterStateWireframe); };
    void DisableWireframe() { m_deviceContext->RSSetState(m_rasterState); };

  private:
    bool m_vsync_enabled;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
    ID3D11Texture2D* m_depthStencilBuffer = nullptr;
    ID3D11DepthStencilState* m_depthStencilState = nullptr;
    ID3D11DepthStencilView* m_depthStencilView = nullptr;
    ID3D11RasterizerState* m_rasterState = nullptr;
    ID3D11RasterizerState* m_rasterStateNoCulling = nullptr;
    ID3D11RasterizerState* m_rasterStateWireframe = nullptr;
    XMMATRIX m_projectionMatrix;
    XMMATRIX m_worldMatrix;
    XMMATRIX m_orthoMatrix;
    ID3D11DepthStencilState* m_depthDisabledStencilState = nullptr;
    ID3D11BlendState* m_alphaEnableBlendingState = nullptr;
    ID3D11BlendState* m_alphaDisableBlendingState = nullptr;
    ID3D11BlendState* m_alphaEnableBlendingState2 = nullptr;

    D3D11_VIEWPORT m_viewport;
};

#endif
