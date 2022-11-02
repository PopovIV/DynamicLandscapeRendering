#define NUM_CONTROL_POINTS 4

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 cameraPos;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
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


[domain("quad")]
PS_INPUT main(HS_CONSTANT_DATA_OUTPUT input, float2 domain : SV_DomainLocation, const OutputPatch<DS_INPUT, NUM_CONTROL_POINTS> patch)
{
    PS_INPUT output;

    output.position = lerp(lerp(patch[0].position, patch[1].position, domain.x), lerp(patch[2].position, patch[3].position, domain.x), domain.y);
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    return output;
}