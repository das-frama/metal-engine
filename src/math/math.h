#pragma once

#include "defines.h"

#include <assert.h>
#include <float.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>

#define MAT4_AT(row, col) ((col) * 4 + (row))
#define DEG2RAD M_PI / 180.0f

typedef struct vec2_u {
    f32 elements[2];
    struct {
        f32 x, y;
    };
} __attribute__((aligned(16))) vec2;

typedef union vec3_u {
    f32 elements[3];
    struct {
        union {
            f32 x, r;
        };
        union {
            f32 y, g;
        };
        union {
            f32 z, b;
        };
    };
} __attribute__((__aligned__(16))) vec3;

typedef union vec4_u {
    f32 elements[4];
    struct {
        f32 x, y, z, w;
    };
} __attribute__((__aligned__(16))) vec4;

typedef struct {
    vec2 position;
    vec4 color;
} __attribute__((aligned(16))) vertex2d;

typedef struct vertex3d_u {
    vec3 position;
    vec3 normal;
    vec2 uv;
} __attribute__((aligned(16))) vertex3d;

typedef union mat4_u {
    f32 data[16];
} __attribute__((__aligned__(16))) mat4;

typedef vec4 quat;

typedef struct {
    f32 w, h;
    f32 x, y;
} rect;

inline vec2 vec2_zero() {
    return (vec2){0.0f, 0.0f};
}
inline vec2 vec2_new(f32 x, f32 y) {
    return (vec2){.x = x, .y = y};
}
inline vec2 vec2_add(vec2 a, vec2 b) {
    return (vec2){a.x + b.x, a.y + b.y};
}
inline f32 vec2_dot(vec2 a, vec2 b) {
    return a.x * b.x + a.y * b.y;
}
inline vec3 vec3_new(f32 x, f32 y, f32 z) {
    return (vec3){x, y, z};
}
inline vec3 vec3_zero() {
    return (vec3){0.0f, 0.0f, 0.0f};
}
inline vec3 vec3_one() {
    return (vec3){1.0f, 1.0f, 1.0f};
}
inline f32 vec3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline vec3 vec3_mul(vec3 v, f32 m) {
    return (vec3){v.x * m, v.y * m, v.z * m};
}
inline vec3 vec3_div(vec3 v, f32 d) {
    if (d == 0.0f) return vec3_zero();
    return (vec3){v.x / d, v.y / d, v.z / d};
}
inline vec3 vec3_add(vec3 a, vec3 b) {
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}
inline vec3 vec3_sub(vec3 a, vec3 b) {
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}
inline f32 vec3_length(vec3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline vec3 vec3_normalize(vec3 v) {
    f32 len = vec3_length(v);
    if (len == 0.0f) return vec3_zero();
    else
        return vec3_div(v, len);
}
inline vec4 vec4_new(f32 x, f32 y, f32 z, f32 w) {
    return (vec4){x, y, z, w};
}
inline vec4 vec4_one() {
    return (vec4){1.0f, 1.0f, 1.0f, 1.0f};
}
inline vec4 vec4_zero() {
    return (vec4){0.0f, 0.0f, 0.0f, 0.0f};
}

inline vec4 vec4_cross(vec4 v1, vec4 v2, vec4 v3) {
    vec4 v = {0};
    v.x = v1.y * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.y * v3.w - v3.y * v2.w) + v1.w * (v2.y * v3.z - v2.z * v3.y);
    v.y =
        -(v1.x * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.z - v3.x * v2.z));
    v.z = v1.x * (v2.y * v3.w - v3.y * v2.w) - v1.y * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.y - v3.x * v2.y);
    v.w =
        -(v1.x * (v2.y * v3.z - v3.y * v2.z) - v1.y * (v2.x * v3.z - v3.x * v2.z) + v1.z * (v2.x * v3.y - v3.x * v2.y));

    return v;
}

inline mat4 mat4_zero() {
    mat4 m = {0};
    memset(&m, 0, sizeof(f32) * 16);

    return m;
}

inline mat4 mat4_id() {
    mat4 out = mat4_zero();
    out.data[MAT4_AT(0, 0)] = 1.0f;
    out.data[MAT4_AT(1, 1)] = 1.0f;
    out.data[MAT4_AT(2, 2)] = 1.0f;
    out.data[MAT4_AT(3, 3)] = 1.0f;
    return out;
}

inline mat4 mat4_mul(mat4 m1, mat4 m2) {
    mat4 out = mat4_zero();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            out.data[MAT4_AT(i, j)] =
                m1.data[MAT4_AT(i, 0)] * m2.data[MAT4_AT(0, j)] + m1.data[MAT4_AT(i, 1)] * m2.data[MAT4_AT(1, j)] +
                m1.data[MAT4_AT(i, 2)] * m2.data[MAT4_AT(2, j)] + m1.data[MAT4_AT(i, 3)] * m2.data[MAT4_AT(3, j)];
        }
    }

    return out;
}

