#include "entity.h"

#include <stdlib.h>

Entity *entity_create() {
    Entity *e = calloc(1, sizeof(Entity));
    e->position = vec3_zero();
    e->rotation = quat_id();
    e->scale = vec3_one();
    // e->mesh_id = -1;
    // e->material_id = -1;
    return e;
}

void entity_destroy(Entity *e) {
    if (!e) return;

    // if (e->mesh) renderer_destroy_mesh(renderer, u32 mesh_id)
    // if (e->material) material_destroy(e->material);

    memset(e, 0, sizeof(Entity));
    free(e);
}
