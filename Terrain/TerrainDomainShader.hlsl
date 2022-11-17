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

    float3 position, projection0, projection1, projection2,
        projectedPosition;

    position = patch[0].position * uvwCoord.x + patch[1].position * uvwCoord.y + patch[2].position * uvwCoord.z;
    projection0 = position - dot(position - patch[0].position, patch[0].normal) * patch[0].normal;
    projection1 = position - dot(position - patch[1].position, patch[1].normal) * patch[1].normal;
    projection2 = position - dot(position - patch[2].position, patch[2].normal) * patch[2].normal;
    projectedPosition = projection0 * uvwCoord.x + projection1 * uvwCoord.y + projection2 * uvwCoord.z;

    float3 p1 = patch[1].position - patch[0].position;
    float3 p2 = patch[2].position - patch[0].position;

    float3 p1crossp2 = float3(p1.y * p2.z - p2.y * p1.z, -(p1.x * p2.z - p2.x * p1.z), p1.x * p2.y - p2.x * p1.y);
    float area = sqrt(dot(p1crossp2, p1crossp2));
    float coef = max(int(area / 100), 1);

    // Find new vertex pos
    vertexPos = lerp(position, projectedPosition, 0.9);
    vertexTex = patch[0].tex * uvwCoord.x + patch[1].tex * uvwCoord.y + patch[2].tex * uvwCoord.z;
    vertexNormal = patch[0].normal * uvwCoord.x + patch[1].normal * uvwCoord.y + patch[2].normal * uvwCoord.z;
    vertexTangent = patch[0].tangent * uvwCoord.x + patch[1].tangent * uvwCoord.y + patch[2].tangent * uvwCoord.z;
    vertexBinormal = patch[0].binormal * uvwCoord.x + patch[1].binormal * uvwCoord.y + patch[2].binormal * uvwCoord.z;
    // To new coords
    output.position = mul(float4(vertexPos, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.tex = vertexTex * coef;
    output.position = mul(output.position, projectionMatrix);
    output.normal = normalize(mul(vertexNormal, (float3x3)worldMatrix));
    output.tangent = normalize(mul(vertexTangent, (float3x3)worldMatrix));
    output.binormal = normalize(mul(vertexBinormal, (float3x3)worldMatrix));
    output.pixelHeight = mul(float4(vertexPos, 1.0f), worldMatrix).y;
    output.tex2 = patch[0].tex2 * uvwCoord.x + patch[1].tex2 * uvwCoord.y + patch[2].tex2 * uvwCoord.z;

    float4 worldPosition;
    worldPosition = mul(float4(vertexPos, 1.0f), worldMatrix);
    output.viewDirection = normalize(cameraPos - worldPosition.xyz);

    return output;
}