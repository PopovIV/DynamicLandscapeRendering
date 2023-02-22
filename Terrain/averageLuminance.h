//--------------------------------------------------------------------------------------
// averageLuminance - class for calculating average luminance
//--------------------------------------------------------------------------------------
#ifndef _AVERAGELUMINANCE_H_
#define _AVERAGELUMINANCE_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include <vector>
#include "renderTexture.h"

using namespace DirectX;
using namespace std;


class AverageLuminance {

public:
    // constructors
    AverageLuminance();
    AverageLuminance(const AverageLuminance& other) {};
    // destructor
    ~AverageLuminance() {};

    // Function to initialize
    bool Initialize(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight);
    // Function to realese
    void Shutdown();
    // Render function
    float Process(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture);

private:
    void CopyTexture(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture, RenderTexture& dst, ID3D11PixelShader* pixelShader);

    bool CreateTextures(ID3D11Device* device, int width, int height);

    // Function to initialize
    bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psCopyFilename, WCHAR* psFilename);

    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

    std::vector<RenderTexture*> m_renderTextures;

    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_copyPixelShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11SamplerState* m_sampleState;
    ID3D11Texture2D* m_luminanceTexture;

    float m_adaptedLuminance;

    LARGE_INTEGER m_qpcFrequency;
    LARGE_INTEGER m_qpcLastTime;
};
#endif
