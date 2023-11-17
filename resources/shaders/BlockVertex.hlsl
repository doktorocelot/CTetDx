struct VertexInput {
    float2 Position : POSITION;
    float Brightness : BRIGHTNESS;
    bool Enabled : ENABLED;
    float3 Color : COLOR;
};

struct VertexOutput {
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

cbuffer AspectRatioBuffer : register(b0) {
    float AspectRatio;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    
    float2 scaledPosition = (input.Position / 12.0f) * input.Enabled;
    
    float2 adjustedPosition = scaledPosition;
    
    if (AspectRatio > 1.0f) {
        adjustedPosition.x /= AspectRatio;
    } else if (AspectRatio < 1.0f) {
        adjustedPosition.y *= AspectRatio;
    }
    
    output.Position = float4(adjustedPosition, 0.0f, 1.0f);
    
    float3 color = input.Color * input.Brightness;
    output.Color = float4(color, 1.0);
    
    return output;
}
