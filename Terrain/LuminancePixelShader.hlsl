Texture2D<float4> sourceTexture : register(t0);

SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = sourceTexture.Sample(Sampler, input.tex);
    float l = 0.2126f * color.r + 0.7151f * color.g + 0.0722f * color.b;
    return log(l + 1);
}
