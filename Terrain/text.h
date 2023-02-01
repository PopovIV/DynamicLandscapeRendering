//--------------------------------------------------------------------------------------
// Text class for user interface
//--------------------------------------------------------------------------------------

#ifndef _TEXT_H_
#define _TEXT_H_

#include "font.h"
#include "shadermanager.h"

class Text {
private:
    struct VertexType {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    // constructors
    Text();
    Text(const Text& other) {};
    // destructor
    ~Text() {};

    // Function to initialize text class
    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, int maxLength, bool shadow, Font* Font, char* text, int positionX, int positionY, float red, float green, float blue);
    // Function to realese all data
    void Shutdown();
    // Render data
    void Render(ID3D11DeviceContext* deviceContext, ShaderManager* ShaderManager, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix, ID3D11ShaderResourceView* fontTexture);
    // Function to update string to render
    bool UpdateSentence(ID3D11DeviceContext* deviceContext, Font* Font, char* text, int positionX, int positionY, float red, float green, float blue);

private:
    // Function to initialize string to render
    bool InitializeSentence(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Font* Font, char* text, int positionX, int positionY, float red, float green, float blue);
    // Function to render string
    void RenderSentence(ID3D11DeviceContext* deviceContext, ShaderManager* ShaderManager, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix, ID3D11ShaderResourceView* fontTexture);

private:
    ID3D11Buffer* m_vertexBuffer, * m_indexBuffer, * m_vertexBuffer2, * m_indexBuffer2;
    int m_screenWidth, m_screenHeight, m_maxLength, m_vertexCount, m_indexCount;
    bool m_shadow;
    XMFLOAT4 m_pixelColor;
};

#endif
