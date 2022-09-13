#define NUM_CONTROL_POINTS 3

cbuffer tessellationBuffer : register(b1)
{
    float tesselationAmount;
    float3 padding;
};

struct HS_INPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR;

};

struct HS_OUTPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
};


struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgesTessFactor[3]			: SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;

    Output.EdgesTessFactor[0] = padding[1];
    Output.EdgesTessFactor[1] = padding[0];
    Output.EdgesTessFactor[2] = padding[2];
    Output.InsideTessFactor = tesselationAmount;

    return Output;
}
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_OUTPUT main(InputPatch<HS_INPUT, NUM_CONTROL_POINTS> patch, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HS_OUTPUT output;
    output.Pos = patch[pointID].Pos;
    output.Color = patch[pointID].Color;

    return output;
}