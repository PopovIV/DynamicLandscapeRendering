#define NUM_CONTROL_POINTS 4

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 cameraPos;
};

struct HS_INPUT
{
    float4 position : POSITION;
};

struct DS_INPUT
{
    float4 position : POSITION;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeFactors[4]         : SV_TessFactor;
    float InsideFactor[2] : SV_InsideTessFactor;
};

HS_CONSTANT_DATA_OUTPUT constantsHullShader(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;

    // Assign tessellation levels (constant for now)
    output.EdgeFactors[0] = 4;
    output.EdgeFactors[1] = 4;
    output.EdgeFactors[2] = 4;
    output.EdgeFactors[3] = 4;
    output.InsideFactor[0] = 4;
    output.InsideFactor[1] = 4;

    return output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("constantsHullShader")]
[maxtessfactor(1.0)]
DS_INPUT main(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_OutputControlPointID)
{
    DS_INPUT output;

    output.position = patch[patchID].position;

    return output;
}