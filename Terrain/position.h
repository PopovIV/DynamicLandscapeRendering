//--------------------------------------------------------------------------------------
// Position class encapsulate the camera/viewer location and camera movement
//--------------------------------------------------------------------------------------

#ifndef _POSITION_H_
#define _POSITION_H_

#include <math.h>
#include <directxmath.h>

using namespace DirectX;

class Position {
  public:
    // Function to set position for viever/camera
    void SetPosition(float x, float y, float z);
    // Function to set rotation for viever/camera
    void SetRotation(float x, float y, float z);

    // Function to get position for viever/camera
    void GetPosition(float& x, float& y, float& z);
    // Function to get rotation for viever/camera
    void GetRotation(float& x, float& y, float& z);

    // Function to set frame time
    void SetFrameTime(float time) { m_frameTime = time; };

    // Function to calculate forward speed and movement
    void MoveForward(bool keydown);
    // Function to calculate backward speed and movement
    void MoveBackward(bool keydown);
    // Function to calculate upward speed and movement
    void MoveUpward(bool keydown);
    // Function to calculate downward speed and movement
    void MoveDownward(bool keydown);
    // Function to calculate left move speed and movement
    void MoveLeft(bool keydown);
    // Function to calculate right move speed and movement
    void MoveRight(bool keydown);
    // Function to calculate left turn speed and movement
    void TurnLeft(bool keydown);
    // Function to calculate right turn speed and movement
    void TurnRight(bool keydown);
    // Function to calculate upward turn speed and movement
    void LookUpward(bool keydown);
    // Function to calculate downward turn speed and movement
    void LookDownward(bool keydown);
    // Function to calculate turn using mouse
    void TurnMouse(XMFLOAT2 mouseMove);

  private:
    float m_positionX = 0.0f, m_positionY = 0.0f, m_positionZ = 0.0f;
    float m_rotationX = 0.0f, m_rotationY = 0.0f, m_rotationZ = 0.0f;

    float m_frameTime = 0.0f;

    float m_forwardSpeed = 0.0f, m_backwardSpeed = 0.0f;
    float m_rightSpeed = 0.0f, m_leftSpeed = 0.0f;
    float m_upwardSpeed = 0.0f, m_downwardSpeed = 0.0f;
    float m_leftTurnSpeed = 0.0f, m_rightTurnSpeed = 0.0f;
    float m_lookUpSpeed = 0.0f, m_lookDownSpeed = 0.0f;
};

#endif
