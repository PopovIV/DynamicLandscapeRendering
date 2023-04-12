//--------------------------------------------------------------------------------------
// heightMap - class for calculating cull params from heigh map texture
//--------------------------------------------------------------------------------------
#ifndef _HEIGHTMAP_H_
#define _HEIGHTMAP_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include <vector>
#include "renderTexture.h"
#include "defines.h"

using namespace DirectX;
using namespace std;

class HeightMap {
public:
    // Function to initialize
    bool Initialize(ID3D11Device* device, HWND hwnd);
    // Function to realese
    void Shutdown();
    // Render function
    bool Process(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture);
private:
    void CopyTexture(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture, RenderTexture& dst, ID3D11PixelShader* pixelShader);

    // Function to initialize
    bool InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* convertPsFilename, const wchar_t* psFilename);

    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename);

    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_convertPixelShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11ShaderResourceView* m_resTextureView = nullptr;
    ID3D11SamplerState* m_samplerState = nullptr;
    ID3D11SamplerState* m_samplerMinState = nullptr;
    ID3D11SamplerState* m_samplerMaxState = nullptr;
};
#endif
