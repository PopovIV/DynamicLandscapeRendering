//--------------------------------------------------------------------------------------
// Frustum class used for culling
//--------------------------------------------------------------------------------------
#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include <directxmath.h>
using namespace DirectX;

class Frustum {
  public:
    // Function to initialize frustum class
    void Initialize(float screenDepth) { m_screenDepth = screenDepth; };
    // Function to build frustum
    void ConstructFrustum(XMMATRIX projectionMatrix, XMMATRIX viewMatrix);

    XMFLOAT4* GetPlanes() { return m_planes; };

    void SetLockView(bool lockView) { m_lockView = lockView; };
  private:
    float m_screenDepth;
    XMFLOAT4 m_planes[6];
    XMMATRIX m_finalMatrix;
    bool m_lockView = false;
};

#endif
