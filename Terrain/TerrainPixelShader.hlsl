SamplerState SampleType;

Texture2D grassDiffuseTexture : register(t0);
Texture2D grassNormalTexture : register(t1);
Texture2D rockDiffuseTexture : register(t2);
Texture2D rockNormalTexture : register(t3);
Texture2D slopeDiffuseTexture : register(t4);
Texture2D slopeNormalTexture : register(t5);
Texture2D snowDiffuseTexture : register(t6);
Texture2D snowNormalTexture : register(t7);
Texture2D grass2 : register(t8);
Texture2D rock2 : register(t9);
Texture2D noise : register(t10);

cbuffer LightBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float4 specularColor;
    float3 lightDirection;
    float specularPower;
};

cbuffer scaleBuffer : register(b1)
{
    float grassScale;
    float rockScale;
    float slopeScale;
    float snowScale;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldPosition : WORLD;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex2 : TEXCOORD1;
    float pixelHeight: POSITION;
    float3 viewDirection: TEXCOORD2;
};

float4 blend(float4 texture1, float a1, float4 texture2, float a2)
{
    float depth = 0.2;
    float ma = max(texture1.a + a1, texture2.a + a2) - depth;

    float b1 = max(texture1.a + a1 - ma, 0);
    float b2 = max(texture2.a + a2 - ma, 0);

    return float4((texture1.rgb * b1 + texture2.rgb * b2) / (b1 + b2), 1.0f);
}

float4 triplanar_sampleGrass(Texture2D tex, float3 pos, float3 N) {
    float tighten = 0.4679f;
    float3 blending = saturate(abs(N) - tighten);
    float b = blending.x + blending.y + blending.z;
    blending /= float3(b, b, b);;
    float scale = 1 / grassScale;
    float4 x = tex.Sample(SampleType, pos.xy * scale);
    float4 y = tex.Sample(SampleType, pos.xz * scale);
    float4 z = tex.Sample(SampleType, pos.zy * scale);
    return x * blending.x + y * blending.y + z * blending.z;
}

float4 triplanar_sampleRock(Texture2D tex, float3 pos, float3 N) {
    float tighten = 0.4679f;
    float3 blending = saturate(abs(N) - tighten);
    float b = blending.x + blending.y + blending.z;
    blending /= float3(b, b, b);;
    float scale = 1.0f / rockScale;
    float4 x = tex.Sample(SampleType, pos.xy * scale);
    float4 y = tex.Sample(SampleType, pos.xz * scale);
    float4 z = tex.Sample(SampleType, pos.yz * scale);
    return x * blending.x + y * blending.y + z * blending.z;
}

float4 triplanar_sampleSlope(Texture2D tex, float3 pos, float3 N) {
    float tighten = 0.4679f;
    float3 blending = saturate(abs(N) - tighten);
    float b = blending.x + blending.y + blending.z;
    blending /= float3(b, b, b);;
    float scale = 1.0f / slopeScale;
    float4 x = tex.Sample(SampleType, pos.xy * scale);
    float4 y = tex.Sample(SampleType, pos.xz * scale);
    float4 z = tex.Sample(SampleType, pos.yz * scale);
    return x * blending.x + y * blending.y + z * blending.z;
}

float4 triplanar_sampleSnow(Texture2D tex, float3 pos, float3 N) {
    float tighten = 0.4679f;
    float3 blending = saturate(abs(N) - tighten);
    float b = blending.x + blending.y + blending.z;
    blending /= float3(b, b, b);;
    float scale = 1.0f / snowScale;
    float4 x = tex.Sample(SampleType, pos.xy * scale);
    float4 y = tex.Sample(SampleType, pos.xz * scale);
    float4 z = tex.Sample(SampleType, pos.yz * scale);
    return x * blending.x + y * blending.y + z * blending.z;
}


