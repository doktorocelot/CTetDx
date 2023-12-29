struct VertexInput {
    float2 Position : POSITION;
    float Brightness : BRIGHTNESS;
    bool Enabled : ENABLED;
    float2 TexCoord : TEXCOORD0;
    float3 AddColor : ADDCOLOR;
};

struct VertexOutput {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float Brightness : BRIGHTNESS;
    float3 AddColor : ADDCOLOR;
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
    
    output.TexCoord = input.TexCoord;
    output.Brightness = input.Brightness;    
    output.AddColor = input.AddColor;

    return output;
}
