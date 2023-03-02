//--------------------------------------------------------------------------------------
// LightShader  - class for light terrain shader
//--------------------------------------------------------------------------------------
#ifndef _LIGHTSHADER_H_
#define _LIGHTSHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class LightShader {
private:
    struct MatrixBufferType {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    struct LightBufferType {
        XMFLOAT4 diffuseColor;
        XMFLOAT3 lightDirection;
       float padding;
    };

public:
    // constructors
    LightShader();
    LightShader(const LightShader& other) {};
    // destructor
    ~LightShader() {};

    // Function to initialize shader
    bool Initialize(ID3D11Device* device, HWND hwnd);
    // Function to realese shader
    void Shutdown() { ShutdownShader(); }
    // Render function
    bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

private:
    // Function to initialize shader
    bool InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilename);
    // Function to release shader
    void ShutdownShader();
    // Function to print errors to file
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename);
    // Function to fill shader buffers and params
    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);
    // Render function
    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11Buffer* m_matrixBuffer;
    ID3D11SamplerState* m_sampleState;
    ID3D11Buffer* m_lightBuffer;
};

#endif
