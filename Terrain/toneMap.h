//--------------------------------------------------------------------------------------
// toneMap  - class for adding post effects
//--------------------------------------------------------------------------------------
#ifndef _TONEMAP_H_
#define _TONEMAP_H_

#include "averageLuminance.h"

using namespace DirectX;
using namespace std;

class ToneMap {
  public:
    // Function to initialize
    bool Initialize(ID3D11Device* device, HWND hwnd, int textureWidth, int textureHeight);
    // Function to realese
    void Shutdown();
    void Resize(ID3D11Device* device, int width, int height);
    // Render function
    void Process(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture, ID3D11RenderTargetView* renderTarget, D3D11_VIEWPORT viewport);
  private:
    // Function to initialize
    bool InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilenam);
    // Function to print error if error happened
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename);

    AverageLuminance* m_averageLuminance = nullptr;
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11SamplerState* m_sampleState = nullptr;
};
#endif
