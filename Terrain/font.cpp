#include "font.h"

Font::Font() {

    m_Font = 0;
    m_Texture = 0;

}

// Function to initialize font class
bool Font::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* fontFilename, const wchar_t* textureFilename, float fontHeight, int spaceSize) {

    bool result;

    // Store the height of the font.
    m_fontHeight = fontHeight;

    // Store the size of spaces in pixel size.
    m_spaceSize = spaceSize;

    // Load in the text file containing the font data.
    result = LoadFontData(fontFilename);
    if (!result)
        return false;


    // Load the texture that has the font characters on it.
    result = LoadTexture(device, deviceContext, textureFilename);
    if (!result)
        return false;

    return true;

}

// Function to realese font class
void Font::Shutdown() {

    // Release the font texture.
    ReleaseTexture();

    // Release the font data.
    ReleaseFontData();

}

// Function to load data for font
bool Font::LoadFontData(const wchar_t* filename) {

    ifstream fin;
    int i;
    char temp;

    // Create the font spacing buffer.
    m_Font = new FontType[95];
    if (!m_Font)
        return false;

    // Read in the font size and spacing between chars.
    fin.open(filename);
    if (fin.fail())
        return false;

    // Read in the 95 used ascii characters for text.
    for (i = 0; i < 95; i++) {
        fin.get(temp);
        while (temp != ' ')
            fin.get(temp);
        fin.get(temp);
        while (temp != ' ')
            fin.get(temp);
        fin >> m_Font[i].left;
        fin >> m_Font[i].right;
        fin >> m_Font[i].size;
    }
    // Close the file.
    fin.close();

    return true;

}

// Function to realese data from font
void Font::ReleaseFontData() {

    // Release the font data array.
    if (m_Font) {
        delete[] m_Font;
        m_Font = 0;
    }

}

// Texture loader function
bool Font::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename) {

    bool result;

    // Create the texture object.
    m_Texture = new Texture;
    if (!m_Texture)
        return false;

    // Initialize the texture object.
    result = m_Texture->Initialize(device, deviceContext, filename, Texture::Targa);
    if (!result)
        return false;

    return true;

}

// Function to realse texture
void Font::ReleaseTexture() {

    // Release the texture object.
    if (m_Texture) {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = 0;
    }

}

void Font::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY) {

    VertexType* vertexPtr;
    int numLetters, index, i, letter;

    // Coerce the input vertices into a VertexType structure.
    vertexPtr = (VertexType*)vertices;

    // Get the number of letters in the sentence.
    numLetters = (int)strlen(sentence);

    // Initialize the index to the vertex array.
    index = 0;

    // Draw each letter onto a quad.
    for (i = 0; i < numLetters; i++) {
        letter = ((int)sentence[i]) - 32;

        // If the letter is a space then just move over three pixels.
        if (letter == 0)
            drawX = drawX + (float)m_spaceSize;
        else {
            // First triangle in quad.
            vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // Bottom right.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3(drawX, (drawY - m_fontHeight), 0.0f);  // Bottom left.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
            index++;

            // Second triangle in quad.
            vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // Bottom right.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
            index++;

            // Update the x location for drawing by the size of the letter and one pixel.
            drawX = drawX + m_Font[letter].size + 1.0f;
        }
    }

}


int Font::GetSentencePixelLength(char* sentence) {

    int pixelLength, numLetters, i, letter;

    pixelLength = 0;
    numLetters = (int)strlen(sentence);

    for (i = 0; i < numLetters; i++) {
        letter = ((int)sentence[i]) - 32;

        // If the letter is a space then count it as three pixels.
        if (letter == 0)
            pixelLength += m_spaceSize;
        else
            pixelLength += (m_Font[letter].size + 1);
    }

    return pixelLength;

}