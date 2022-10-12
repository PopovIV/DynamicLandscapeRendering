struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct HS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
HS_INPUT main(VS_INPUT input)
{
    HS_INPUT output;

    output.position = input.position;
    output.color = input.color;

    return output;
}