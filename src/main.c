#include "core/engine.h"
#include "core/entity.h"
#include "core/input.h"
#include "core/scene.h"
#include "math/math.h"
#include "resources/light.h"
#include "resources/material.h"
#include "resources/mesh.h"

#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct {
    Entity *entity;
    Light *light;
    Material_Data *mat;
} Sun;

typedef struct Planet {
    Entity *entity;
    Entity *orbit_entity;

    f32 diameter;
    f32 orbit;
    f32 orbit_period;
    f32 orbit_angle;

    struct Planet *sputniks[5];
    u8 sputnik_count;
} Planet;

static Sun sun;
static Planet planets[8];
static Mesh_Data *sphere_mesh = NULL;

Sun create_sun() {
    assert(sphere_mesh);

    // Сущность.
    Sun sun = {0};
    sun.entity = entity_create();
    sun.entity->scale = vec3_mul(sun.entity->scale, 2.0f);

    // Меш.
    sun.entity->mesh_id = engine_load_mesh(sphere_mesh);

    // Материал.
    sun.mat = material_new(vec3_new(1.0, 1.0, 0.5));
    sun.mat->emissive = true;
    sun.entity->material_id = engine_load_material(sun.mat);

    // Создать свет.
    sun.light = light_create(vec3_new(0.0, 0.0, 0.0), LIGHT_TYPE_POINT);
    sun.light->power = 0.5f;
    sun.light->radius = 0.0f;
    sun.light->falloff = 0.0f;
    sun.light->ambient_color = vec3_new(0.01f, 0.01f, 0.01f);
    sun.light->diffuse_color = vec3_new(1.0f, 1.0f, 0.58f);
    sun.light->specular_color = vec3_new(0.5f, 0.5f, 0.5f);

    return sun;
}

Planet create_planet(f32 diameter, f32 orbit, f32 orbit_period, const char *image) {
    assert(sphere_mesh);

    Planet planet = {0};
    planet.diameter = diameter;
    planet.orbit = orbit;
    planet.orbit_period = orbit_period;
    planet.entity = entity_create();

    Material_Data *mat = material_new(vec3_one());
    // Загрузка текстуры.
    if (image) {
        mat->albedo_texture = engine_load_texture(image);
    }

    planet.entity->material_id = engine_load_material(mat);
    planet.entity->mesh_id = engine_load_mesh(sphere_mesh);

    f32 theta = 0.0f;
    planet.entity->position = vec3_new(orbit * cosf(theta), 0.0f, orbit * sinf(theta));
    planet.entity->rotation = quat_id();
    planet.entity->scale = vec3_new(diameter, diameter, diameter);

    // Орбита.
    planet.orbit_entity = entity_create();
    Mesh_Data *orbit_mesh = mesh_generate_circle(orbit, 100);
    Material_Data *orbit_mat = material_new(vec3_one());
    orbit_mat->emissive = true;
    planet.orbit_entity->mesh_id = engine_load_mesh(orbit_mesh);
    planet.orbit_entity->material_id = engine_load_material(orbit_mat);

    planet.orbit_entity->position = vec3_zero();
    planet.orbit_entity->rotation = quat_id();
    planet.orbit_entity->scale = vec3_one();

    return planet;
}

void create_solar_system(Scene *scene) {
    // Создать сферический меш.
    sphere_mesh = mesh_generate_sphere(1.0f, 32, 32);

    // Солнце.
    sun = create_sun();
    scene_add_entity(scene, sun.entity);
    scene_add_light(scene, sun.light);

    // Планеты.
    planets[0] = create_planet(0.15f, 5.00f, 88.0f, NULL);  // Меркурий
    planets[1] = create_planet(0.35f, 7.50f, 225.0f, NULL); // Венера
    planets[2] =
        create_planet(0.40f, 10.0f, 365.0f, "/Users/frama/Dev/Engines/mac-engine/assets/earth_day.jpg"); // Земля
    planets[3] = create_planet(0.38f, 13.0f, 687.0f, NULL);                                              // Марс
    planets[4] = create_planet(1.20f, 18.0f, 4333.0f, NULL);                                             // Юпитер
    planets[5] = create_planet(1.00f, 24.0f, 10759.0f, NULL);                                            // Сатурн
    planets[6] = create_planet(0.60f, 30.0f, 30687.0f, NULL);                                            // Уран
    planets[7] = create_planet(0.60f, 36.0f, 60190.0f, NULL);                                            // Нептун
    for (int i = 0; i < 8; i++) {
        scene_add_entity(scene, planets[i].entity);
        scene_add_entity(scene, planets[i].orbit_entity);
    }

    // Спутники.
    planets[2].sputniks[0] = calloc(1, sizeof(Planet));
    *planets[2].sputniks[0] = create_planet(0.1f, 1.0f, 30.0f, NULL); // Луна.
    planets[2].sputnik_count = 1;

    scene_add_entity(scene, planets[2].sputniks[0]->entity);
}

void update_solar_system(Scene *scene, f32 dt) {
    for (int i = 0; i < 8; i++) {
        f32 orbit = planets[i].orbit;
        f32 period = planets[i].orbit_period;
        f32 angle = planets[i].orbit_angle + (2.0 * M_PI * dt * 10.0f) / period;

        planets[i].orbit_angle = angle;
        planets[i].entity->position.x = orbit * cosf(angle);
        planets[i].entity->position.z = orbit * sinf(angle);
        planets[i].entity->position.y = 0.0f;

        for (int j = 0; j < planets[i].sputnik_count; j++) {
            Planet *sputnik = planets[i].sputniks[j];
            f32 orbit = sputnik->orbit;
            f32 period = sputnik->orbit_period;
            f32 angle = sputnik->orbit_angle + (2.0 * M_PI * dt * 1.0f) / period;
            sputnik->orbit_angle = angle;
            sputnik->entity->position.x = orbit * cosf(angle) + planets[i].entity->position.x;
            sputnik->entity->position.z = orbit * sinf(angle) + planets[i].entity->position.z;
            sputnik->entity->position.y = 0.0f;
        }
    }
}

void destroy_solar_system(Scene *scene) {
    mesh_destroy(sphere_mesh);
}

int main() {
    engine_init(SCREEN_WIDTH, SCREEN_HEIGHT, "My Mac Engine");

    // Create scene.
    Scene *scene = scene_create();
    engine_show_grid(true, scene);

    Camera *camera = camera_create((f32)SCREEN_WIDTH / SCREEN_HEIGHT);

    create_solar_system(scene);

    while (!engine_should_close()) {
        engine_poll_events();

        f32 dt = engine_get_dt();
        update_solar_system(scene, engine_get_dt());
        camera_update(camera, dt);
        input_update();

        engine_do_one_frame(scene, camera);
    }

    destroy_solar_system(scene);
    camera_destroy(camera);
    scene_destroy(scene);
    engine_shutdown();

    return 0;
}
