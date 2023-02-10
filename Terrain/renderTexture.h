////////////////////////////////////////////////////////////////////////////////
// Class for rendering terrain to render target
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include <d3d11.h>

class RenderTexture {
  public:
    RenderTexture();
    RenderTexture(const RenderTexture& other) {};
    ~RenderTexture() {};

    bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight);
    void Shutdown();

    void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView) { deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView); };
    void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha);
    ID3D11ShaderResourceView* GetShaderResourceView() { return m_shaderResourceView; };

  private:
    ID3D11Texture2D* m_renderTargetTexture;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11ShaderResourceView* m_shaderResourceView;
};

#endif
