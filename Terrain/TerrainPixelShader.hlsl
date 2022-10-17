SamplerState SampleType;

Texture2D diffuseTexture1 : register(t0);
Texture2D normalTexture1 : register(t1);
Texture2D normalTexture2 : register(t2);

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
    float4 color : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float slope;
    float3 lightDir;
    float4 textureColor1;
    float4 textureColor2;
    float4 bumpMap;
    float3 bumpNormal;
    float lightIntensity;
    float4 material1;
    float4 material2;
    float blendAmount;
    float4 color;

    // Calculate the slope of this point.
    slope = 1.0f - input.normal.y;
    // Invert the light direction for calculations.
    lightDir = -lightDirection;
    // Setup the first material.
    textureColor1 = diffuseTexture1.Sample(SampleType, input.tex);
    bumpMap = normalTexture1.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    lightIntensity = saturate(dot(bumpNormal, lightDir));
    material1 = saturate(textureColor1 * lightIntensity);

    // Setup the second material.
    textureColor2 = float4(1.0f, 1.0f, 1.0f, 1.0f);  // Snow color.
    bumpMap = normalTexture2.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    lightIntensity = saturate(dot(bumpNormal, lightDir));
    material2 = saturate(textureColor2 * lightIntensity);

    // Determine which material to use based on slope.
    if (slope < 0.1) {
        blendAmount = slope / 0.2f;
        color = lerp(material2, material1, blendAmount);
    }
    if (slope >= 0.2)
        color = material1;

    return color;
}
