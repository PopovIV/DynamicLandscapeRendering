//--------------------------------------------------------------------------------------
// TerrainShader  - class for color map terrain shader
//--------------------------------------------------------------------------------------
#ifndef _TERRAINSHADER_H_
#define _TERRAINSHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "light.h"
#include "D3DInclude.h"
#include "defines.h"

using namespace DirectX;
using namespace std;

class TerrainShader {
  private:
    struct MatrixBufferType {
        XMMATRIX worldMatrix;
    };

    struct SceneProjectionBufferType {
        XMMATRIX viewProjectionMatrix;
        XMFLOAT4 planes[6];
        XMFLOAT3 cameraPos;
    };

    struct LightBufferType {
        XMFLOAT4 ambientColor;
        XMFLOAT4 diffuseColor;
        XMFLOAT4 specularColor;
        XMFLOAT3 lightDirection;
        float specularPower;
    };


  public:

    struct ScaleBufferType {
          XMINT4 scales; // x - grass, y - rock, z - slope, w - snow
          XMINT4 detailScale; // x - detail texture scale
    };

    // Function to initialize shader
    bool Initialize(ID3D11Device* device, HWND hwnd);
    // Function to realese shader
    void Shutdown() { ShutdownShader(); }
    // Render function
    bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMFLOAT4* frustumPlanes, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], Light* light, ScaleBufferType scaleBuffer, bool normalPass);

  private:
    // Function to initialize shader
    bool InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* csFilename);
    // Function to release shader
    void ShutdownShader();
    // Function to print errors to file
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename);
    // Function to fill shader buffers and params
    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4* frustumPlanes, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], Light* light, ScaleBufferType scaleBuffer);
    // Render function
    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, bool normalPass);

    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11HullShader* m_hullShader = nullptr;
    ID3D11DomainShader* m_domainShader = nullptr;
    ID3D11ComputeShader* m_pCullShader = nullptr;
    ID3D11InputLayout* m_layout = nullptr;
    ID3D11Buffer* m_worldMatrixBuffer = nullptr;
    ID3D11Buffer* m_pCullParams = nullptr;
    ID3D11Buffer* m_viewProjectionMatrixBuffer = nullptr;
    ID3D11SamplerState* m_samplerState = nullptr;
    ID3D11SamplerState* m_samplerStateNoMips = nullptr;
    ID3D11Buffer* m_lightBuffer = nullptr;
    ID3D11Buffer* m_scaleBuffer = nullptr;

    ID3D11Buffer* m_pInderectArgsSrc = nullptr;
    ID3D11Buffer* m_pInderectArgs = nullptr;
    ID3D11UnorderedAccessView* m_pInderectArgsUAV = nullptr;
    ID3D11Buffer* m_pGeomBufferInstVis = nullptr;
    ID3D11Buffer* m_pGeomBufferInstVisGpu = nullptr;
    ID3D11UnorderedAccessView* m_pGeomBufferInstVisGpu_UAV = nullptr;
};

#endif
