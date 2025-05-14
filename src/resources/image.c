#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

Image_Data image_load(const char *filename) {
    Image_Data image = {0};
    image.pixels = stbi_load(filename, &image.width, &image.height, &image.channels, 0);
    return image;
}

void image_destroy(Image_Data *image) {
    stbi_image_free(image->pixels);
    image->width = 0;
    image->height = 0;
    image->channels = 0;
}
