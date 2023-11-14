struct VertexInput {
    float3 Position : POSITION;
    float Brightness : TEXCOORD0;
    bool Enabled : TEXCOORD1;
    float3 Color : COLOR;
};

struct VertexOutput {
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    float3 position = (input.Position / 12.0f) * input.Enabled;
    output.Position = float4(position, 1.0f);
    float3 color = input.Color * input.Brightness;
    output.Color = float4(color, 1.0);
    return output;
}
