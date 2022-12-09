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
Texture2D colorMap : register(t11);

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

float3 mysmoothstep(float3 p1, float3 p2, float t)
{
    t = max(min(t, 1), 0);
    return lerp(p1, p2, 3 * t * t - 2 * t * t * t);
}

float4 blend(float4 texture1, float a1, float4 texture2, float a2)
{
    float depth = 0.2;
    float ma = max(texture1.a + a1, texture2.a + a2) - depth;

    float b1 = max(texture1.a + a1 - ma, 0);
    float b2 = max(texture2.a + a2 - ma, 0);

    //return float4(mysmoothstep(texture1.rgb, texture2.rgb, c1 / (c1 + c2)), 1.0f);

    return float4((texture1.rgb * b1 + texture2.rgb * b2) / (b1 + b2), 1.0f);
}

float4 SampleTriplanar(Texture2D tex, float3 pos, float3 N, float input_scale) {
    float tighten = 0.4679f;
    float3 blending = saturate(abs(N) - tighten);
    float b = blending.x + blending.y + blending.z;
    blending /= float3(b, b, b);;
    blending = abs(blending);
    float scale = 1.0f / input_scale;
    float4 x = tex.Sample(SampleType, pos.yz * scale);
    float4 y = tex.Sample(SampleType, pos.xz * scale);
    float4 z = tex.Sample(SampleType, pos.xy * scale);
    return x * blending.x + y * blending.y + z * blending.z;
}

// TODO: FIX NORMALS
float4 SampleTriplanarNorm(Texture2D tex, float3 pos, float3 N, float input_scale) { 
    float tighten = 0.4679f;
    float3 blending = saturate(abs(N) - tighten);
    float b = blending.x + blending.y + blending.z;
    blending /= float3(b, b, b);
    blending = abs(blending);
    float scale = 1.0f / input_scale;
    float4 x = tex.Sample(SampleType, pos.yz * scale);
    float4 y = tex.Sample(SampleType, pos.xz * scale);
    float4 z = tex.Sample(SampleType, pos.xy * scale);
    float4 tmp = x * blending.x + y * blending.y + z * blending.z;
    return tmp;
}

float CalculateLightIntensity(float4 bumpMap, float3 normal, float3 tangent, float3 binormal, float3 viewDirection, float specPower) {
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    float3 bumpNormal = (bumpMap.x * tangent) + (bumpMap.y * binormal) + (bumpMap.z * normal);
    bumpNormal = normalize(bumpNormal);
    float4 diffuse = saturate(dot(bumpNormal, - lightDirection));
    float3 refl = normalize(2 * diffuse * bumpNormal + lightDirection);
    float3 specular = pow(saturate(dot(refl, viewDirection)), specPower);
    return ambientColor + diffuse + specular;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float slope;
    float4 textureColor;
    float4 bumpMap;
    float lightIntensity;
    float4 grassTexture, grassTexture2;
    float4 rockTexture, rockTexture2;
    float4 slopeTexture;
    float4 snowTexture;
    float blendAmount;
    float4 color;

    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    input.binormal = normalize(input.binormal);

    // Calculate the slope of this point.
    slope = 1.0f - input.normal.y;
    // Invert the light direction for calculations.

    // Get data from height map
    float4 alpha = noise.Sample(SampleType, input.tex2);

    // Setup the grass material
    textureColor = SampleTriplanar(grassDiffuseTexture, input.worldPosition.xyz, input.normal, grassScale);
    bumpMap = SampleTriplanarNorm(grassNormalTexture, input.worldPosition.xyz, input.normal, grassScale);
    lightIntensity = CalculateLightIntensity(bumpMap, input.normal, input.tangent, input.binormal, input.viewDirection, specularPower) - 0.1f;
    grassTexture = saturate(textureColor * lightIntensity);

    textureColor = SampleTriplanar(grass2, input.worldPosition.xyz, input.normal, grassScale);
    grassTexture2 = saturate(textureColor * lightIntensity);

    // Setup the rock material.
    textureColor = SampleTriplanar(rockDiffuseTexture, input.worldPosition.xyz, input.normal, rockScale);
    lightIntensity = CalculateLightIntensity(bumpMap, input.normal, input.tangent, input.binormal, input.viewDirection, specularPower);
    rockTexture = saturate(textureColor * lightIntensity);

    textureColor = SampleTriplanar(rock2, input.worldPosition.xyz, input.normal, rockScale);
    rockTexture2 = saturate(textureColor * lightIntensity);

    // Setup the slope material.
    textureColor = SampleTriplanar(slopeDiffuseTexture, input.worldPosition.xyz, input.normal, slopeScale);
    bumpMap = SampleTriplanarNorm(slopeNormalTexture, input.worldPosition.xyz, input.normal, slopeScale);
    lightIntensity = CalculateLightIntensity(bumpMap, input.normal, input.tangent, input.binormal, input.viewDirection, specularPower);
    slopeTexture = saturate(textureColor * lightIntensity);

    // Setup the snow material.
    textureColor = SampleTriplanar(snowDiffuseTexture, input.worldPosition.xyz, input.normal, snowScale);
    bumpMap = SampleTriplanarNorm(snowNormalTexture, input.worldPosition.xyz, input.normal, snowScale);
    lightIntensity = CalculateLightIntensity(bumpMap, input.normal, input.tangent, input.binormal, input.viewDirection, 100000.0f) - 0.5f;
    snowTexture = saturate(textureColor * lightIntensity);

    // Determine which material to use based on slope.
    float4 baseColor;
    grassTexture = lerp(grassTexture, grassTexture2, alpha.r);
    rockTexture = lerp(rockTexture, rockTexture2, alpha.r);
    if (input.pixelHeight < 200.0f) {
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

    // if use color map
    float4 c = colorMap.Sample(SampleType, input.tex2);
    float r = c.r;
    float g = c.g;
    float b = c.b;
    float sum = r + g + b;
    r /= sum;
    g /= sum;
    b /= sum;
    //return r * rockTexture + g * grassTexture + b * snowTexture;
    return color;
}
