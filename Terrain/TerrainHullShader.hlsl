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

float calcTessFactor(float3 p) {
    float d = distance(p, cameraPos);

    float s = saturate((d - 16.0f) / (512.0f - 16.0f));
    return pow(2, (lerp(6, 0, s)));
}

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeFactors[4]         : SV_TessFactor;
    float InsideFactor[2] : SV_InsideTessFactor;
};

HS_CONSTANT_DATA_OUTPUT constantsHullShader(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;

    float3 v0 = mul(patch[0].position, worldMatrix);
    float3 v1 = mul(patch[1].position, worldMatrix);
    float3 v2 = mul(patch[2].position, worldMatrix);
    float3 v3 = mul(patch[3].position, worldMatrix);

    float3 e0 = 0.5f * (v0 + v2);
    float3 e1 = 0.5f * (v0 + v1);
    float3 e2 = 0.5f * (v1 + v3);
    float3 e3 = 0.5f * (v2 + v3);
    float3 c = 0.25f * (v0 + v1 + v2 + v3);

    // Assign tessellation levels
    output.EdgeFactors[0] = calcTessFactor(e0);
    output.EdgeFactors[1] = calcTessFactor(e1);
    output.EdgeFactors[2] = calcTessFactor(e2);
    output.EdgeFactors[3] = calcTessFactor(e3);
    output.InsideFactor[0] = calcTessFactor(c);
    output.InsideFactor[1] = output.InsideFactor[0];

    return output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("constantsHullShader")]
DS_INPUT main(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID)
{
    DS_INPUT output;

    output.position = patch[patchID].position;

    return output;
}