#include "position.h"

// Function to set position for viever/camera
void Position::SetPosition(float x, float y, float z) {
    m_positionX = x;
    m_positionY = y;
    m_positionZ = z;
}

// Function to set rotation for viever/camera
void Position::SetRotation(float x, float y, float z) {
    m_rotationX = x;
    m_rotationY = y;
    m_rotationZ = z;
}

// Function to get position for viever/camera
void Position::GetPosition(float& x, float& y, float& z) {
    x = m_positionX;
    y = m_positionY;
    z = m_positionZ;
}

// Function to get rotation for viever/camera
void Position::GetRotation(float& x, float& y, float& z) {
    x = m_rotationX;
    y = m_rotationY;
    z = m_rotationZ;
}

// Function to calculate forward speed and movement
void Position::MoveForward(bool keydown) {
    // Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_forwardSpeed += m_frameTime * 1.0f;
        if (m_forwardSpeed > (m_frameTime * 30.0f))
            m_forwardSpeed = m_frameTime * 30.0f;
    }
    else {
        m_forwardSpeed -= m_frameTime * 0.5f;
        if (m_forwardSpeed < 0.0f)
            m_forwardSpeed = 0.0f;
    }

    // Convert degrees to radians.
    float radians = m_rotationY * 0.0174532925f;

    // Update the position.
    m_positionX += sinf(radians) * m_forwardSpeed;
    m_positionY -= sinf(m_rotationX * 0.0174532925f) * m_forwardSpeed;
    m_positionZ += cosf(radians) * m_forwardSpeed;
}

// Function to calculate backward speed and movement
void Position::MoveBackward(bool keydown) {
    // Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_backwardSpeed += m_frameTime * 1.0f;
        if (m_backwardSpeed > (m_frameTime * 30.0f))
            m_backwardSpeed = m_frameTime * 30.0f;
    }
    else {
        m_backwardSpeed -= m_frameTime * 0.5f;
        if (m_backwardSpeed < 0.0f)
            m_backwardSpeed = 0.0f;
    }

    // Convert degrees to radians.
    float radians = m_rotationY * 0.0174532925f;

    // Update the position.
    m_positionX -= sinf(radians) * m_backwardSpeed;
    m_positionY += sinf(m_rotationX * 0.0174532925f) * m_backwardSpeed;
    m_positionZ -= cosf(radians) * m_backwardSpeed;
}

// Function to calculate upward speed and movement
void Position::MoveUpward(bool keydown) {
    // Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_upwardSpeed += m_frameTime * 1.5f;
        if (m_upwardSpeed > (m_frameTime * 10.0f))
            m_upwardSpeed = m_frameTime * 10.0f;
    }
    else {
        m_upwardSpeed -= m_frameTime * 0.5f;
        if (m_upwardSpeed < 0.0f)
            m_upwardSpeed = 0.0f;
    }

    // Update the height position.
    m_positionY += m_upwardSpeed;
}

// Function to calculate downward speed and movement
void Position::MoveDownward(bool keydown) {
    // Update the downward speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_downwardSpeed += m_frameTime * 1.5f;
        if (m_downwardSpeed > (m_frameTime * 10.0f))
            m_downwardSpeed = m_frameTime * 10.0f;
    }
    else {
        m_downwardSpeed -= m_frameTime * 0.5f;
        if (m_downwardSpeed < 0.0f)
            m_downwardSpeed = 0.0f;
    }

    // Update the height position.
    m_positionY -= m_downwardSpeed;
}

// Function to calculate left move speed and movement
void Position::MoveLeft(bool keydown) {
    // Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_leftSpeed += m_frameTime * 1.0f;
        if (m_leftSpeed > (m_frameTime * 10.0f))
            m_leftSpeed = m_frameTime * 10.0f;
    }
    else {
        m_leftSpeed -= m_frameTime * 0.5f;
        if (m_leftSpeed < 0.0f)
            m_leftSpeed = 0.0f;
    }

    float radians = m_rotationY * XM_PI / 180;

    // Update the height position.
    m_positionX -= cosf(radians) * m_leftSpeed;
    m_positionZ += sinf(radians) * m_leftSpeed;
}

