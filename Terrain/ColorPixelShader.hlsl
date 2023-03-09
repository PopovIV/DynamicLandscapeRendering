struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 other_color = float4(1, 1, 1, 1);
    float alpha = input.worldPosition.y / 3;

    return input.color;
}