inline mat4 mat4_transponse(mat4 m) {
    mat4 out = mat4_zero();
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            out.data[MAT4_AT(row, col)] = m.data[MAT4_AT(col, row)];
        }
    }
    return out;
}

inline mat4 mat4_scale(vec3 v) {
    mat4 out = mat4_id();
    out.data[MAT4_AT(0, 0)] = v.x;
    out.data[MAT4_AT(1, 1)] = v.y;
    out.data[MAT4_AT(2, 2)] = v.z;
    return out;
}
inline mat4 mat4_scale_by(f32 f) {
    return mat4_scale((vec3){f, f, f});
}

inline mat4 mat4_translate(mat4 m, vec3 v) {
    mat4 out = mat4_id();
    out.data[MAT4_AT(0, 3)] = v.x;
    out.data[MAT4_AT(1, 3)] = v.y;
    out.data[MAT4_AT(2, 3)] = v.z;

    return mat4_mul(out, m);
}

inline mat4 mat4_rotate_x(f32 x) {
    mat4 out = mat4_id();
    f32 c = cosf(x);
    f32 s = sinf(x);

    out.data[MAT4_AT(1, 1)] = c;
    out.data[MAT4_AT(1, 2)] = s;
    out.data[MAT4_AT(2, 1)] = -s;
    out.data[MAT4_AT(2, 2)] = c;

    return out;
}

inline mat4 mat4_rotate_y(f32 y) {
    mat4 out = mat4_id();
    f32 c = cosf(y);
    f32 s = sinf(y);

    out.data[MAT4_AT(0, 0)] = c;
    out.data[MAT4_AT(0, 2)] = s;
    out.data[MAT4_AT(2, 0)] = -s;
    out.data[MAT4_AT(2, 2)] = c;

    return out;
}

inline mat4 mat4_rotate_z(f32 z) {
    mat4 out = mat4_id();
    f32 c = cosf(z);
    f32 s = sinf(z);

    out.data[MAT4_AT(0, 0)] = c;
    out.data[MAT4_AT(0, 1)] = s;
    out.data[MAT4_AT(1, 0)] = -s;
    out.data[MAT4_AT(1, 1)] = c;

    return out;
}

inline f32 mat4_det(mat4 m) {
    vec4 v1 = (vec4){
        m.data[MAT4_AT(0, 0)],
        m.data[MAT4_AT(1, 0)],
        m.data[MAT4_AT(2, 0)],
        m.data[MAT4_AT(3, 0)],
    };
    vec4 v2 = (vec4){
        m.data[MAT4_AT(0, 1)],
        m.data[MAT4_AT(1, 1)],
        m.data[MAT4_AT(2, 1)],
        m.data[MAT4_AT(3, 1)],
    };
    vec4 v3 = (vec4){
        m.data[MAT4_AT(0, 2)],
        m.data[MAT4_AT(1, 2)],
        m.data[MAT4_AT(2, 2)],
        m.data[MAT4_AT(3, 2)],
    };
    vec4 minor = vec4_cross(v1, v2, v3);

    return -(m.data[MAT4_AT(0, 3)] * minor.x + m.data[MAT4_AT(1, 3)] * minor.y + m.data[MAT4_AT(2, 3)] * minor.z +
             m.data[MAT4_AT(3, 3)] * minor.w);
}

inline mat4 mat4_inverse(mat4 m) {
    mat4 out = mat4_zero();
    vec4 v = vec4_zero(), vec[3] = {0};

    f32 det = mat4_det(m);
    if (det == 0.0f) return m;

    int a;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (j != i) {
                a = j;
                if (j > i) a = a - 1;
                vec[a].x = m.data[MAT4_AT(j, 0)];
                vec[a].y = m.data[MAT4_AT(j, 1)];
                vec[a].z = m.data[MAT4_AT(j, 2)];
                vec[a].w = m.data[MAT4_AT(j, 3)];
            }
        }
        v = vec4_cross(vec[0], vec[1], vec[2]);

        out.data[MAT4_AT(0, i)] = (f32)pow(-1.0f, i) * v.x / det;
        out.data[MAT4_AT(1, i)] = (f32)pow(-1.0f, i) * v.y / det;
        out.data[MAT4_AT(2, i)] = (f32)pow(-1.0f, i) * v.z / det;
        out.data[MAT4_AT(3, i)] = (f32)pow(-1.0f, i) * v.w / det;
    }

    return out;
}

