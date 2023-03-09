#include "texturemanager.h"

// Function to initialize texture manager
bool TextureManager::Initialize(int count) {
    m_textureCount = count;

    // Create the color texture object.
    m_TextureArray = new Texture[m_textureCount];
    if (!m_TextureArray)
        return false;

    return true;
}

// Function to realese texture manager
void TextureManager::Shutdown() {
    // Release the texture objects.
    if (m_TextureArray) {
        for (int i = 0; i < m_textureCount; i++) {
            m_TextureArray[i].Shutdown();
        }
        delete[] m_TextureArray;
        m_TextureArray = nullptr;
    }
}

// Texture loader function
bool TextureManager::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename, int location, Texture::TextureType type, bool sRGB) {
    // Initialize the color texture object.
    if (!m_TextureArray[location].Initialize(device, deviceContext, filename, type, sRGB)) {
        return false;
    }

    return true;
}
