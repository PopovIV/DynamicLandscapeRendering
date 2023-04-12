Texture2D<float> sourceTexture : register(t0);

SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET{
    float3 color = float3(0, 0, 0);
    color.x = color.y = sourceTexture.Sample(Sampler, input.tex).r;
    return float4(color, 1.0);
}
