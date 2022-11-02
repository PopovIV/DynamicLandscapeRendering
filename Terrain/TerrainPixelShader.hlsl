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
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float pixelHeight: POSITION;
};

float4 blend(float4 texture1, float a1, float4 texture2, float a2)
{
    float depth = 0.2;
    float ma = max(texture1.a + a1, texture2.a + a2) - depth;

    float b1 = max(texture1.a + a1 - ma, 0);
    float b2 = max(texture2.a + a2 - ma, 0);

    return float4((texture1.rgb * b1 + texture2.rgb * b2) / (b1 + b2), 1.0f);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float slope;
    float3 lightDir;
    float4 textureColor1;
    float4 textureColor2;
    float4 textureColor3;
    float4 textureColor4;
    float4 bumpMap;
    float3 bumpNormal;
    float lightIntensity;
    float4 material1;
    float4 material2;
    float4 material3;
    float4 material4;
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
    textureColor2 = diffuseTexture2.Sample(SampleType, input.tex);
    bumpMap = normalTexture2.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    lightIntensity = saturate(dot(bumpNormal, lightDir));
    material2 = saturate(textureColor2 * lightIntensity);

    // Setup the third material.
    textureColor3 = diffuseTexture3.Sample(SampleType, input.tex);
    bumpMap = normalTexture3.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    lightIntensity = saturate(dot(bumpNormal, lightDir));
    material3 = saturate(textureColor3 * lightIntensity);

    // Setup the third material.
    textureColor4 = diffuseTexture4.Sample(SampleType, input.tex);
    bumpMap = normalTexture4.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    lightIntensity = saturate(dot(bumpNormal, lightDir));
    material4 = saturate(textureColor4 * lightIntensity);

    // Determine which material to use based on slope.
    float4 baseColor;

    //if (input.pixelHeight < 125.0f)
    //{
    //    baseColor = material1;
    //}
    //else if (input.pixelHeight >= 125.0f && input.pixelHeight < 250.0f)
    //{
    //    blendAmount = (250.f - input.pixelHeight) / (250.0f - 125.0f);
    //    baseColor = lerp(material2, material1, blendAmount);
        //baseColor = blend(material2, 1 - blendAmount, material1, blendAmount);
    //}
    //else if (input.pixelHeight > 250.0f)
    //{
    //    baseColor = material2;
    //}
    blendAmount = (380.f - input.pixelHeight) / (380.0f);
    //baseColor = lerp(material2, material1, blendAmount);
    baseColor = blend(material2, 1 - blendAmount, material1, blendAmount);
    if (slope < 0.4f)
    {
        blendAmount = slope / 0.4f;
        //color = lerp(baseColor, material3, blendAmount);
        color = blend(baseColor, 1 - blendAmount, material3, blendAmount);
    }
    else if (slope >= 0.4 && slope < 0.7) {
        blendAmount = (slope - 0.4f) * (1.0f / (0.7f - 0.4f));
        //color = lerp(material3, material4, blendAmount);
        color = blend(material3, 1 - blendAmount, material4, blendAmount);
    }
    else if (slope >= 0.7) {
        color = material4;
    }

    return color;
}
