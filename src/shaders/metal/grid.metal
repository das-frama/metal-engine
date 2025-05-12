#include "base.metal"

vertex float4 grid_vertex(VertexIn in [[stage_in]]) {
    return float4(in.position, 1.0);
}

fragment float4 grid_fragment() {
    return float4(1.0, 1.0, 0.0, 1.0);
}
