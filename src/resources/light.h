#pragma once

#include "math/math.h"

enum {
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_SPOTLIGHT,
};

typedef struct {
    int type;

    vec3 position; // для PointLight и SpotLight.
    vec3 direction; // для Directional и SpotLight.

    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;

    f32 power;

    // Только для Pointlight и Spotlight.
    f32 radius;
    f32 falloff;

    // Только для Spotlight.
    f32 inner_cone_angle;
    f32 outer_cone_angle;

    bool enabled;
    bool cast_shadows;
} Light;

Light *light_create(vec3 position, u32 type);
void light_destroy(Light *l);

void light_set_direction(Light *l, vec3 target);
void light_set_type(Light *l, u32 type);
