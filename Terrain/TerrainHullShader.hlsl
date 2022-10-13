#define NUM_CONTROL_POINTS 3

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 cameraPos;
};

struct HS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct DS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeFactors[3]         : SV_TessFactor;
    float InsideFactor           : SV_InsideTessFactor;
};

float calculateEdgeFactor(float4 p0, float4 p1)
{
    p0 = mul(p0, worldMatrix);
    p1 = mul(p1, worldMatrix);
    float edgeLength = distance(p0, p1);
    float4 edgeCenter = (p0 + p1) * 0.5;
    float viewDistance = distance(edgeCenter, mul(float4(cameraPos, 1.0f), worldMatrix));
    return (edgeLength * 1080) / (10.0 * viewDistance);
}


HS_CONSTANT_DATA_OUTPUT constantsHullShader(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;

    // Calculate tessFactor
    float p0factor = calculateEdgeFactor(patch[1].position, patch[2].position);
    float p1factor = calculateEdgeFactor(patch[2].position, patch[0].position);
    float p2factor = calculateEdgeFactor(patch[0].position, patch[1].position);

    // Assign tessellation levels (constant for now)
    output.EdgeFactors[0] = p0factor;
    output.EdgeFactors[1] = p1factor;
    output.EdgeFactors[2] = p2factor;
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
    output.normal = patch[patchID].normal;
    output.tex = patch[patchID].tex;

    return output;
}
