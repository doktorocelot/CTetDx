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

VertexOutput main(VertexInput input) {
    VertexOutput output;
    float2 position = (input.Position / 12.0f) * input.Enabled;
    output.Position = float4(position, 0.0f, 1.0f);
    float3 color = input.Color * input.Brightness;
    output.Color = float4(color, 1.0);
    return output;
}
