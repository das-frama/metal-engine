#pragma once

#include "defines.h"
#include "math/math.h"

typedef enum {
    TOPOLOGY_NONE,
    TOPOLOGY_TRIANGLE_LIST,
    TOPOLOGY_LINE_LIST,
} draw_topology;

typedef struct {
    vertex3d *vertices;
    u32 *indices;
    u32 vertex_count;
    u32 index_count;
    draw_topology topology;
    bool is_wireframe;
} Mesh_Data;

typedef struct Mesh Mesh;

Mesh_Data *mesh_generate_cube(f32 width, f32 height, f32 depth);
Mesh_Data *mesh_generate_sphere(f32 radius, s32 slice_count, s32 stack_count);
Mesh_Data *mesh_generate_grid(s32 width, s32 depth, s32 m, s32 n);
Mesh_Data *mesh_generate_circle(f32 raidus, s32 segments);
void mesh_destroy(Mesh_Data *m);

void print_mesh(Mesh_Data *m);
