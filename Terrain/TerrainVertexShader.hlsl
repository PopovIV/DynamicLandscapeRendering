struct VS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex2 : TEXCOORD1;
};

struct HS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex2 : TEXCOORD1;
};

HS_INPUT main(VS_INPUT input)
{
    HS_INPUT output;

    output.position = input.position;
    output.tex = input.tex;
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.binormal = input.binormal;
    output.tex2 = input.tex2;

    return output;
}
