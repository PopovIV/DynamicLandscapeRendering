SamplerState SampleType;

Texture2D shaderTexture : register(t0);
Texture2D normalTexture : register(t1);

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float4 bumpMap;
    float3 bumpNormal;
    float lightIntensity;
    float4 color;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    // Invert the light direction for calculations.
    lightDir = -lightDirection;
    // Calculate the amount of light on this pixel using the normal map.
    bumpMap = normalTexture.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    lightIntensity = saturate(dot(bumpNormal, lightDir));
    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = saturate(diffuseColor * lightIntensity);
    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
    color = color * textureColor;
    return color;
}
