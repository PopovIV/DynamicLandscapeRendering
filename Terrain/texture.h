#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <d3d11.h>
#include <stdio.h>

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
    // constructors
    Texture();
    Texture(const Texture&) {};
    // destructor
    ~Texture() {};

    // Function to initialize texture
    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename);
    // Function to realese texture
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture() { return m_textureView; };

private:
    bool LoadTarga(char* filename, int& height, int& width);

private:
    unsigned char* m_targaData;
    ID3D11Texture2D* m_texture;
    ID3D11ShaderResourceView* m_textureView;

};

#endif