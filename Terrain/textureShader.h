//--------------------------------------------------------------------------------------
// TextureShader  - class for texture terrain shader
//--------------------------------------------------------------------------------------
#ifndef _TEXTURESHADER_H_
#define _TEXTURESHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class TextureShader {
  private:
    struct MatrixBufferType {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

  public:
    // Function to initialize shader
    bool Initialize(ID3D11Device* device, HWND hwnd);
    // Function to realese shader
    void Shutdown() { ShutdownShader(); };
    // Render function
    bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);

  private:
    // Function to initialize shader
    bool InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilename);
    // Function to release shader
    void ShutdownShader();
    // Function to print errors to file
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename);
    // Function to fill shader buffers and params
    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);
    // Render function
    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

  private:
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_layout = nullptr;
    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11SamplerState* m_sampleState = nullptr;
};

#endif
