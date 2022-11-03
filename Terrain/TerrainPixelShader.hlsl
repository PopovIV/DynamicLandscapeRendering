SamplerState SampleType;

Texture2D diffuseTexture1 : register(t0);
Texture2D normalTexture1 : register(t1);
Texture2D diffuseTexture2 : register(t2);
Texture2D normalTexture2 : register(t3);
Texture2D diffuseTexture3 : register(t4);
Texture2D normalTexture3 : register(t5);
Texture2D diffuseTexture4 : register(t6);
Texture2D normalTexture4 : register(t7);

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(0.0, 6.0, 0.0, 1.0);
}