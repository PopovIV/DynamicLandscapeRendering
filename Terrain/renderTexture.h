////////////////////////////////////////////////////////////////////////////////
// Class for rendering terrain to render target
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include <d3d11.h>

class RenderTexture {
  public:
    // Constructors
    RenderTexture();
    RenderTexture(ID3D11Device* device, int textureWidth, int textureHeight) { Initialize(device, textureWidth, textureHeight); };

    // Function to initialize render texture class
    bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight);
    // Function to realese render texture class
    void Shutdown();

    // Function to set render target
    void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView) { deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView); };
    // Function to clear render target
    void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha);
    // Function to get texture from render texture
    ID3D11Texture2D* GetRenderTarget() { return m_renderTargetTexture; };
    // Function to get render target view
    ID3D11RenderTargetView* GetRenderTargetView() { return m_renderTargetView; };
    // Function to get shader resource view
    ID3D11ShaderResourceView* GetShaderResourceView() { return m_shaderResourceView; };
    // Function to get view port 
    D3D11_VIEWPORT GetViewPort() { return m_viewport; };

  private:
    ID3D11Texture2D* m_renderTargetTexture = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
    ID3D11ShaderResourceView* m_shaderResourceView = nullptr;
    D3D11_VIEWPORT m_viewport;
};

#endif
