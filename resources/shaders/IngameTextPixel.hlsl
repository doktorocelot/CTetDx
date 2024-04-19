Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct PixelInput {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

cbuffer AspectRatioBuffer : register(b0) {
    uint DistanceRange;
    float TexelSize;
};

float Median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float SDF(float2 uv) {
    const float CUTOFF = 0.5f;
    const float SHARPNESS = 2.0f;

    float4 sample = g_Texture.Sample(g_Sampler, uv);
    float winner = Median(sample.r, sample.g, sample.b);
    float dist = (CUTOFF - winner) * DistanceRange;
    float2 duv = fwidth(uv);
    float dtex = length(duv * float2(TexelSize, TexelSize));
    float pixelDist = dist * 1.5f / dtex;

    return saturate(0.5 - pixelDist);
}

float4 main(PixelInput input) : SV_TARGET {
    
    float sdf = SDF(input.TexCoord);

    return float4(1.0f, 1.0f, 1.0f, sdf);
}
