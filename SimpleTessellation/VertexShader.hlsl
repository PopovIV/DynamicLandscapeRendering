struct VS_INPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
};

struct HS_INPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
};

HS_INPUT main(VS_INPUT input)
{
    HS_INPUT output;
    output.Pos = input.Pos.xyz * 6.0f;
    output.Color = input.Color;

    return output;
}