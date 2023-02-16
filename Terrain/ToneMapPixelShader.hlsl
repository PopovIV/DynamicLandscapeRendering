Texture2D<float4> sourceTexture : register(t0);

SamplerState Sampler : register(s0);

cbuffer AverageLuminanceBuffer : register(b0)
{
    float AverageLuminance;
}

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

float Exposure()
{
    float luminance = AverageLuminance;
    float keyValue = 1.03 - 2 / (2 + log10(luminance + 1));
    return keyValue / luminance;
}

float3 Uncharted2Tonemap(float3 x)
{
    static const float a = 0.1;  // Shoulder Strength
    static const float b = 0.50; // Linear Strength
    static const float c = 0.1;  // Linear Angle
    static const float d = 0.20; // Toe Strength
    static const float e = 0.02; // Toe Numerator
    static const float f = 0.30; // Toe Denominator
                                 // Note: e/f = Toe Angle

    return ((x * (a * x + c * b) + d * e) / (x * (a * x + b) + d * f)) - e / f;
}

float3 TonemapFilmic(float3 color)
{
    static const float W = 11.2; // Linear White Point Value

    float e = Exposure();
    float3 curr = Uncharted2Tonemap(e * 8 * color);
    float3 whiteScale = 1.0f / Uncharted2Tonemap(W);
    return curr * whiteScale;
}

float3 LinearToSRGB(float3 color)
{
    return pow(abs(color), 1 / 2.2f);
}

float4 main(PS_INPUT input) : SV_TARGET {
    float4 color =  sourceTexture.Sample(Sampler, input.tex);
    return float4(LinearToSRGB(TonemapFilmic(color.xyz)), color.a);
}
