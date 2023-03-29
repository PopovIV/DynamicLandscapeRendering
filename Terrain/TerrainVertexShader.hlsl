#include "CBScene.h"

SamplerState SampleTypeNoMips : register(s0);

Texture2D HM : register(t0);

struct VS_INPUT
{
    float4 position : POSITION;
    uint instanceId : SV_InstanceID;
};

struct HS_INPUT
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent: TANGENT;
    float3 bitangent : BITANGENT;
    float2 tex : TEXCOORD;
    uint instanceId : INST_ID;
};

HS_INPUT main(VS_INPUT input)
{
    HS_INPUT output;

    output.instanceId = input.instanceId;

    output.position = mul(float4(input.position.xyz, 1.0), geomBuffer[input.instanceId].worldMatrix);
    output.tex = float2(1.0f * output.position.x / (TERRAIN_CHUNK_WIDTH * TERRAIN_CHUNK_COUNT_WIDTH), 1.0f * (TERRAIN_CHUNK_HEIGHT * TERRAIN_CHUNK_COUNT_HEIGHT - output.position.z) / (TERRAIN_CHUNK_HEIGHT * TERRAIN_CHUNK_COUNT_HEIGHT));
    output.position.y += HM.SampleLevel(SampleTypeNoMips, output.tex, 0.0f).x * 400.0;
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

    return output;
}
