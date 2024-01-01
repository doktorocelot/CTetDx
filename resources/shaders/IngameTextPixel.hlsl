Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct PixelInput {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float SDF(float2 uv) {
    // fetch the SDF value from the texture
    // use the g_Sampler to interpolate values between texels
    float dist = g_Texture.Sample(g_Sampler, uv).r;

    // choose a value for the smoothing radius
    const float smoothing = 0.05f;
    float smoothed = smoothstep(0.5f - smoothing, 0.5f + smoothing, dist);

    return smoothed;
}

float4 main(PixelInput input) : SV_TARGET {
    
    float sdf = SDF(input.TexCoord);

    return float4(1.0f, 1.0f, 1.0f, sdf);
}
