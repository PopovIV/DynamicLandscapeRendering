//--------------------------------------------------------------------------------------
// TextureManager class manages multiple textures
//--------------------------------------------------------------------------------------
#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include "texture.h"

class TextureManager {
public:
    // constructors
    TextureManager() { m_TextureArray = nullptr; };
    TextureManager(const TextureManager& other) {};
    // destructor
    ~TextureManager() {};

    // Function to initialize texture manager
    bool Initialize(int count);
    // Function to realese texture manager
    void Shutdown();

    // Texture loader function
    bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename, int location);

    ID3D11ShaderResourceView* GetTexture(int id) { return m_TextureArray[id].GetTexture(); };

private:
    Texture* m_TextureArray;
    int m_textureCount;
};

#endif