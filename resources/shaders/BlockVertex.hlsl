struct VertexInput {
    float2 Position : POSITION;
    float2 PositionInstance : POSITION_INSTANCE;
};

struct VertexOutput {
    float4 Position : SV_POSITION;
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
    
    // output.TexCoord = input.TexCoord;
    // output.Brightness = input.Brightness;    
    // output.AddColor = input.AddColor;

    return output;
}
