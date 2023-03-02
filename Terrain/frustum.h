//--------------------------------------------------------------------------------------
// Frustum class used for culling
//--------------------------------------------------------------------------------------
#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include <directxmath.h>
using namespace DirectX;

class Frustum {
public:
    Frustum() {};
    Frustum(const Frustum&) {};
    ~Frustum() {};

    void Initialize(float screenDepth) { m_screenDepth = screenDepth; };

    void ConstructFrustum(XMMATRIX projectionMatrix, XMMATRIX viewMatrix);

    bool CheckPoint(float x, float y, float z);
    bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);
    bool CheckSphere(float xCenter, float yCenter, float zCenter, float radius);
    bool CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize);
    bool CheckRectangle2(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth);

private:
    float m_screenDepth;
    float m_planes[6][4];
};

#endif
