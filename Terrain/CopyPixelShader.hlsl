Texture2D<float4> sourceTexture : register(t0);

SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET{
    return sourceTexture.Sample(Sampler, input.tex);
}
