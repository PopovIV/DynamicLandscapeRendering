struct VS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct HS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
HS_INPUT main(VS_INPUT input)
{
    HS_INPUT output;

    output.position = input.position;
    output.tex = input.tex;
    output.normal = input.normal;

    return output;
}