// Function to calculate right move speed and movement
void Position::MoveRight(bool keydown) {
    // Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_rightSpeed += m_frameTime * 1.0f;
        if (m_rightSpeed > (m_frameTime * 10.0f))
            m_rightSpeed = m_frameTime * 10.0f;
    }
    else {
        m_rightSpeed -= m_frameTime * 0.5f;
        if (m_rightSpeed < 0.0f)
            m_rightSpeed = 0.0f;
    }

    float radians = m_rotationY * XM_PI / 180;

    // Update the height position.
    m_positionX += cosf(radians) * m_rightSpeed;
    m_positionZ -= sinf(radians) * m_rightSpeed;
};

// Function to calculate left turn speed and movement
void Position::TurnLeft(bool keydown) {
    // Update the left turn speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_leftTurnSpeed += m_frameTime * 5.0f;
        if (m_leftTurnSpeed > (m_frameTime * 75.0f))
            m_leftTurnSpeed = m_frameTime * 75.0f;
    }
    else {
        m_leftTurnSpeed -= m_frameTime * 3.5f;
        if (m_leftTurnSpeed < 0.0f)
            m_leftTurnSpeed = 0.0f;
    }

    // Update the rotation.
    m_rotationY -= m_leftTurnSpeed;

    // Keep the rotation in the 0 to 360 range.
    if (m_rotationY < 0.0f) {
        m_rotationY += 360.0f;
    }
}

// Function to calculate right turn speed and movement
void Position::TurnRight(bool keydown) {
    // Update the right turn speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_rightTurnSpeed += m_frameTime * 5.0f;
        if (m_rightTurnSpeed > (m_frameTime * 75.0f))
           m_rightTurnSpeed = m_frameTime * 75.0f;
    }
    else {
        m_rightTurnSpeed -= m_frameTime * 3.5f;
        if (m_rightTurnSpeed < 0.0f)
            m_rightTurnSpeed = 0.0f;
    }

    // Update the rotation.
    m_rotationY += m_rightTurnSpeed;

    // Keep the rotation in the 0 to 360 range.
    if (m_rotationY > 360.0f) {
        m_rotationY -= 360.0f;
    }
}

// Function to calculate upward turn speed and movement
void Position::LookUpward(bool keydown) {
    // Update the upward rotation speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
         m_lookUpSpeed += m_frameTime * 7.5f;
         if (m_lookUpSpeed > (m_frameTime * 75.0f))
            m_lookUpSpeed = m_frameTime * 75.0f;
    }
    else {
        m_lookUpSpeed -= m_frameTime * 2.0f;
        if (m_lookUpSpeed < 0.0f)
            m_lookUpSpeed = 0.0f;
    }

    // Update the rotation.
    m_rotationX -= m_lookUpSpeed;

    // Keep the rotation maximum 90 degrees.
    if (m_rotationX > 90.0f) {
        m_rotationX = 90.0f;
    }
}

// Function to calculate downward turn speed and movement
void Position::LookDownward(bool keydown) {
    // Update the downward rotation speed movement based on the frame time and whether the user is holding the key down or not.
    if (keydown) {
        m_lookDownSpeed += m_frameTime * 7.5f;
        if (m_lookDownSpeed > (m_frameTime * 75.0f))
            m_lookDownSpeed = m_frameTime * 75.0f;
    }
    else {
        m_lookDownSpeed -= m_frameTime * 2.0f;
        if (m_lookDownSpeed < 0.0f)
            m_lookDownSpeed = 0.0f;
    }

    // Update the rotation.
    m_rotationX += m_lookDownSpeed;

    // Keep the rotation maximum 90 degrees.
    if (m_rotationX < -90.0f) {
        m_rotationX = -90.0f;
    }
}

// Function to calculate turn using mouse
void Position::TurnMouse(XMFLOAT2 mouseMove) {
    float x = mouseMove.x;
    float y = mouseMove.y;
    float eps = 1e-10;
    while (fabs(x) > eps || fabs(y) > eps) {
        if (x > 0) {
            m_rotationY += 0.3;
            x -= 1;
        } else if (x < 0) {
            m_rotationY -= 0.3;
            x += 1;
        }
        if (y > 0) {
            m_rotationX += 0.3;
            y -= 1;
        } else if (y < 0) {
            m_rotationX -= 0.3;
            y += 1;
        }
    }
}
