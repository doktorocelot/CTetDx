struct VertexInput {
    float3 Position : POSITION;
};

struct VertexOutput {
    float4 Position : SV_POSITION;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    output.Position = float4(input.Position / 40.0f, 1.0f);
    return output;
}
