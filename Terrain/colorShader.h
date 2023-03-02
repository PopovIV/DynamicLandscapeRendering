//--------------------------------------------------------------------------------------
// ColorShader  - class for main terrain shader
//--------------------------------------------------------------------------------------
#ifndef _COLORSHADER_H_
#define _COLORSHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class ColorShader {
private:
    struct MatrixBufferType {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

public:
    // constructors
    ColorShader();
    ColorShader(const ColorShader& other) {};
    // destructor
    ~ColorShader() {};

    // Function to initialize shader
    bool Initialize(ID3D11Device* device, HWND hwnd);
    // Function to release shader
    void Shutdown() { ShutdownShader(); };
    // Render function
    bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

private:
    // Function to initialize shader
    bool InitializeShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename, const wchar_t* psFilenam);
    // Function to release shader
    void ShutdownShader();
    // Function to print errors to file
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const wchar_t* shaderFilename);
    // Function to fill shader buffers and params
    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
    // Render function
    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
};

#endif
