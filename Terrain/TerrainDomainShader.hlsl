#define NUM_CONTROL_POINTS 3

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
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float pixelHeight : POSITION;
};

struct DS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3]         : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
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

    // Find new vertex pos
    vertexPos = patch[0].position * uvwCoord.x + patch[1].position * uvwCoord.y + patch[2].position * uvwCoord.z;
    vertexTex = patch[0].tex * uvwCoord.x + patch[1].tex * uvwCoord.y + patch[2].tex * uvwCoord.z;
    vertexNormal = patch[0].normal * uvwCoord.x + patch[1].normal * uvwCoord.y + patch[2].normal * uvwCoord.z;
    vertexTangent = patch[0].tangent * uvwCoord.x + patch[1].tangent * uvwCoord.y + patch[2].tangent * uvwCoord.z;
    vertexBinormal = patch[0].binormal * uvwCoord.x + patch[1].binormal * uvwCoord.y + patch[2].binormal * uvwCoord.z;
    // To new coords
    output.position = mul(float4(vertexPos, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.tex = vertexTex;
    output.position = mul(output.position, projectionMatrix);
    output.normal = normalize(mul(vertexNormal, (float3x3)worldMatrix));
    output.tangent = normalize(mul(vertexTangent, (float3x3)worldMatrix));
    output.binormal = normalize(mul(vertexBinormal, (float3x3)worldMatrix));
    output.pixelHeight = mul(float4(vertexPos, 1.0f), worldMatrix).y;

    return output;
}