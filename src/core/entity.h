#pragma once

#include "math/math.h"
#include "resources/material.h"
#include "resources/mesh.h"

typedef struct Entity {
    vec3 position;
    quat rotation;
    vec3 scale;

    u32 mesh_id;
    u32 material_id;
} Entity;

Entity *entity_create();
void entity_destroy(Entity *e);
