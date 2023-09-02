#include "CBScene.h"

#define NUM_CONTROL_POINTS 3

cbuffer scaleBuffer : register(b3)
{
    int4 scales; // x - grass, y - rock, z - slope, w - snow
    float4 detailScale; // x - detail normal scale, y - height scale, z - max tess 
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

struct DS_INPUT
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent: TANGENT;
    float3 bitangent : BITANGENT;
    float2 tex : TEXCOORD;
    nointerpolation uint instanceId : INST_ID;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeFactors[3]         : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
};

float calculateEdgeFactor(float4 p0, float4 p1, matrix worldMatrix) {
    float edgeLength = distance(p0, p1);
    float4 edgeCenter = (p0 + p1) * 0.5;
    float viewDistance = distance(edgeCenter, cameraPos);
    return (edgeLength * 100) / (viewDistance);
}

HS_CONSTANT_DATA_OUTPUT constantsHullShader(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;

    // Calculate tessFactor
    float p0factor = calculateEdgeFactor(patch[1].position, patch[2].position, geomBuffer[patch[0].instanceId].worldMatrix);
    float p1factor = calculateEdgeFactor(patch[2].position, patch[0].position, geomBuffer[patch[0].instanceId].worldMatrix);
    float p2factor = calculateEdgeFactor(patch[0].position, patch[1].position, geomBuffer[patch[0].instanceId].worldMatrix);

    // Assign tessellation levels (constant for now)
    output.EdgeFactors[0] = clamp(p0factor, 1.0f, (float)detailScale.z);
    output.EdgeFactors[1] = clamp(p1factor, 1.0f, (float)detailScale.z);
    output.EdgeFactors[2] = clamp(p2factor, 1.0f, (float)detailScale.z);
    output.InsideFactor = (output.EdgeFactors[0] + output.EdgeFactors[1] + output.EdgeFactors[2]) / 3.0f;
 
    return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("constantsHullShader")]
[maxtessfactor(64.0)]
DS_INPUT main(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_OutputControlPointID)
{
    DS_INPUT output;

    // Copy inputs to outputs
    output.position = patch[patchID].position;
    output.tex = patch[patchID].tex;
    output.normal = patch[patchID].normal;
    output.bitangent = patch[patchID].bitangent;
    output.tangent = patch[patchID].tangent;
    output.instanceId = patch[patchID].instanceId;

    return output;
}
