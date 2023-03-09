#define NUM_CONTROL_POINTS 3

SamplerState SampleTypeNoMips : register(s0);
Texture2D HeightMap : register(t0);

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 cameraPos;
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
    float pixelHeight : POSITION;
    float3 viewDirection: TEXCOORD2;
};

struct DS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex2 : TEXCOORD1;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3]         : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
    float3 bezierPoints[10] : BEZIERPOS;
};

[domain("tri")]
PS_INPUT main(HS_CONSTANT_DATA_OUTPUT input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<DS_INPUT, NUM_CONTROL_POINTS> patch)
{
    PS_INPUT output;
    float3 vertexPos;
    float2 vertexTex;
    float3 vertexNormal;
    float3 vertexTangent;
    float3 vertexBinormal;

    float3 position, projection0, projection1, projection2, projectedPosition;
    position = patch[0].position * uvwCoord.x + patch[1].position * uvwCoord.y + patch[2].position * uvwCoord.z;

    // Phong Tessellation
    projection0 = position - dot(position - patch[0].position, patch[0].normal) * patch[0].normal;
    projection1 = position - dot(position - patch[1].position, patch[1].normal) * patch[1].normal;
    projection2 = position - dot(position - patch[2].position, patch[2].normal) * patch[2].normal;
    projectedPosition = projection0 * uvwCoord.x + projection1 * uvwCoord.y + projection2 * uvwCoord.z;
    vertexPos = lerp(position, projectedPosition, 0.9);

    // Bezier position
    //projectedPosition =
    //    patch[0].position * (uvwCoord.x * uvwCoord.x * uvwCoord.x) +
    //    patch[1].position * (uvwCoord.y * uvwCoord.y * uvwCoord.y) +
    //    patch[2].position * (uvwCoord.z * uvwCoord.z * uvwCoord.z) +
    //    input.bezierPoints[0] * (3 * uvwCoord.x * uvwCoord.x * uvwCoord.y) +
    //    input.bezierPoints[1] * (3 * uvwCoord.y * uvwCoord.y * uvwCoord.x) +
    //    input.bezierPoints[2] * (3 * uvwCoord.y * uvwCoord.y * uvwCoord.z) +
    //    input.bezierPoints[3] * (3 * uvwCoord.z * uvwCoord.z * uvwCoord.y) +
    //    input.bezierPoints[4] * (3 * uvwCoord.z * uvwCoord.z * uvwCoord.x) +
    //    input.bezierPoints[5] * (3 * uvwCoord.x * uvwCoord.x * uvwCoord.z) +
    //    input.bezierPoints[6] * (6 * uvwCoord.x * uvwCoord.y * uvwCoord.z);
    //vertexPos = lerp(position, projectedPosition, 0.9);

    //float3 normal, projectedNormal;
    //normal = patch[0].normal * uvwCoord.x + patch[1].normal * uvwCoord.y + patch[2].normal * uvwCoord.z;
    //projectedNormal =
    //    patch[0].normal * (uvwCoord.x * uvwCoord.x) +
    //    patch[1].normal * (uvwCoord.y * uvwCoord.y) +
    //    patch[2].normal * (uvwCoord.z * uvwCoord.z) +
    //    input.bezierPoints[7] * (2 * uvwCoord.x * uvwCoord.y) +
    //    input.bezierPoints[8] * (2 * uvwCoord.y * uvwCoord.z) +
    //    input.bezierPoints[9] * (2 * uvwCoord.z * uvwCoord.x);
    //vertexNormal = normalize(lerp(normal, projectedNormal, 0.9));
    //vertexTangent = patch[0].tangent * uvwCoord.x + patch[1].tangent * uvwCoord.y + patch[2].tangent * uvwCoord.z;
    //vertexBinormal = cross(vertexNormal, vertexTangent);
    //vertexTangent = normalize(cross(vertexBinormal, vertexNormal));;

    vertexNormal = patch[0].normal * uvwCoord.x + patch[1].normal * uvwCoord.y + patch[2].normal * uvwCoord.z;
    vertexBinormal = patch[0].binormal * uvwCoord.x + patch[1].binormal * uvwCoord.y + patch[2].binormal * uvwCoord.z;
    vertexTangent = patch[0].tangent * uvwCoord.x + patch[1].tangent * uvwCoord.y + patch[2].tangent * uvwCoord.z;

    vertexTex = patch[0].tex * uvwCoord.x + patch[1].tex * uvwCoord.y + patch[2].tex * uvwCoord.z;
    // To new coords
    float h = HeightMap.SampleLevel(SampleTypeNoMips, vertexPos / 1920, 0.0f).x * 1.0;
    vertexPos += vertexNormal * h;

    float presition = 1024.0;

    vertexPos = float3(int3(vertexPos * presition) / presition);

    output.position = mul(float4(vertexPos, 1.0), worldMatrix);
    output.worldPosition = output.position;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.tex = vertexTex;
    output.normal = normalize(mul(vertexNormal, (float3x3)worldMatrix));
    output.tangent = normalize(mul(vertexTangent, (float3x3)worldMatrix));
    output.binormal = normalize(mul(vertexBinormal, (float3x3)worldMatrix));
    output.pixelHeight = mul(float4(vertexPos, 1.0f), worldMatrix).y;
    output.tex2 = patch[0].tex2 * uvwCoord.x + patch[1].tex2 * uvwCoord.y + patch[2].tex2 * uvwCoord.z;

    output.viewDirection = normalize(cameraPos - output.worldPosition.xyz);

    return output;
}
