#define NUM_CONTROL_POINTS 3

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};


struct DS_INPUT
{
    float4 position : POSITION;
    float4 color    : COLOR;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3]         : SV_TessFactor;
    float InsideTessFactor          : SV_InsideTessFactor;
};

[domain("tri")]
PS_INPUT main(HS_CONSTANT_DATA_OUTPUT input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<DS_INPUT, NUM_CONTROL_POINTS> patch)
{
    float3 vertexPos;
    PS_INPUT output;

    // Find new vertex pos
    vertexPos = patch[0].position * uvwCoord.x + patch[1].position * uvwCoord.y + patch[2].position * uvwCoord.z;

    // To new coords
    output.position = mul(float4(vertexPos, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.color = patch[0].color;

    return output;
}
