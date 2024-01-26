struct VertexInput {
    float2 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float2 PositionInstance : POSITION_INSTANCE;
    float2 Brightness : BRIGHTNESS_INSTANCE;
    float2 TexOffset : TEXOFFSET_INSTANCE;
};

struct VertexOutput {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float Brightness : BRIGHTNESS;
};

cbuffer AspectRatioBuffer : register(b0) {
    float AspectRatio;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    float2 finalPos = input.Position + input.PositionInstance;
    finalPos /= 12.0f;
    
    if (AspectRatio > 1.0f) {
        finalPos.x /= AspectRatio;
    } else if (AspectRatio < 1.0f) {
        finalPos.y *= AspectRatio;
    }
    
    output.Position = float4(finalPos, 0.0f, 1.0f);
    
    output.TexCoord = input.TexCoord + input.TexOffset * float2(100.0f/512.0f,100.0f/512.0f);
    output.Brightness = input.Brightness;    
    // output.AddColor = input.AddColor;

    return output;
}
