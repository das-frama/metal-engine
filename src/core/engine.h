#pragma once

#include "core/camera.h"
#include "core/entity.h"
#include "core/scene.h"
#include "defines.h"
#include "resources/material.h"
#include "resources/mesh.h"
#include "resources/texture.h"

// Инициализация и завершение.
void engine_init(u32 width, u32 height, const char *title);
void engine_shutdown(void);

void engine_show_grid(bool show, Scene *s);

// Рендер окна.
void engine_poll_events();
void engine_do_one_frame(Scene *scene, Camera *camera);
f32 engine_get_dt(void);
bool engine_should_close(void);

// Entity.
Entity *engine_create_entity_with_mesh(Mesh_Data *mesh);
void engine_destory_entity(Entity *e);

// Resources.
u32 engine_load_mesh(Mesh_Data *mesh);
u32 engine_load_material(Material_Data *mat);
Texture* engine_load_texture(const char *filename);

// Разное.
void engine_print_fps(void);
