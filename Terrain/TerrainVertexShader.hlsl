struct VS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct HS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

HS_INPUT main(VS_INPUT input)
{

    HS_INPUT output;

    output.position = input.position;
    output.tex = input.tex;
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.binormal = input.binormal;

    return output;
}