float4 main(PS_INPUT input) : SV_TARGET
{
    float slope;
    float3 lightDir;
    float4 textureColor;
    float4 bumpMap;
    float3 bumpNormal;
    float lightIntensity;
    float4 grassTexture, grassTexture2;
    float4 rockTexture, rockTexture2;
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

    // Get data from height map
    float alpha = noise.Sample(SampleType, input.tex2);

    // Setup the first material.
    textureColor = triplanar_sampleGrass(grassDiffuseTexture, input.worldPosition.xyz, input.normal);
    bumpMap = triplanar_sampleGrass(grassNormalTexture, input.worldPosition.xyz, input.normal);
    //textureColor = grassDiffuseTexture.Sample(SampleType, input.tex);
    //bumpMap = grassNormalTexture.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    diffuse = saturate(dot(bumpNormal, lightDir));
    reflect = normalize(2 * diffuse * bumpNormal - lightDir);
    specular = pow(saturate(dot(reflect, input.viewDirection)), specularPower);
    lightIntensity = ambientColor + diffuse + specular;
    grassTexture = saturate(textureColor * lightIntensity);

    //
    textureColor = triplanar_sampleGrass(grass2, input.worldPosition.xyz, input.normal);
    //textureColor = grass2.Sample(SampleType, input.tex);
    grassTexture2 = saturate(textureColor * lightIntensity);

    // Setup the second material.
    textureColor = triplanar_sampleRock(rockDiffuseTexture, input.worldPosition.xyz, input.normal);
    bumpMap = triplanar_sampleRock(rockNormalTexture, input.worldPosition.xyz, input.normal);
    //textureColor = rockDiffuseTexture.Sample(SampleType, input.tex);
    //bumpMap = rockNormalTexture.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    diffuse = saturate(dot(bumpNormal, lightDir));
    reflect = normalize(2 * diffuse * bumpNormal - lightDir);
    specular = pow(saturate(dot(reflect, input.viewDirection)), specularPower);
    lightIntensity = ambientColor + diffuse + specular;
    rockTexture = saturate(textureColor * lightIntensity);

    //
    textureColor = triplanar_sampleRock(rock2, input.worldPosition.xyz, input.normal);
    //textureColor = rock2.Sample(SampleType, input.tex);
    rockTexture2 = saturate(textureColor * lightIntensity);

    // Setup the third material.
    textureColor = triplanar_sampleSlope(slopeDiffuseTexture, input.worldPosition.xyz, input.normal);
    bumpMap = triplanar_sampleSlope(slopeNormalTexture, input.worldPosition.xyz, input.normal);
    //textureColor = slopeDiffuseTexture.Sample(SampleType, input.tex);
    //bumpMap = slopeNormalTexture.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    diffuse = saturate(dot(bumpNormal, lightDir));
    reflect = normalize(2 * diffuse * bumpNormal - lightDir);
    specular = pow(saturate(dot(reflect, input.viewDirection)), specularPower);
    lightIntensity = ambientColor + diffuse + specular;
    slopeTexture = saturate(textureColor * lightIntensity);

    // Setup the third material.
    textureColor = triplanar_sampleSnow(snowDiffuseTexture, input.worldPosition.xyz, input.normal);
    bumpMap = triplanar_sampleSnow(snowNormalTexture, input.worldPosition.xyz, input.normal);
    //textureColor = snowDiffuseTexture.Sample(SampleType, input.tex);
    //bumpMap = snowNormalTexture.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    bumpNormal = normalize(bumpNormal);
    diffuse = saturate(dot(bumpNormal, lightDir));
    reflect = normalize(2 * diffuse * bumpNormal - lightDir);
    specular = pow(saturate(dot(reflect, input.viewDirection)), 100000.0f);
    lightIntensity = float3(0.3f, 0.3f, 0.3f) + diffuse + specular;
    snowTexture = saturate(textureColor * lightIntensity);

    // Determine which material to use based on slope.
    float4 baseColor;
    grassTexture = lerp(grassTexture, grassTexture2, alpha.r);
    rockTexture = lerp(rockTexture, rockTexture2, alpha.r);
    if (input.pixelHeight < 200.0f)
    {
        baseColor = grassTexture;
    }
    else if (input.pixelHeight >= 200.0f && input.pixelHeight < 300.0f)
    {
        blendAmount = (300.f - input.pixelHeight) / (300.0f - 200.0f);
        baseColor = blend(snowTexture, 1 - blendAmount, grassTexture, blendAmount);
    }
    else if (input.pixelHeight > 300.0f)
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
