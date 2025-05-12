#include "camera.h"
#include "core/input.h"
#include "math/math.h"

#include <stdio.h>
#include <stdlib.h>

Camera *camera_create(f32 aspect) {
    Camera *camera = calloc(1, sizeof(Camera));
    if (!camera) {
        return NULL;
    }
    camera->position = vec3_zero();
    camera->rotation = quat_id();
    camera->target = vec3_zero();
    camera->up = (vec3){0.0f, 1.0f, 0.0f};
    camera->fov = 85.0f;
    camera->aspect = aspect;
    camera->near_clip = 0.1f;
    camera->far_clip = 1000.0f;

    return camera;
}

void camera_destroy(Camera *camera) {
    free(camera);
}

void camera_update(Camera *cam, f32 dt) {
    s16 dx = 0, dy = 0;
    input_get_mouse_delta(&dx, &dy);

    f32 sens = 0.002f;
    cam->yaw -= dx * sens;
    cam->pitch -= dy * sens;

    // Clamp pitch to [-pi/2, pi/2]
    if (cam->pitch < -1.57f) cam->pitch = -1.57f;
    if (cam->pitch > 1.57f) cam->pitch = 1.57f;

    quat q_pitch = quat_rotation_x(cam->pitch);
    quat q_yaw = quat_rotation_y(cam->yaw);
    cam->rotation = quat_mul(q_yaw, q_pitch);

    f32 speed = 10.0f;
    vec3 forward = quat_rotate_vec3(cam->rotation, vec3_new(0, 0, -1));
    vec3 right = quat_rotate_vec3(cam->rotation, vec3_new(1, 0, 0));
    vec3 up = quat_rotate_vec3(cam->rotation, vec3_new(0, 1, 0));
    if (input_is_key_down('w')) {
        cam->position = vec3_add(cam->position, vec3_mul(forward, speed * dt));
    } else if (input_is_key_down('s')) {
        cam->position = vec3_add(cam->position, vec3_mul(forward, -speed * dt));
    }

    if (input_is_key_down('a')) {
        cam->position = vec3_add(cam->position, vec3_mul(right, -speed * dt));
    } else if (input_is_key_down('d')) {
        cam->position = vec3_add(cam->position, vec3_mul(right, speed * dt));
    }

    if (input_is_key_down('e')) {
        cam->position = vec3_add(cam->position, vec3_mul(up, speed * dt));
    } else if (input_is_key_down('q')) {
        cam->position = vec3_add(cam->position, vec3_mul(up, -speed * dt));
    }

    mat4 rotation = mat4_from_quat(cam->rotation);
    // mat4 translate = mat4_translate(mat4_id(), cam->position);
    mat4 translate = mat4_translate(mat4_id(), vec3_new(-cam->position.x, -cam->position.y, -cam->position.z));
    // cam->view = mat4_mul(mat4_inverse(rotation), mat4_inverse(translate));
    cam->view = mat4_mul(rotation, translate);
}
