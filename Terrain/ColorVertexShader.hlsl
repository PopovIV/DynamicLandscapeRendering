cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.worldPosition = output.position;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the input color for the pixel shader to use.
    output.color = input.color;

    return output;
}
