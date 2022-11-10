//--------------------------------------------------------------------------------------
// Light class handles all lightning stuff
//--------------------------------------------------------------------------------------
#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <directxmath.h>
using namespace DirectX;

class Light {
public:
    // constructors
    Light() {};
    Light(const Light& other) {};
    // destructors
    ~Light() {};

    // Function to set ambient light
    void SetAmbientColor(float red, float green, float blue, float alpha);
    // Function to set diffuse light
    void SetDiffuseColor(float red, float green, float blue, float alpha);
    // Function to set specular light
    void SetSpecularColor(float red, float green, float blue, float alpha);
    // Function to set specular power
    void SetSpecularPower(float power);
    // Function to set direction of light
    void SetDirection(float x, float y, float z);
    // Function to set position of light
    void SetPosition(float x, float y, float z);

    // Function to get ambient light
    XMFLOAT4 GetAmbientColor() { return m_ambientColor; };
    // Function to get diffuse light
    XMFLOAT4 GetDiffuseColor() { return m_diffuseColor; };
    // Function to get specular light
    XMFLOAT4 GetSpecularColor() { return m_specularColor; };
    // Function to get direction of light
    XMFLOAT3 GetDirection() { return m_direction; };
    // Function to get position of light
    XMFLOAT3 GetPosition() { return m_position; };
    // Function to get power of specular light
    float GetSpecularPower() { return m_specularPower; };

private:
    XMFLOAT4 m_ambientColor;
    XMFLOAT4 m_diffuseColor;
    XMFLOAT4 m_specularColor;
    XMFLOAT3 m_direction;
    XMFLOAT3 m_position;
    float m_specularPower;
};

#endif