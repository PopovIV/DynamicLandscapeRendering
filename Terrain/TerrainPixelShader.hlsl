SamplerState SampleType;

Texture2D grassDiffuseTexture : register(t0);
Texture2D grassNormalTexture : register(t1);
Texture2D rockDiffuseTexture : register(t2);
Texture2D rockNormalTexture : register(t3);
Texture2D slopeDiffuseTexture : register(t4);
Texture2D slopeNormalTexture : register(t5);
Texture2D snowDiffuseTexture : register(t6);
Texture2D snowNormalTexture : register(t7);

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float4 specularColor;
    float specularPower;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float pixelHeight: POSITION;
    float3 viewDirection: TEXCOORD1;
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
    float4 textureColor;
    float4 bumpMap;
    float3 bumpNormal;
    float lightIntensity;
    float4 grassTexture;
    float4 rockTexture;
    float4 slopeTexture;
    float4 snowTexture;
    float blendAmount;
    float4 color;
    float4 diffuse;
    float4 specular;
    float3 reflect;

    // Calculate the slope of this point.
    slope = 1.0f - input.normal.y;
    // Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Setup the first material.
    textureColor = grassDiffuseTexture.Sample(SampleType, input.tex);
    bumpMap = grassNormalTexture.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    diffuse = saturate(dot(bumpNormal, lightDir));
    reflect = normalize(2 * diffuse * bumpNormal - lightDir);
    specular = pow(saturate(dot(reflect, input.viewDirection)), specularPower);
    lightIntensity = ambientColor + diffuse + specular;
    grassTexture = saturate(textureColor * lightIntensity);

    // Setup the second material.
    textureColor = rockDiffuseTexture.Sample(SampleType, input.tex);
    bumpMap = rockNormalTexture.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    diffuse = saturate(dot(bumpNormal, lightDir));
    reflect = normalize(2 * diffuse * bumpNormal - lightDir);
    specular = pow(saturate(dot(reflect, input.viewDirection)), specularPower);
    lightIntensity = ambientColor + diffuse + specular;
    rockTexture = saturate(textureColor * lightIntensity);

    // Setup the third material.
    textureColor = slopeDiffuseTexture.Sample(SampleType, input.tex);
    bumpMap = slopeNormalTexture.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    diffuse = saturate(dot(bumpNormal, lightDir));
    reflect = normalize(2 * diffuse * bumpNormal - lightDir);
    specular = pow(saturate(dot(reflect, input.viewDirection)), specularPower);
    lightIntensity = ambientColor + diffuse + specular;
    slopeTexture = saturate(textureColor * lightIntensity);

    // Setup the third material.
    textureColor = snowDiffuseTexture.Sample(SampleType, input.tex);
    bumpMap = snowNormalTexture.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    diffuse = saturate(dot(bumpNormal, lightDir));
    reflect = normalize(2 * diffuse * bumpNormal - lightDir);
    specular = pow(saturate(dot(reflect, input.viewDirection)), specularPower);
    lightIntensity = ambientColor + diffuse + specular;
    snowTexture = saturate(textureColor * lightIntensity);

    // Determine which material to use based on slope.
    float4 baseColor;

    if (input.pixelHeight < 100.0f)
    {
        baseColor = grassTexture;
    }
    else if (input.pixelHeight >= 100.0f && input.pixelHeight < 250.0f)
    {
        blendAmount = (250.f - input.pixelHeight) / (250.0f - 100.0f);
        baseColor = blend(snowTexture, 1 - blendAmount, grassTexture, blendAmount);
    }
    else if (input.pixelHeight > 250.0f)
    {
        baseColor = snowTexture;
    }

    if (slope < 0.4f)
    {
        blendAmount = slope / 0.4f;
        color = blend(baseColor, 1 - blendAmount, rockTexture, blendAmount);
    }
    else if (slope >= 0.4 && slope < 0.7) {
        blendAmount = (slope - 0.4f) * (1.0f / (0.7f - 0.4f));
        color = blend(rockTexture, 1 - blendAmount, slopeTexture, blendAmount);
    }
    else if (slope >= 0.7) {
        color = slopeTexture;
    }

    return color;
}
