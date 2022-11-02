struct VS_INPUT
{
    float4 position : POSITION;
};

struct HS_INPUT
{
    float4 position : POSITION;
};

HS_INPUT main(VS_INPUT input)
{
    HS_INPUT output;

    output.position = input.position;

    return output;
}