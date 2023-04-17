#include "CBScene.h"

SamplerState SampleTypeNoMips : register(s0);

Texture2D HM : register(t0);

#define NUM_CONTROL_POINTS 3

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex: TEXCOORD;
    float4 worldPosition : WORLD;
    float3 normal : NORMAL;
    float3 tangent: TANGENT;
    float3 bitangent : BITANGENT;
    float pixelHeight : POSITION;
    float3 viewDirection: DIR;
    nointerpolation uint instanceId : INST_ID;
};

struct DS_INPUT
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent: TANGENT;
    float3 bitangent : BITANGENT;
    float2 tex: TEXCOORD;
    nointerpolation uint instanceId : INST_ID;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3]         : SV_TessFactor;
    float InsideTessFactor          : SV_InsideTessFactor;
};

[domain("tri")]
PS_INPUT main(HS_CONSTANT_DATA_OUTPUT input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<DS_INPUT, NUM_CONTROL_POINTS> patch)
{
    PS_INPUT output;
    float3 vertexPos;
    float2 vertexTex;

    float3 position, projection0, projection1, projection2, projectedPosition;
    position = patch[0].position * uvwCoord.x + patch[1].position * uvwCoord.y + patch[2].position * uvwCoord.z;

    output.instanceId = patch[0].instanceId;
    output.tex = patch[0].tex * uvwCoord.x + patch[1].tex * uvwCoord.y + patch[2].tex * uvwCoord.z;

    // Phong Tessellation
    projection0 = position - dot(position - patch[0].position, patch[0].normal) * patch[0].normal;
    projection1 = position - dot(position - patch[1].position, patch[1].normal) * patch[1].normal;
    projection2 = position - dot(position - patch[2].position, patch[2].normal) * patch[2].normal;
    projectedPosition = projection0 * uvwCoord.x + projection1 * uvwCoord.y + projection2 * uvwCoord.z;
    vertexPos = lerp(position, projectedPosition, 0.9);

    output.normal = patch[0].normal * uvwCoord.x + patch[1].normal * uvwCoord.y + patch[2].normal * uvwCoord.z;

    float h = HM.SampleLevel(SampleTypeNoMips, output.tex, 0.0f).x;
    vertexPos += output.normal * h;

    output.position = mul(float4(vertexPos, 1.0f), viewProjectionMatrix);
    output.worldPosition = float4(vertexPos, 1.0f);
    output.tangent = patch[0].tangent * uvwCoord.x + patch[1].tangent * uvwCoord.y + patch[2].tangent * uvwCoord.z;
    output.bitangent = patch[0].bitangent * uvwCoord.x + patch[1].bitangent * uvwCoord.y + patch[2].bitangent * uvwCoord.z;
    output.pixelHeight = output.worldPosition.y;
    output.viewDirection = normalize(cameraPos - output.worldPosition.xyz);

    return output;
}
