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

    // Function to check if point is in frustum
    bool CheckPoint(float x, float y, float z);
    // Function to check if cube is in frustum
    bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);
    // Function to check if sphere is in frustum
    bool CheckSphere(float xCenter, float yCenter, float zCenter, float radius);
    // Functions to check if rectengle is in frustum
    bool CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize);
    bool CheckRectangle2(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth);
    bool CheckRectangle3(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth);

    void SetLockView(bool lockView) { m_lockView = lockView; };
  private:
    float m_screenDepth;
    float m_planes[6][4];
    XMMATRIX m_finalMatrix;
    bool m_lockView = false;
};

#endif
