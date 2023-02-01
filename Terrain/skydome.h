//--------------------------------------------------------------------------------------
// Skydome  - class for skydome
//--------------------------------------------------------------------------------------
#ifndef _SKYDOME_H_
#define _SKYDOME_H_

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

class SkyDome {
private:
    struct ModelType {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

    struct VertexType {
        XMFLOAT3 position;
    };

public:
    SkyDome();
    SkyDome(const SkyDome& other) {};
    ~SkyDome() {};

    bool Initialize(ID3D11Device* device);
    void Shutdown();
    void Render(ID3D11DeviceContext* deviceContext);

    int GetIndexCount() { return m_indexCount; };
    XMFLOAT4 GetApexColor() { return m_apexColor; };
    XMFLOAT4 GetCenterColor() { return m_centerColor; };

private:
    bool LoadSkyDomeModel(char*);
    void ReleaseSkyDomeModel();

    bool InitializeBuffers(ID3D11Device* device);
    void ReleaseBuffers();
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
    ModelType* m_model;
    int m_vertexCount, m_indexCount;
    ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
    XMFLOAT4 m_apexColor, m_centerColor;
};

#endif
