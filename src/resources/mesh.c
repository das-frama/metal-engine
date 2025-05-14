#include "mesh.h"
#include "math/math.h"

#include <stdio.h>
#include <stdlib.h>

Mesh_Data *mesh_generate_cube(f32 width, f32 height, f32 depth) {
    Mesh_Data *out = calloc(1, sizeof(Mesh_Data));
    out->vertex_count = 24;
    out->index_count = 36; // Fixed: A cube has 12 triangles (6 faces × 2 triangles), so 36 indices, not 12
    out->vertices = calloc(out->vertex_count, sizeof(vertex3d));
    out->indices = calloc(out->index_count, sizeof(u32));
    out->topology = TOPOLOGY_TRIANGLE_LIST;

    f32 half_width = width * 0.5f;
    f32 half_height = height * 0.5f;
    f32 half_depth = depth * 0.5f;
    f32 min_x = -half_width;
    f32 min_y = -half_height;
    f32 min_z = -half_depth;
    f32 max_x = half_width;
    f32 max_y = half_height;
    f32 max_z = half_depth;

    vertex3d verts[] = {
        // Передняя грань (z = max_z)
        {.position = {min_x, min_y, max_z}, .normal = {0.0, 0.0, 1.0}, .uv = {0.0, 0.0}}, // 0
        {.position = {max_x, min_y, max_z}, .normal = {0.0, 0.0, 1.0}, .uv = {1.0, 0.0}}, // 1
        {.position = {max_x, max_y, max_z}, .normal = {0.0, 0.0, 1.0}, .uv = {1.0, 1.0}}, // 2
        {.position = {min_x, max_y, max_z}, .normal = {0.0, 0.0, 1.0}, .uv = {0.0, 1.0}}, // 3
        // Задняя грань (z = min_z)
        {.position = {min_x, min_y, min_z}, .normal = {0.0, 0.0, -1.0}, .uv = {1.0, 0.0}}, // 4
        {.position = {max_x, min_y, min_z}, .normal = {0.0, 0.0, -1.0}, .uv = {0.0, 0.0}}, // 5
        {.position = {max_x, max_y, min_z}, .normal = {0.0, 0.0, -1.0}, .uv = {0.0, 1.0}}, // 6
        {.position = {min_x, max_y, min_z}, .normal = {0.0, 0.0, -1.0}, .uv = {1.0, 1.0}}, // 7
        // Левая грань (x = min_x)
        {.position = {min_x, min_y, min_z}, .normal = {-1.0, 0.0, 0.0}, .uv = {0.0, 0.0}}, // 8
        {.position = {min_x, min_y, max_z}, .normal = {-1.0, 0.0, 0.0}, .uv = {1.0, 0.0}}, // 9
        {.position = {min_x, max_y, max_z}, .normal = {-1.0, 0.0, 0.0}, .uv = {1.0, 1.0}}, // 10
        {.position = {min_x, max_y, min_z}, .normal = {-1.0, 0.0, 0.0}, .uv = {0.0, 1.0}}, // 11
        // Правая грань (x = max_x)
        {.position = {max_x, min_y, max_z}, .normal = {1.0, 0.0, 0.0}, .uv = {0.0, 0.0}}, // 12
        {.position = {max_x, min_y, min_z}, .normal = {1.0, 0.0, 0.0}, .uv = {1.0, 0.0}}, // 13
        {.position = {max_x, max_y, min_z}, .normal = {1.0, 0.0, 0.0}, .uv = {1.0, 1.0}}, // 14
        {.position = {max_x, max_y, max_z}, .normal = {1.0, 0.0, 0.0}, .uv = {0.0, 1.0}}, // 15
        // Верхняя грань (y = max_y)
        {.position = {min_x, max_y, max_z}, .normal = {0.0, 1.0, 0.0}, .uv = {0.0, 0.0}}, // 16
        {.position = {max_x, max_y, max_z}, .normal = {0.0, 1.0, 0.0}, .uv = {1.0, 0.0}}, // 17
        {.position = {max_x, max_y, min_z}, .normal = {0.0, 1.0, 0.0}, .uv = {1.0, 1.0}}, // 18
        {.position = {min_x, max_y, min_z}, .normal = {0.0, 1.0, 0.0}, .uv = {0.0, 1.0}}, // 19
        // Нижняя грань (y = min_y)
        {.position = {min_x, min_y, min_z}, .normal = {0.0, -1.0, 0.0}, .uv = {0.0, 0.0}}, // 20
        {.position = {max_x, min_y, min_z}, .normal = {0.0, -1.0, 0.0}, .uv = {1.0, 0.0}}, // 21
        {.position = {max_x, min_y, max_z}, .normal = {0.0, -1.0, 0.0}, .uv = {1.0, 1.0}}, // 22
        {.position = {min_x, min_y, max_z}, .normal = {0.0, -1.0, 0.0}, .uv = {0.0, 1.0}}, // 23
    };

    memcpy(out->vertices, verts, sizeof(vertex3d) * out->vertex_count);

    u32 indices[] = {
        // Передняя грань
        0,
        1,
        2,
        2,
        3,
        0,
        // Задняя грань
        4,
        5,
        6,
        6,
        7,
        4,
        // Левая грань
        8,
        9,
        10,
        10,
        11,
        8,
        // Правая грань
        12,
        13,
        14,
        14,
        15,
        12,
        // Верхняя грань
        16,
        17,
        18,
        18,
        19,
        16,
        // Нижняя грань
        20,
        21,
        22,
        22,
        23,
        20,
    };
    memcpy(out->indices, indices, sizeof(u32) * out->index_count);

    return out;
}

