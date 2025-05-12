#pragma once

#include "defines.h"
#include "math/math.h"

typedef struct Platform Platform;
typedef struct {
    void (*wait)(void *);
    void (*signal)(void *);
    void *impl;
} FrameWriter;

typedef struct Window {
    u16 width, height;
} Window;

Platform *platform_init(int width, int height, const char *title, bool *should_close);
void platform_destroy(Platform *p);

void platform_start_rendering(Platform *p);
void platform_poll_events(Platform *p);
bool platform_wait_frame(Platform *p);

vec2 platform_mouse_position(Platform *p);
vec2 platform_center_cursor();

void *platform_get_swapchain(Platform *p); // MTKView* (macos)
