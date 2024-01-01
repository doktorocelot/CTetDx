struct VertexInput {
    float2 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct VertexOutput {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

cbuffer AspectRatioBuffer : register(b0) {
    float AspectRatio;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    
    float2 scaledPosition = (input.Position / 12.0f);
    
    float2 adjustedPosition = scaledPosition;
    
    if (AspectRatio > 1.0f) {
        adjustedPosition.x /= AspectRatio;
    } else if (AspectRatio < 1.0f) {
        adjustedPosition.y *= AspectRatio;
    }
    
    output.Position = float4(adjustedPosition, 0.0f, 1.0f);
    output.TexCoord = input.TexCoord;
    
    return output;
}
