Texture2D shaderTexture;
SamplerState SampleType;

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 textureColor;

    textureColor = shaderTexture.Sample(SampleType, input.tex);

    return textureColor;
}