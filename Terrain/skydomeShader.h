//--------------------------------------------------------------------------------------
// skydomeShader  - class for skydome shader
//--------------------------------------------------------------------------------------
#ifndef _SKYDOMESHADER_H_
#define _SKYDOMESHADER_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class SkyDomeShader {
  private:
    struct MatrixBufferType {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    struct ColorBufferType {
        XMFLOAT4 apexColor;
        XMFLOAT4 centerColor;
    };

  public:
    bool Initialize(ID3D11Device* device, HWND hwnd);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 apexColor, XMFLOAT4 centerColor);

  private:
    bool InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilename);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename);

    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 apexColor, XMFLOAT4 centerColor);
    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_layout = nullptr;
    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11Buffer* m_colorBuffer = nullptr;
};

#endif
