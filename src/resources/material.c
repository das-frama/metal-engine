#include "material.h"

#include <stdlib.h>

Material_Data *material_new(vec3 color) {
    Material_Data *out = calloc(1, sizeof(Material_Data));
    out->color = color;

    return out;
}

void material_destroy(Material_Data *mat) {
    free(mat);
}
