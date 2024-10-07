#ifndef Camera_H_
#define Camera_H_
#include <stdbool.h>
#include <tgmath.h>
#include "webgpu.h"
#include "linear/algebra.h"

typedef struct {
    Vector3f target;
    Vector3f position;
    Vector2f angles;
    float zoom;
    bool dragging;
} Camera;

Matrix4f Application_Camera_viewGet(Camera camera);
void Application_Camera_rotate(Camera camera[static 1], const float phi);
void Application_Camera_moveTarget(Camera camera[static 1], Vector3f direction);
void Application_Camera_zoom(Camera camera[static 1], float x, float y);

Matrix4f Application_Camera_viewGet(Camera camera) {
  return Matrix4f_lookAt(camera.position, camera.target, Vector3f_make(0, 0, 1.0f));
}
const float sensitivity = 0.01f;
const float scrollSensitivity = 0.1f;
static double clamp(double value) {
  return fabs(value) > 2 * M_PI ? 0 : value;
}
void Application_Camera_rotate(Camera camera[static 1], const float phi) {
  camera->angles.components[0] = clamp(camera->angles.components[0]);
  camera->angles.components[1] = clamp(camera->angles.components[1] + sensitivity * phi);
  const double norm = Vector_norm(Vector_subtract(camera->position, camera->target));
  const float cosTheta = cos(camera->angles.components[0]);
  const float sinTheta = sin(camera->angles.components[0]);
  const float cosPhi = cos(camera->angles.components[1]);
  const float sinPhi = sin(camera->angles.components[1]);
  camera->position = Vector_add(
    Vector_scale(norm, Vector3f_make(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta)),
    camera->target);
}
void Application_Camera_moveTarget(Camera camera[static 1], Vector3f direction) {
  camera->target.components[0] += sensitivity * direction.components[0];
  camera->target.components[1] += sensitivity * direction.components[1];
  camera->target.components[2] += sensitivity * direction.components[2];
}
void Application_Camera_zoom(Camera camera[static 1], float /*x*/, float y) {
  camera->zoom += scrollSensitivity * y;
  float cx = cos(camera->angles.components[0]);
  float sx = sin(camera->angles.components[0]);
  float cy = cos(camera->angles.components[1]);
  float sy = sin(camera->angles.components[1]);
  camera->position =
    Vector_scale(exp(-camera->zoom), Vector3f_make(cx * cy, sx * cy, sy));
}

#endif // Camera_H_
