#define NUM_CONTROL_POINTS 3

cbuffer matrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
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

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

[domain("tri")]
PS_INPUT main(HS_CONSTANT_DATA_OUTPUT input, float3  uvwCoord : SV_DomainLocation, const OutputPatch<HS_OUTPUT, NUM_CONTROL_POINTS> patch)
{
    float3 vertexPos;
    PS_INPUT output;

    // Find new vertex pos
    vertexPos = patch[0].Pos * uvwCoord.x + patch[1].Pos * uvwCoord.y + patch[2].Pos * uvwCoord.z;

    // To new coords
    output.Pos = mul(float4(vertexPos, 1.0f), worldMatrix);
    output.Pos = mul(output.Pos, viewMatrix);
    output.Pos = mul(output.Pos, projectionMatrix);

    output.Color = patch[0].Color;

    return output;
}