struct VS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct HS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
HS_INPUT main(VS_INPUT input)
{
    HS_INPUT output;

    output.position = input.position;
    output.tex = input.tex;

    return output;
}