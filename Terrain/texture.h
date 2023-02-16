#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3d11.h>
#include <stdio.h>
#include "DDSTextureLoader.h"
#include "stringConverter.h"

#include "stb_image.h"

class Texture {
private:
    struct TargaHeader {
        unsigned char data1[12];
        unsigned short width;
        unsigned short height;
        unsigned char bpp;
        unsigned char data2;
    };

public:
    enum TextureType {
        Targa,
        DDS,
        ANY,
    };

    // constructors
    Texture();
    Texture(const Texture&) {};
    // destructor
    ~Texture() {};

    // Function to initialize texture
    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename, TextureType type, bool sRGB = false);
    // Function to realese texture
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture() { return m_textureView; };

private:
    bool LoadTarga(const wchar_t* filename, int& height, int& width);

private:
    unsigned char* m_targaData;
    ID3D11Texture2D* m_texture;
    ID3D11ShaderResourceView* m_textureView;

};

#endif
