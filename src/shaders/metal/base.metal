#include <metal_stdlib>

using namespace metal;

#define MAX_LIGHTS 10

struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 uv [[attribute(2)]];
};

struct VertexOut {
    float4 position [[position]];
    float3 normal;
    float2 uv;
    float3 world_position;
};

struct Uniforms {
    float4x4 view;
    float4x4 proj;
    float4 camera_position;
};

struct Light {
    uint type;

    float3 position;
    float3 direction;

    float3 ambient_color;
    float3 diffuse_color;
    float3 specular_color;

    float power;

    float falloff;
    float radius;

    float inner_cone_angle;
    float outer_cone_angle;

    bool enabled;
    bool cast_shadows;
};

struct Light_Buffer {
    Light lights[MAX_LIGHTS];
    uint light_count;
};

struct Material {
    float3 color;
    bool emissive;
    bool receive_light;
    bool has_texture;
};
