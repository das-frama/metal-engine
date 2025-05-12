#pragma once

#include "math/math.h"

typedef struct Renderer Renderer;

typedef enum {
    SHADER_TYPE_NONE,
    SHADER_TYPE_METAL,
} Shader_Type;

typedef struct Shader {
    void *platform_shader;
    Shader_Type type;
} Shader;

Shader* shader_new(void);
Shader* shader_load(Renderer *r, const char* path, const char* v_entry, const char* f_entry);
void shader_destroy(Shader* s);
