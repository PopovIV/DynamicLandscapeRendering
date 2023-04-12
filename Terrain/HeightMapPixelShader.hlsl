Texture2D<float2> sourceTexture : register(t0);

SamplerState minSampler : register(s0);
SamplerState maxSampler : register(s1);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET{
    float3 color = float3(0, 0, 0);
    color.x = sourceTexture.Sample(minSampler, input.tex).r;
    color.y = sourceTexture.Sample(maxSampler, input.tex).g;
    return float4(color, 1.0);
}
