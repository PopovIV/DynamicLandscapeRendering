//--------------------------------------------------------------------------------------
// TextureManager class manages multiple textures
//--------------------------------------------------------------------------------------
#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include "texture.h"

class TextureManager {
public:
    // Function to initialize texture manager
    bool Initialize(int count);
    // Function to realese texture manager
    void Shutdown();

    // Texture loader function
    bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename, int location, Texture::TextureType type, bool sRGB = false);
    // Function to get texture by id
    ID3D11ShaderResourceView* GetTexture(int id) { return m_TextureArray[id].GetTexture(); };
private:
    Texture* m_TextureArray = nullptr;
    int m_textureCount = 0;
};

#endif
