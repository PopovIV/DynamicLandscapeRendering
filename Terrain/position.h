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
    // constructors
    Position();
    Position(const Position& other) {};
    // destructor
    ~Position() {};

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
    float m_positionX, m_positionY, m_positionZ;
    float m_rotationX, m_rotationY, m_rotationZ;

    float m_frameTime;

    float m_forwardSpeed, m_backwardSpeed;
    float m_rightSpeed, m_leftSpeed;
    float m_upwardSpeed, m_downwardSpeed;
    float m_leftTurnSpeed, m_rightTurnSpeed;
    float m_lookUpSpeed, m_lookDownSpeed;
};

#endif