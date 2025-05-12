#include "engine.h"
#include "core/entity.h"
#include "core/input.h"
#include "platform/platform.h"
#include "renderer/renderer.h"
#include "resources/mesh.h"

#include <mach/mach_time.h>
#include <stdio.h>
#include <stdlib.h>

static u64 last_fps_time = 0;
static int frame_count = 0;
static f64 fps = 0.0f;
static f64 frame_start = 0;
static f32 dt = 0;

static bool should_close = false;
static Renderer *renderer;
static Platform *platform;
static int screen_width = 0;
static int screen_height = 0;
static Entity *debug_grid = NULL;

f64 get_time_seconds() {
    static mach_timebase_info_data_t timebase;
    static bool initialized = false;
    if (!initialized) {
        mach_timebase_info(&timebase);
        initialized = true;
    }

    u64 time = mach_absolute_time();
    return (f64)time * timebase.numer / timebase.denom / 1e9;
}

void engine_print_fps(void) {
    f64 now = get_time_seconds();
    frame_count += 1;
    if (now - last_fps_time >= 1.0f) {
        fps = frame_count / (now - last_fps_time);
        printf("FPS: %.2f\n", fps);
        last_fps_time = now;
        frame_count = 0;
    }
}

void engine_init(u32 width, u32 height, const char *title) {
    printf("init engine...\n");
    platform = platform_init(width, height, title, &should_close);
    renderer = renderer_init(platform_get_swapchain(platform), width, height);
    platform_start_rendering(platform);

    screen_width = width;
    screen_height = height;

    input_init();
}

void engine_shutdown() {
    printf("shutdown engine...\n");
    renderer_destroy(renderer);
    platform_destroy(platform);
}

void engine_show_grid(bool show, Scene *s) {
    if (debug_grid == NULL && show) {
        Mesh_Data *md = mesh_generate_grid(100, 100, 50, 50);
        if (!md) {
            printf("Failed to create grid mesh\n");
            return;
        }

        Material_Data *mat = material_new(vec3_new(0.25f, 0.25f, 0.25f));
        mat->emissive = true;

        debug_grid = entity_create();
        debug_grid->mesh_id = renderer_load_mesh(renderer, md);
        debug_grid->material_id = renderer_load_material(renderer, mat);

        scene_add_entity(s, debug_grid);
    }

    if (debug_grid && !show) {
        scene_remove_entity(s, debug_grid);
        debug_grid = NULL;
    }
}

bool engine_should_close() {
    return should_close;
}

void engine_poll_events() {
    platform_poll_events(platform);
}

void engine_do_one_frame(Scene *scene, Camera *cam) {
    frame_start = get_time_seconds();

    // Подготовить рендер команды для GPU.
    Render_Command commands[1024];
    for (u32 i = 0; i < scene->entity_count; i++) {
        Entity *e = scene->entities[i];
        commands[i].model = model_matrix(e->position, e->rotation, e->scale);
        commands[i].mesh_id = e->mesh_id;
        commands[i].material_id = e->material_id;
    }

    renderer_begin_frame(renderer, cam, scene);
    renderer_draw(renderer, commands, scene->entity_count);
    renderer_end_frame(renderer);

    platform_wait_frame(platform);
    dt = get_time_seconds() - frame_start;
}

f32 engine_get_dt() {
    return dt;
}

Entity *engine_create_entity_with_mesh(Mesh_Data *md) {
    Entity *e = entity_create();

    e->mesh_id = renderer_load_mesh(renderer, md);
    e->material_id = 0;

    return e;
}

void engine_destory_entity(Entity *e) {
    // if (e->material) material_destroy(e->material);
    // if (e->mesh) mesh_destroy(e->mesh);
    renderer_destroy_mesh(renderer, e->mesh_id);
    renderer_destroy_material(renderer, e->material_id);

    entity_destroy(e);
}

u32 engine_load_mesh(Mesh_Data *mesh) {
    return renderer_load_mesh(renderer, mesh);
}

u32 engine_load_material(Material_Data *mat) {
    return renderer_load_material(renderer, mat);
}
