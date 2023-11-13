struct VertexInput {
    float3 Position : POSITION;
    float Brightness : TEXCOORD0;
};

struct VertexOutput {
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    output.Position = float4(input.Position / 12.0f, 1.0f);
    output.Color = float4(input.Brightness, input.Brightness, input.Brightness, 1.0);
    return output;
}
