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
using namespace DirectX;
using namespace std;

class TerrainShader {
private:
    struct MatrixBufferType {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
        XMFLOAT3 cameraPos;
    };

    struct LightBufferType {
        XMFLOAT4 ambientColor;
        XMFLOAT4 diffuseColor;
        XMFLOAT4 specularColor;
        XMFLOAT3 lightDirection;
        float specularPower;
    };

    struct ScaleBufferType {
        float grassScale;
        float rockScale;
        float slopeScale;
        float snowScale;
        float detailScale;
        XMFLOAT3 tmp;
    };

public:
    // constructors
    TerrainShader();
    TerrainShader(const TerrainShader& other) {};
    // destructor
    ~TerrainShader() {};

    // Function to initialize shader
    bool Initialize(ID3D11Device* device, HWND hwnd);
    // Function to realese shader
    void Shutdown() { ShutdownShader(); }
    // Render function
    bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], ID3D11ShaderResourceView* normalMaps[], ID3D11ShaderResourceView* roughMaps[], ID3D11ShaderResourceView* aoMaps[], Light* light, XMFLOAT4 scales, float detailScale);

private:
    // Function to initialize shader
    bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename, WCHAR* hsFilename, WCHAR* dsFilename);
    // Function to release shader
    void ShutdownShader();
    // Function to print errors to file
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
    // Function to fill shader buffers and params
    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos, ID3D11ShaderResourceView* textures[], ID3D11ShaderResourceView* normalMaps[], ID3D11ShaderResourceView* roughMaps[], ID3D11ShaderResourceView* aoMaps[], Light* light, XMFLOAT4 scales, float detailScale);
    // Render function
    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11HullShader* m_hullShader;
    ID3D11DomainShader* m_domainShader;
    ID3D11InputLayout* m_layout;
    ID3D11Buffer* m_matrixBuffer;
    ID3D11SamplerState* m_samplerState;
    ID3D11SamplerState* m_samplerStateNoMips;
    ID3D11Buffer* m_lightBuffer;
    ID3D11Buffer* m_scaleBuffer;
};

#endif
