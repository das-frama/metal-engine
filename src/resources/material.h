#pragma once

#include "defines.h"
#include "math/math.h"

typedef struct Material_Data {
    vec3 color;
    bool emissive;
    bool receive_light;
} Material_Data;

Material_Data *material_new(vec3 color);
void material_destroy(Material_Data *mat);
