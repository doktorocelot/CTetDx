Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct PixelInput {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float Brightness : BRIGHTNESS;
    // float3 AddColor : ADDCOLOR;
};

float4 main(PixelInput input) : SV_TARGET {
    // float4 textureColor = g_Texture.Sample(g_Sampler, input.TexCoord);
    float4 color = g_Texture.Sample(g_Sampler, input.TexCoord);
    color *= float4(input.Brightness, input.Brightness, input.Brightness, 1.0);
    // float4 colorWithBrightness = textureColor * float4(input.Brightness, input.Brightness, input.Brightness, 1.0);
    //
    // return colorWithBrightness + float4(input.AddColor, 0.0);
    return color;
}
