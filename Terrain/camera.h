//--------------------------------------------------------------------------------------
// Camera handles the location of the camera in the world 
//--------------------------------------------------------------------------------------
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <directxmath.h>

using namespace DirectX;

class Camera {
  public:
    // Function to set camera position
    void SetPosition(float x, float y, float z);
    // Function to set camera rotation
    void SetRotation(float x, float y, float z);

    // Function to get camera location
    XMFLOAT3 GetPosition() { return XMFLOAT3(m_positionX, m_positionY, m_positionZ); };
    // Function to get camera rotation
    XMFLOAT3 GetRotation() { XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ); };

    // Render function
    void Render();
    // Function to get view matrix 
    void GetViewMatrix(XMMATRIX& viewMatrix) { viewMatrix = m_viewMatrix; };
    // Function to get right view Matrix
    void RenderBaseViewMatrix();
    // Function to get base view matrix
    void GetBaseViewMatrix(XMMATRIX& baseViewMatrix) { baseViewMatrix = m_baseViewMatrix; };

  private:
    float m_positionX = 0.0f, m_positionY = 0.0f, m_positionZ = 0.0f;
    float m_rotationX = 0.0f, m_rotationY = 0.0f, m_rotationZ = 0.0f;
    XMMATRIX m_viewMatrix = XMMATRIX(), m_baseViewMatrix = XMMATRIX();
};

#endif
