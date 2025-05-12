#include "shader.h"

#include <stdlib.h>

Shader* shader_new(void) {
    Shader* out = calloc(1, sizeof(Shader));
    if (!out) return NULL;

    out->type = SHADER_TYPE_NONE;
    out->platform_shader = NULL;
    return out;
}

Shader* shader_load(Renderer* r, const char* path, const char* v_entry, const char* f_entry) {
    Shader* out = shader_new();
    // out->platform_shader = renderer_load_shader(r, path, v_entry, f_entry);

    return out;
}

void shader_destroy(Shader* s) {
    if (!s) return;

    // renderer_destroy_shader(s->platform_shader);

    free(s);
}
