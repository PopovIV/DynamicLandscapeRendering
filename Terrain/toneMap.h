//--------------------------------------------------------------------------------------
// toneMap  - class for adding post effects
//--------------------------------------------------------------------------------------
#ifndef _TONEMAP_H_
#define _TONEMAP_H_

#include "averageLuminance.h"

using namespace DirectX;
using namespace std;

__declspec(align(16))
struct LuminanceConstantBuffer
{
    float AverageLuminance;
};

class ToneMap {

public:
    // constructors
    ToneMap();
    ToneMap(const ToneMap& other) {};
    // destructor
    ~ToneMap() {};

    // Function to initialize
    bool Initialize(ID3D11Device* device, HWND hwnd, int textureWidth, int textureHeight);
    // Function to realese
    void Shutdown();
    // Render function
    void Process(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture, ID3D11RenderTargetView* renderTarget, D3D11_VIEWPORT viewport);

private:
    // Function to initialize
    bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilenam);

    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

    AverageLuminance* m_averageLuminance;
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11SamplerState* m_sampleState;
    ID3D11Buffer* m_luminanceBuffer;

};
#endif