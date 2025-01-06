#ifndef Uniform_H_
#define Uniform_H_
// #include <stdio.h>
#include <stdbool.h>
#include "webgpu.h"
#include "linear/algebra.h"

typedef struct {
    struct {
        Matrix4f projection;
        Matrix4f view;
        Matrix4f model;
    } matrices;
    Vector4f color;
    Vector3f cameraPosition;
    float time;
} Uniform_Global;

#endif // Uniform_H_
