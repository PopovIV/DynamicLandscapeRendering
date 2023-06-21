#include "CBScene.h"

SamplerState SampleTypeNoMips : register(s0);

Texture2D HM : register(t0);

#define NUM_CONTROL_POINTS 3

cbuffer scaleBuffer : register(b3)
{
    int4 scales; // x - grass, y - rock, z - slope, w - snow
    float4 detailScale; // x - detail normal scale, y - height scale, z - max tess 
};

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

    //output.normal = patch[0].normal * uvwCoord.x + patch[1].normal * uvwCoord.y + patch[2].normal * uvwCoord.z;

    int H, W, dummy;
    HM.GetDimensions(0, H, W, dummy);
    float dX = 1.0f / W;
    float dZ = 1.0f / H;

    output.tangent = float3(2 * dX, 0, 0);
    output.tangent.y +=
        HM.SampleLevel(SampleTypeNoMips, float2(output.tex.x + dX, output.tex.y), 0.0f).x - HM.SampleLevel(SampleTypeNoMips, float2(output.tex.x - dX, output.tex.y), 0.0f).x;

    output.bitangent = float3(0, 0, 2 * dZ);
    output.bitangent.y += HM.SampleLevel(SampleTypeNoMips, float2(output.tex.x, output.tex.y + dZ), 0.0f).x -
        HM.SampleLevel(SampleTypeNoMips, float2(output.tex.x, output.tex.y - dZ), 0.0f).x;
    output.tangent = normalize(output.tangent);
    output.bitangent = normalize(output.bitangent);
    output.normal = cross(output.bitangent, output.tangent);

    float h = HM.SampleLevel(SampleTypeNoMips, output.tex, 0.0f).x;
    //vertexPos += h * output.normal;
    vertexPos.y = h * detailScale.y;

    output.position = mul(float4(vertexPos, 1.0f), viewProjectionMatrix);
    output.worldPosition = float4(vertexPos, 1.0f);
    //output.tangent = patch[0].tangent * uvwCoord.x + patch[1].tangent * uvwCoord.y + patch[2].tangent * uvwCoord.z;
    //output.bitangent = patch[0].bitangent * uvwCoord.x + patch[1].bitangent * uvwCoord.y + patch[2].bitangent * uvwCoord.z;


    output.pixelHeight = output.worldPosition.y;
    output.viewDirection = normalize(cameraPos - output.worldPosition.xyz);

    return output;
}
