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
    // constructors
    D3DClass();
    D3DClass(const D3DClass&) {};
    // destructor
    ~D3DClass() {};

    // Function to initialize DX11
    bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
    // Function to realese DX11 data
    void Shutdown();

    // Function to prepare scene
    void BeginScene(float red, float green, float blue, float alpha);
    // Function to present scene
    void EndScene();

    ID3D11Device* GetDevice() { return m_device; };
    ID3D11DeviceContext* GetDeviceContext() { return m_deviceContext; };

    void GetProjectionMatrix(XMMATRIX& projectionMatrix) { projectionMatrix = m_projectionMatrix; };
    void GetWorldMatrix(XMMATRIX& worldMatrix) { worldMatrix = m_worldMatrix; };
    void GetOrthoMatrix(XMMATRIX& orthoMatrix) { orthoMatrix = m_orthoMatrix; };

    void GetVideoCardInfo(char* cardName, int& memory);

    void TurnZBufferOn() { m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1); };
    void TurnZBufferOff() { m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1); };

    void TurnOnCulling() { m_deviceContext->RSSetState(m_rasterState); };
    void TurnOffCulling() { m_deviceContext->RSSetState(m_rasterStateNoCulling); };

    void EnableAlphaBlending();
    void EnableAlphaToCoverageBlending();
    void DisableAlphaBlending();

    void EnableWireframe() { m_deviceContext->RSSetState(m_rasterStateWireframe); };
    void DisableWireframe() { m_deviceContext->RSSetState(m_rasterState); };

private:
    bool m_vsync_enabled;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11DepthStencilState* m_depthStencilState;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11RasterizerState* m_rasterState;
    ID3D11RasterizerState* m_rasterStateNoCulling;
    ID3D11RasterizerState* m_rasterStateWireframe;
    XMMATRIX m_projectionMatrix;
    XMMATRIX m_worldMatrix;
    XMMATRIX m_orthoMatrix;
    ID3D11DepthStencilState* m_depthDisabledStencilState;
    ID3D11BlendState* m_alphaEnableBlendingState;
    ID3D11BlendState* m_alphaDisableBlendingState;
    ID3D11BlendState* m_alphaEnableBlendingState2;
};

#endif