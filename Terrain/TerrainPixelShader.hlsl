SamplerState SampleType;

Texture2D grassDiffuseTexture : register(t0);
Texture2D grassNormalTexture : register(t1);
Texture2D grassRoughTexture : register(t2);
Texture2D grassAoTexture : register(t3);
Texture2D rockDiffuseTexture : register(t4);
Texture2D rockNormalTexture : register(t5);
Texture2D rockRoughTexture : register(t6);
Texture2D rockAoTexture : register(t7);
Texture2D snowDiffuseTexture : register(t8);
Texture2D snowNormalTexture : register(t9);
Texture2D snowRoughTexture : register(t10);
Texture2D snowAoTexture : register(t11);

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
    return  BRDF * diffuseColor * max(dot(L, N), 0.0f) * ao * 4;
}

float4 CalculateColor(Texture2D diffuseTexture, Texture2D normalTexture, Texture2D roughTexture, Texture2D aoTexture,
    float2 tex, float3 normal, float3 tangent, float3 binormal, float3 L, float3 V) {
    float4 albedo = diffuseTexture.Sample(SampleType, tex);
    float4 bumpMap = normalTexture.Sample(SampleType, tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    float rough = roughTexture.Sample(SampleType, tex).r;
    float ao = aoTexture.Sample(SampleType, tex).r;
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
    float4 grassTexture = CalculateColor(grassDiffuseTexture, grassNormalTexture, grassRoughTexture, grassAoTexture, input.tex, input.normal, input.tangent, input.binormal, L, V);
    
    //resColor = resColor / (resColor + float3(1.0f, 1.0f, 1.0f));
    //resColor = pow(resColor, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

    // Setup the rock material
    float4 rockTexture = CalculateColor(rockDiffuseTexture, rockNormalTexture, rockRoughTexture, rockAoTexture, input.tex, input.normal, input.tangent, input.binormal, L, V);

    // Setup the grass material
    float4 snowTexture = CalculateColor(snowDiffuseTexture, snowNormalTexture, snowRoughTexture, snowAoTexture, input.tex, input.normal, input.tangent, input.binormal, L, V);

    // Determine which material to use based on slope.
    float4 baseColor;
    if (input.pixelHeight < 250.0f) {
        blendAmount = (250.0f - input.pixelHeight) / 100.0f;
        baseColor = lerp(grassTexture, rockTexture, 1 - blendAmount);
    }
    else if (input.pixelHeight >= 250.0f && input.pixelHeight < 300.0f) {
        blendAmount = (input.pixelHeight - 250.0f) / (300.0f - 250.0f);
        baseColor = blend(rockTexture, 1 - blendAmount, snowTexture, blendAmount);
    }
    else if (input.pixelHeight >= 300.0f) {
        baseColor = snowTexture;
    }

    return baseColor;
}
