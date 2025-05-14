#pragma once

#include "defines.h"

typedef struct {
    u8 *pixels;
    s32 width, height;
    s32 channels;
} Image_Data;

Image_Data image_load(const char *filename);
void image_destroy(Image_Data *id);
