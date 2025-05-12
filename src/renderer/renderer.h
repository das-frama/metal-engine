#pragma once

#include "core/camera.h"
#include "core/scene.h"
#include "defines.h"
#include "resources/material.h"
#include "resources/mesh.h"

typedef struct Uniforms {
    mat4 view;
    mat4 proj;
    vec4 camera_position;
} Uniforms;

typedef struct Render_Command {
    mat4 model;
    u32 mesh_id;
    u32 material_id;
} Render_Command;

typedef struct Renderer Renderer;

Renderer *renderer_init(void *platform_data, u32 width, u32 height);
void renderer_destroy(Renderer *r);

u32 renderer_load_mesh(Renderer *r, Mesh_Data *mesh);
u32 renderer_load_material(Renderer *r, Material_Data *mat);
void renderer_destroy_mesh(Renderer *r, u32 mesh_id);
void renderer_destroy_material(Renderer *r, u32 material_id);

void renderer_begin_frame(Renderer *r, Camera *cam, Scene *scene);
void renderer_draw(Renderer *r, Render_Command *commands, u32 command_count);
void renderer_end_frame(Renderer *r);

void *renderer_load_shader(Renderer *r, const char *filepath, const char *v_entry, const char *f_entry);
void renderer_destroy_shader(void *s);
