#include "math.h"

#include <stdio.h>

mat4 model_matrix(vec3 position, quat rotate, vec3 scale) {
    mat4 t = mat4_translate(mat4_id(), position);
    mat4 r = mat4_from_quat(rotate);
    mat4 s = mat4_scale(scale);

    return mat4_mul(mat4_mul(t, r), s);
}

void mat4_print(mat4 m) {
    for (int i = 0; i < 4; i++) {
        printf("[%.4f %.4f %.4f %.4f]\n",
            m.data[MAT4_AT(i, 0)],
            m.data[MAT4_AT(i, 1)],
            m.data[MAT4_AT(i, 2)],
            m.data[MAT4_AT(i, 3)]
        );
    }
    printf("\n");
}
