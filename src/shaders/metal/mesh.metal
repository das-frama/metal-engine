#include "base.metal"

inline float3 apply_light(Light light, float3 world_pos, float3 normal, float3 V, float3 material_color) {
    if (!light.enabled) return float3(0.0);

    float3 N = normalize(normal);
    float3 result = float3(0.0);
    float shininess = 32.0;

    float3 ambient = light.ambient_color * light.power;
    float3 diffuse = float3(0.0);
    float3 specular = float3(0.0);

    if (light.type == 0) { // Directional.
        float3 L = normalize(-light.direction);
        float3 R = reflect(-L, N);
        // float3 D = light.target - light.position;
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(R, V), 0.0), shininess);

        diffuse = diff * light.diffuse_color * light.power * material_color; // * material.kd
        specular = spec * light.specular_color * light.power; // * material.ks;
        result = ambient + diffuse + specular;
    } else if (light.type == 1) { // Point Light.
        float3 to_light = light.position - world_pos;
        float distance = length(to_light);
        float3 L = normalize(to_light);
        float3 H = normalize(L + V);

        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), shininess);

        float attenuation = clamp(1.0 - distance / light.radius, 0.0, 1.0);
        attenuation = pow(attenuation, light.falloff);

        diffuse = diff * light.diffuse_color * light.power * material_color;
        specular = spec * light.specular_color * light.power;

        result = ambient + (diffuse + specular) * attenuation;
    }

    return result;
}

vertex VertexOut mesh_vertex(VertexIn in [[stage_in]], constant Uniforms &u [[buffer(1)]],
                             constant float4x4 &model [[buffer(2)]]) {
    VertexOut out;
    out.position = u.proj * u.view * model * float4(in.position, 1.0);
    out.normal = (model * float4(in.normal, 0.0)).xyz;
    out.uv = in.uv;
    out.world_position = (model * float4(in.position, 1.0)).xyz;
    return out;
}

fragment float4 mesh_fragment(VertexOut in [[stage_in]], constant Uniforms &u [[buffer(0)]],
                              constant Light_Buffer &light_buffer [[buffer(1)]],
                              constant Material &material [[buffer(2)]]) {
    if (material.emissive) {
        return float4(material.color, 1.0);
    }

    float3 result = float3(0.0);
    float3 V = normalize(u.camera_position.xyz - in.world_position);

    for (uint i = 0; i < light_buffer.light_count; i++) {
        result += apply_light(light_buffer.lights[i], in.world_position, in.normal, V, material.color);
    }

    return float4(result, 1.0);
}
