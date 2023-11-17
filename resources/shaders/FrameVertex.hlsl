struct VertexInput {
    float3 Position : POSITION;
};

struct VertexOutput {
    float4 Position : SV_POSITION;
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
    
    
    return output;
}
