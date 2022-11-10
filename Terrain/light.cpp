#include "light.h"

// Function to set ambient light
void Light::SetAmbientColor(float red, float green, float blue, float alpha) {
    m_ambientColor = XMFLOAT4(red, green, blue, alpha);
}

// Function to set diffuse light
void Light::SetDiffuseColor(float red, float green, float blue, float alpha) {
    m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
}

// Function to set specular light
void Light::SetSpecularColor(float red, float green, float blue, float alpha) {
    m_specularColor = XMFLOAT4(red, green, blue, alpha);
}

void Light::SetSpecularPower(float power) {
    m_specularPower = power;
}

// Function to set direction of light
void Light::SetDirection(float x, float y, float z) {
    m_direction = XMFLOAT3(x, y, z);
}

// Function to set position of light
void Light::SetPosition(float x, float y, float z) {
    m_position = XMFLOAT3(x, y, z);
}