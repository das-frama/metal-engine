#include "scene.h"
#include "resources/light.h"

#include <stdint.h>
#include <stdlib.h>

Scene *scene_create(void) {
    Scene *s = calloc(1, sizeof(Scene));

    s->entity_count = 0;
    s->entity_capacity = 16;
    s->entities = calloc(s->entity_capacity, sizeof(Entity));

    return s;
}

void scene_destroy(Scene *s) {
    if (!s) return;

    for (u32 i = 0; i < s->entity_count; i++) {
        entity_destroy(s->entities[i]);
    }

    s->entity_count = 0;
    s->entity_capacity = 0;
    // free lights
    for (int i = 0; i < s->light_count; i++) {
        light_destroy(s->lights[i]);
    }
    free(s->entities);
    free(s);
}

void scene_add_entity(Scene *s, Entity *e) {
    assert(s != NULL && e != NULL);

    if (s->entity_count >= s->entity_capacity) {
        s->entity_capacity *= 2;
        s->entities = realloc(s->entities, sizeof(Entity) * s->entity_capacity);
    }

    s->entities[s->entity_count++] = e;
}

void scene_remove_entity(Scene *s, Entity *e) {
    // find entity from list.
    for (int i = 0; i < s->entity_count; i++) {
        if (s->entities[i] == e) {
            s->entities[i] = s->entities[--s->entity_count];
            return;
        }
    }
}

void scene_add_light(Scene *s, Light *l) {
    assert(s && l);
    if (s->light_count >= MAX_LIGHTS) return;

    s->lights[s->light_count++] = l;
}

void scene_remove_light(Scene *s, u32 index) {
    assert(s && index < s->light_count);

    s->lights[index] = s->lights[s->light_count - 1];
    s->light_count--;
}
