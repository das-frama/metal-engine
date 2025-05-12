#include "light.h"
#include "math/math.h"

#include <stdlib.h>

Light *light_create(vec3 position, u32 type) {
    Light *l = calloc(1, sizeof(Light));
    l->position = position;
    l->diffuse_color = vec3_one();
    l->specular_color = vec3_one();
    l->ambient_color = vec3_new(0.25f, 0.25f, 0.25f);

    light_set_type(l, type);

    return l;
}

void light_destroy(Light *l) {
    free(l);
}

void light_set_direction(Light *l, vec3 target) {
    l->direction = vec3_normalize(vec3_sub(target, l->position));
}

void light_set_type(Light *l, u32 type) {
    if (type == LIGHT_TYPE_DIRECTIONAL) {
        l->type = LIGHT_TYPE_DIRECTIONAL;

        l->position = vec3_new(0.0f, 512.0f, 0.0f);
        light_set_direction(l, vec3_zero());

        l->ambient_color = vec3_mul(vec3_new(0.8f, 0.93f, 1.0f), 0.6f);
        l->diffuse_color = vec3_new(1.0f, 0.93f, 0.8f);
        l->specular_color = vec3_new(1.0f, 0.89f, 0.81f);

        l->power = 1.5f;
        l->falloff = 0.0f;

        l->enabled = true;
        l->cast_shadows = true;
    } else if (type == LIGHT_TYPE_POINT) {
        l->type = LIGHT_TYPE_POINT;

        l->ambient_color = vec3_new(0.05f, 0.05f, 0.05f);
        l->diffuse_color = vec3_new(1.0f, 1.0f, 1.0f);
        l->specular_color = vec3_new(1.0f, 1.0f, 1.0f);

        l->power = 1.0f;
        l->radius = 5.0f;
        l->falloff = 2.0f;

        l->enabled = true;
        l->cast_shadows = true;
    }
}
