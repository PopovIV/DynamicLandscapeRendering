SamplerState SampleType : register(s0);
SamplerState SampleTypeNoMips : register(s1);

Texture2D grassDiffuseTexture : register(t0);
Texture2D grassNormalTexture : register(t1);
Texture2D grassRoughTexture : register(t2);
Texture2D grassAoTexture : register(t3);
Texture2D grassDiffuse2Texture : register(t4);
Texture2D grassNormal2Texture : register(t5);
Texture2D grassRough2Texture : register(t6);
Texture2D grassAo2Texture : register(t7);
Texture2D rockDiffuseTexture : register(t8);
Texture2D rockNormalTexture : register(t9);
Texture2D rockRoughTexture : register(t10);
Texture2D rockAoTexture : register(t11);
Texture2D slopeDiffuseTexture : register(t12);
Texture2D slopeNormalTexture : register(t13);
Texture2D slopeRoughTexture : register(t14);
Texture2D slopeAoTexture : register(t15);
Texture2D snowDiffuseTexture : register(t16);
Texture2D snowNormalTexture : register(t17);
Texture2D snowRoughTexture : register(t18);
Texture2D snowAoTexture : register(t19);
Texture2D detailNormalMap : register(t20);
Texture2D noise : register(t21);

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
    float detailScale;
    float3 padding;
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

float4 SampleTriplanar(Texture2D tex, float3 pos, float3 N, float input_scale, SamplerState Sampler) {
    float tighten = 0.4679f;
    float3 blending = saturate(abs(N) - tighten);
    float b = blending.x + blending.y + blending.z;
    blending /= float3(b, b, b);;
    blending = abs(blending);
    float scale = 1.0f / input_scale;
    float4 x = tex.Sample(Sampler, pos.yz * scale);
    float4 y = tex.Sample(Sampler, pos.xz * scale);
    float4 z = tex.Sample(Sampler, pos.xy * scale);
    return x * blending.x + y * blending.y + z * blending.z;
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

static const float PI = 3.14159265359;

float3 F(float3 F0, float3 V, float3 H) {
    return F0 + (1 - F0) * pow(1 - dot(V, H), 5.0f);
}

float3 D(float alpha, float3 N, float3 H) {
    float numerator = alpha * alpha;

    float NdotH = max(dot(N, H), 0.0f);
    float denominator = PI * pow(NdotH * NdotH * (alpha * alpha - 1) + 1, 2.0f);
    denominator = max(denominator, 0.00001f);
    return numerator / denominator;
}

float3 G1(float alpha, float3 N, float3 X) {
    float numerator = max(dot(N, X), 0.0f);
    float k = alpha / 2.0f;
    float denominator = max(dot(N, X), 0.0f) * (1.0f - k) + k;
    denominator = max(denominator, 0.00001f);
    return numerator / denominator;
}

float3 G(float alpha, float3 N, float3 V, float3 L) {
    return G1(alpha, N, V) * G1(alpha, N, L);
}

float3 CalculatePBR(float3 N, float3 L, float3 V, float3 H, float alpha, float3 albedo, float3 F0, float ao) {
    float3 Ks = F(F0, V, H);
    float3 Kd = float3(1.0f, 1.0f, 1.0f) - Ks;
    float3 lambert = albedo / PI;
    
    float3 cookTorranceNumerator = D(alpha, N, H) * G(alpha, N, V, L) * Ks;
    float cookTorranceDenominator = 4.0f * max(dot(V, N), 0.0f) * max(dot(L, N), 0.0f);
    cookTorranceDenominator = max(cookTorranceDenominator, 0.00001f);
    float3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

    float3 BRDF = Kd * lambert + cookTorrance;
    return  BRDF * diffuseColor * max(dot(L, N), 0.2f) * ao;//4
}

float4 CalculateColor(Texture2D diffuseTexture, Texture2D normalTexture, Texture2D roughTexture, Texture2D aoTexture,
    float3 pos, float3 normal, float3 tangent, float3 binormal, float3 L, float3 V, float scale) {
    float4 albedo = SampleTriplanar(diffuseTexture, pos, normal, scale, SampleType);
    float4 bumpMap = SampleTriplanar(normalTexture, pos, normal, scale, SampleType) * 2.0f - 1.0f;
    float4 detailBumpMap = detailNormalMap.Sample(SampleType, pos.yz * detailScale) * 2.0f - 1.0f;//SampleTriplanar(detailNormalMap, pos, normal, 1 / detailScale, SampleType) * 2.0f - 1.0f;
    bumpMap.x += detailBumpMap.x;
    bumpMap.y += detailBumpMap.y;
    float rough = roughTexture.Sample(SampleType, pos.yz * scale);// SampleTriplanar(roughTexture, pos, normal, scale, SampleType).r;
    float ao = aoTexture.Sample(SampleType, pos.yz * scale);// SampleTriplanar(aoTexture, pos, normal, scale, SampleType).r;
    float3 N = (bumpMap.x * tangent) + (bumpMap.y * binormal) + (bumpMap.z * normal);
    N = normalize(N);
    float3 H = normalize(V + L);
    return float4(CalculatePBR(N, L, V, H, rough, albedo.xyz, float3(0.04f, 0.04f, 0.04f), ao), 1.0f);
}


float4 main(PS_INPUT input) : SV_TARGET
{
    float blendAmount;
    float4 color;
    float3 L = -normalize(lightDirection);
    float3 V = normalize(input.viewDirection);

    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    input.binormal = normalize(input.binormal);

    // Setup the grass material
    float4 grassTexture2 = CalculateColor(grassDiffuseTexture, grassNormalTexture, grassRoughTexture, grassAoTexture, input.worldPosition.xyz, input.normal, input.tangent, input.binormal, L, V, grassScale / 2);// /2
    float4 grassTexture;
    grassTexture = CalculateColor(grassDiffuse2Texture, grassNormal2Texture, grassRough2Texture, grassAo2Texture, input.worldPosition.xyz, input.normal, input.tangent, input.binormal, L, V, grassScale * 1.5);//*2
    float alpha = noise.Sample(SampleType, input.tex2).r;
    grassTexture = (alpha * grassTexture) + ((1.0 - alpha) * grassTexture2);

    // Setup the rock material
    float4 rockTexture = CalculateColor(rockDiffuseTexture, rockNormalTexture, rockRoughTexture, rockAoTexture, input.worldPosition.xyz, input.normal, input.tangent, input.binormal, L, V, rockScale);

    // Setup the slope material
    float4 slopeTexture = CalculateColor(slopeDiffuseTexture, slopeNormalTexture, slopeRoughTexture, slopeAoTexture, input.worldPosition.xyz, input.normal, input.tangent, input.binormal, L, V, slopeScale);

    // Setup the grass material
    float4 snowTexture = CalculateColor(snowDiffuseTexture, snowNormalTexture, snowRoughTexture, snowAoTexture, input.worldPosition.xyz, input.normal, input.tangent, input.binormal, L, V, snowScale);

    // Determine which material to use based on slope.
    float4 baseColor;
    if (input.pixelHeight < 200.0f) {
        baseColor = grassTexture;
    }
    else if (input.pixelHeight >= 200.0f && input.pixelHeight < 300.0f) {
        blendAmount = (300 - input.pixelHeight) / (300.0f - 200.0f);
        baseColor = blend(snowTexture, 1 - blendAmount, grassTexture, blendAmount);
    }
    else if (input.pixelHeight >= 300.0f) {
        baseColor = snowTexture;
    }
    
    float slope = 1.0f - input.normal.y;
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
