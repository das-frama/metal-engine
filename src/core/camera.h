#pragma once

#include "math/math.h"

typedef struct Camera {
   vec3 position;
   vec3 target;
   vec3 up;
   quat rotation;
   f32 fov;
   f32 aspect;
   f32 near_clip;
   f32 far_clip;
   mat4 view;
   f32 yaw, pitch;
} Camera;

Camera *camera_create(f32 aspect);
void camera_destroy(Camera *camera);

void camera_update(Camera *camera, f32 dt);
