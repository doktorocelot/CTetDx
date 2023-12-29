Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct PixelInput {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float4 main(PixelInput input) : SV_TARGET {
    return g_Texture.Sample(g_Sampler, input.TexCoord);
}
