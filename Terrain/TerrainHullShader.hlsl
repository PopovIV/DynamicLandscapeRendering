#define NUM_CONTROL_POINTS 3

struct HS_INPUT
{
    float4 position : POSITION;
    float4 color    : COLOR;
};

struct DS_INPUT
{
    float4 position : POSITION;
    float4 color    : COLOR;
};


struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeFactors[3]         : SV_TessFactor;
    float InsideFactor           : SV_InsideTessFactor;
};

// Функция константы исправления
HS_CONSTANT_DATA_OUTPUT constantsHullShader(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;

    float cullFactor = 1.0f;

    // Assign tessellation levels (constant for now)
    output.EdgeFactors[0] = 4.0f * cullFactor;
    output.EdgeFactors[1] = 4.0f * cullFactor;
    output.EdgeFactors[2] = 4.0f * cullFactor;
    output.InsideFactor = 9.0f * cullFactor;

    return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("constantsHullShader")]
DS_INPUT main(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_OutputControlPointID)
{
    DS_INPUT output;

    // Copy inputs to outputs
    output.position = patch[patchID].position;
    output.color = patch[patchID].color;

    return output;
}