inline mat4 mat4_perspective(f32 fov, f32 aspect, f32 near, f32 far) {
    assert(near != far && fov > 0 && aspect > 0);

    mat4 out = mat4_zero();
    f32 f = 1.0f / tanf(fov * 0.5f);

    out.data[MAT4_AT(0, 0)] = f / aspect;
    out.data[MAT4_AT(1, 1)] = f;
    out.data[MAT4_AT(2, 2)] = (far + near) / (near - far);
    out.data[MAT4_AT(2, 3)] = 2.0f * far * near / (near - far);
    out.data[MAT4_AT(3, 2)] = -1.0f;

    return out;
}

inline mat4 mat4_mvp(mat4 proj, mat4 view, mat4 model) {
    return mat4_mul(mat4_mul(proj, view), model);
}

inline mat4 mat4_mul_x(int n, ...) {
    assert(n >= 2);

    va_list args;
    va_start(args, n);

    mat4 m1 = va_arg(args, mat4);
    mat4 m2 = va_arg(args, mat4);
    mat4 m = mat4_mul(m1, m2);
    for (int i = 0; i < n - 2; i++) {
        mat4 temp = va_arg(args, mat4);
        m = mat4_mul(m, temp);
    }

    va_end(args);
    return m;
}

inline mat4 mat4_from_quat(quat q) {
    f32 x2 = q.x + q.x;
    f32 y2 = q.y + q.y;
    f32 z2 = q.z + q.z;

    f32 xx = q.x * x2;
    f32 yy = q.y * y2;
    f32 zz = q.z * z2;
    f32 xy = q.x * y2;
    f32 xz = q.x * z2;
    f32 yz = q.y * z2;
    f32 wx = q.w * x2;
    f32 wy = q.w * y2;
    f32 wz = q.w * z2;

    mat4 m = {0};
    m.data[MAT4_AT(0, 0)] = 1.0f - (yy + zz);
    m.data[MAT4_AT(0, 1)] = xy + wz;
    m.data[MAT4_AT(0, 2)] = xz - wy;
    m.data[MAT4_AT(0, 3)] = 0.0f;

    m.data[MAT4_AT(1, 0)] = xy - wz;
    m.data[MAT4_AT(1, 1)] = 1.0f - (xx + zz);
    m.data[MAT4_AT(1, 2)] = yz + wx;
    m.data[MAT4_AT(1, 3)] = 0.0f;

    m.data[MAT4_AT(2, 0)] = xz + wy;
    m.data[MAT4_AT(2, 1)] = yz - wx;
    m.data[MAT4_AT(2, 2)] = 1.0f - (xx + yy);
    m.data[MAT4_AT(2, 3)] = 0.0f;

    m.data[MAT4_AT(3, 0)] = 0.0f;
    m.data[MAT4_AT(3, 1)] = 0.0f;
    m.data[MAT4_AT(3, 2)] = 0.0f;
    m.data[MAT4_AT(3, 3)] = 1.0f;

    return m;
}

mat4 model_matrix(vec3 pos, quat rotation, vec3 scale);

void mat4_print(mat4 m);

inline quat quat_new(f32 x, f32 y, f32 z, f32 w) {
    quat q = {x, y, z, w};
    return q;
}

inline quat quat_id(void) {
    return quat_new(0, 0, 0, 1);
}

inline f32 quat_length(quat q) {
    return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

inline quat quat_normalize(quat q) {
    f32 scale = quat_length(q);

    if (scale > FLT_EPSILON) {
        return quat_new(q.x / scale, q.y / scale, q.z / scale, q.w / scale);
    } else {
        return quat_new(0, 0, 0, 0);
    }
}

inline quat quat_angle_axis(f32 angle, vec3 axis) {
    f32 sine = sinf(angle / 2.0f);
    f32 cosine = cosf(angle / 2.0f);

    return quat_normalize(quat_new(axis.x * sine, axis.y * sine, axis.z * sine, cosine));
}

inline quat quat_rotation_x(f32 angle) {
    return quat_angle_axis(angle, vec3_new(1, 0, 0));
}
inline quat quat_rotation_y(f32 angle) {
    return quat_angle_axis(angle, vec3_new(0, 1, 0));
}
inline quat quat_rotation_z(f32 angle) {
    return quat_angle_axis(angle, vec3_new(0, 0, 1));
}

inline quat quat_mul(quat q1, quat q2) {
    return quat_new((q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y),
                    (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x),
                    (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w),
                    (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z));
}

inline vec3 quat_rotate_vec3(quat q, vec3 v) {
    q = quat_normalize(q);
    quat v_quat = quat_new(v.x, v.y, v.z, 0.0f);
    quat q_inv = quat_new(-q.x, -q.y, -q.z, q.w);
    quat rotated = quat_mul(quat_mul(q, v_quat), q_inv);

    return vec3_new(rotated.x, rotated.y, rotated.z);
}
