#pragma once

#include "core/entity.h"
#include "defines.h"
#include "resources/light.h"

#define MAX_ENTITIES 100
#define MAX_LIGHTS 10

typedef struct Scene {
    Entity **entities;
    u32 entity_count;
    u32 entity_capacity;

    Light *lights[MAX_LIGHTS];
    u32 light_count;
} Scene;

Scene *scene_create(void);
void scene_destroy(Scene *s);

void scene_add_entity(Scene *s, Entity *e);
void scene_remove_entity(Scene *s, Entity *e);

void scene_add_light(Scene *s, Light* l);
void scene_remove_light(Scene *s, u32 i);
