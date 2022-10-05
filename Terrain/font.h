//--------------------------------------------------------------------------------------
// Font class for user interface
//--------------------------------------------------------------------------------------

#ifndef _FONT_H_
#define _FONT_H_

#include <directxmath.h>
#include <fstream>
#include "stringConverter.h"
using namespace DirectX;
using namespace std;

#include "texture.h"

class Font {
private:
    struct FontType {
        float left, right;
        int size;
    };

    struct VertexType {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    // constructors
    Font();
    Font(const Font&) {};
    // destructor
    ~Font() {};

    // Function to initialize font class
    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* fontFilename, const wchar_t* textureFilename, float fontHeight, int spaceSize);
    // Function to realese font class
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture() { return m_Texture->GetTexture(); };
    void BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY);
    int GetSentencePixelLength(char* sentence);
    int GetFontHeight() { return (int)m_fontHeight; };

private:
    // Function to load data for font
    bool LoadFontData(const wchar_t* filename);
    // Function to realese data from font
    void ReleaseFontData();
    // Texture loader function
    bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename);
    // Function to realse texture
    void ReleaseTexture();

private:
    FontType* m_Font;
    Texture* m_Texture;
    float m_fontHeight;
    int m_spaceSize;
};

#endif