Mesh_Data *mesh_generate_grid(s32 width, s32 depth, s32 m, s32 n) {
    Mesh_Data *out = calloc(1, sizeof(Mesh_Data));
    out->vertex_count = (m + 1 + n + 1) * 2;
    out->index_count = out->vertex_count;
    out->vertices = calloc(out->vertex_count, sizeof(vertex3d));
    out->indices = calloc(out->index_count, sizeof(u32));
    out->topology = TOPOLOGY_LINE_LIST;

    f32 half_width = (f32)width * 0.5f;
    f32 half_depth = (f32)depth * 0.5f;
    f32 dx = (f32)width / (f32)m;
    f32 dz = (f32)depth / (f32)n;

    // Create vertices for grid lines along x-axis
    for (int i = 0; i <= m; i++) {
        f32 x = -half_width + i * dx;
        // Line from front to back at position x
        out->vertices[i * 2].position = vec3_new(x, 0, half_depth);
        out->vertices[i * 2].normal = vec3_new(0, 1, 0); // Fixed: normal should be up
        out->vertices[i * 2].uv = vec2_zero();

        out->vertices[i * 2 + 1].position = vec3_new(x, 0, -half_depth);
        out->vertices[i * 2 + 1].normal = vec3_new(0, 1, 0);
        out->vertices[i * 2 + 1].uv = vec2_zero();
    }

    // Create vertices for grid lines along z-axis
    int base_index = (m + 1) * 2;
    for (int i = 0; i <= n; i++) {
        f32 z = -half_depth + i * dz;
        // Line from left to right at position z
        out->vertices[base_index + i * 2].position = vec3_new(-half_width, 0, z);
        out->vertices[base_index + i * 2].normal = vec3_new(0, 1, 0);
        out->vertices[base_index + i * 2].uv = vec2_zero();

        out->vertices[base_index + i * 2 + 1].position = vec3_new(half_width, 0, z);
        out->vertices[base_index + i * 2 + 1].normal = vec3_new(0, 1, 0);
        out->vertices[base_index + i * 2 + 1].uv = vec2_zero();
    }

    // Create indices for grid lines
    int index = 0;
    // Indices for lines along x-axis
    for (int i = 0; i <= m; i++) {
        out->indices[index++] = i * 2;
        out->indices[index++] = i * 2 + 1;
    }
    // Indices for lines along z-axis
    for (int i = 0; i <= n; i++) {
        out->indices[index++] = base_index + i * 2;
        out->indices[index++] = base_index + i * 2 + 1;
    }

    return out;
}

Mesh_Data *mesh_generate_sphere(f32 radius, s32 slice_count, s32 stack_count) {
    Mesh_Data *out = calloc(1, sizeof(Mesh_Data));
    out->vertex_count = (stack_count + 1) * (slice_count + 1) + 2; // Fixed: include top and bottom vertices
    out->index_count =
        3 * (2 * slice_count * (stack_count - 1) + 2 * slice_count); // Fixed: correct index count formula
    out->vertices = calloc(out->vertex_count, sizeof(vertex3d));
    out->indices = calloc(out->index_count, sizeof(u32));
    out->topology = TOPOLOGY_TRIANGLE_LIST;

    vertex3d top_vertex = {{0, radius, 0}, {0, 1, 0}, {0, 0}};
    vertex3d bottom_vertex = {{0, -radius, 0}, {0, -1, 0}, {0, 1}}; // Fixed: UV coordinates for bottom vertex

    out->vertices[0] = top_vertex;

    f32 phi_step = M_PI / stack_count;
    f32 theta_step = M_PI * 2 / slice_count;

    // Compute vertices.
    int vertex_index = 1; // Start after top vertex
    for (int i = 0; i <= stack_count; i++) {
        f32 phi = i * phi_step;
        for (int j = 0; j <= slice_count; j++) {
            f32 theta = j * theta_step;
            vertex3d v = {0};
            v.position.x = radius * sinf(phi) * cosf(theta);
            v.position.y = radius * cosf(phi);
            v.position.z = radius * sinf(phi) * sinf(theta);

            v.uv.x = (f32)j / slice_count;
            v.uv.y = (f32)i / stack_count;

            v.normal = vec3_normalize(v.position);

            if (vertex_index < out->vertex_count) {
                out->vertices[vertex_index++] = v;
            }
        }
    }

    int south_pole_index = vertex_index;
    if (south_pole_index < out->vertex_count) {
        out->vertices[south_pole_index] = bottom_vertex;
    }

    // Compute indices.
    int index = 0;
    int base_index = 1;
    int ring_vertex_count = slice_count + 1;
    // Middle
    for (int i = 0; i <= stack_count - 1; i++) {
        for (int j = 0; j < slice_count; j++) {
            if (index + 5 < out->index_count) {
                out->indices[index++] = base_index + i * ring_vertex_count + j;
                out->indices[index++] = base_index + i * ring_vertex_count + j + 1;
                out->indices[index++] = base_index + (i + 1) * ring_vertex_count + j;

                out->indices[index++] = base_index + (i + 1) * ring_vertex_count + j;
                out->indices[index++] = base_index + i * ring_vertex_count + j + 1;
                out->indices[index++] = base_index + (i + 1) * ring_vertex_count + j + 1;
            }
        }
    }

    return out;
}

Mesh_Data *mesh_generate_circle(f32 radius, s32 segments) {
    Mesh_Data *out = calloc(1, sizeof(Mesh_Data));
    out->index_count = segments + 1;
    out->vertex_count = segments + 1;
    out->vertices = calloc(out->vertex_count, sizeof(vertex3d));
    out->indices = calloc(out->index_count, sizeof(u32));
    out->topology = TOPOLOGY_LINE_LIST;

    f32 theta_step = M_PI * 2 / segments;
    for (int i = 0; i <= segments; i++) {
        f32 theta = i * theta_step;
        vertex3d v = {0};
        v.position.x = radius * cosf(theta);
        v.position.y = radius * 0;
        v.position.z = radius * sinf(theta);

        v.normal = vec3_normalize(v.position);

        out->vertices[i] = v;
        out->indices[i] = i;
    }

    return out;
}

void mesh_destroy(Mesh_Data *m) {
    if (!m) return;
    if (m->indices) free(m->indices);
    if (m->vertices) free(m->vertices);

    memset(m, 0, sizeof(Mesh_Data));
    free(m);
}

void print_mesh(Mesh_Data *m) {
    printf("Mesh: vertex_count = %d, index_count = %d\n", m->vertex_count, m->index_count);
}
