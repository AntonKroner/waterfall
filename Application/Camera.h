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

void Application_Camera_initiate(
  Camera camera[static 1],
  const float phi,
  const float theta);
Matrix4f Application_Camera_viewGet(Camera camera);
void Application_Camera_rotate(Camera camera[static 1], const float phi);
void Application_Camera_moveTarget(Camera camera[static 1], Vector3f direction);
void Application_Camera_zoom(Camera camera[static 1], float x, float y);

Matrix4f Application_Camera_viewGet(Camera camera) {
  return Matrix4f_lookAt(
    Vector_scale(camera.zoom, camera.position),
    camera.target,
    Vector3f_make(0, 0, 1.0f));
}
const float sensitivity = 0.1f;
const float scrollSensitivity = 0.1f;
static double clamp(double value) {
  return fabs(value) > 2 * M_PI ? 0 : value;
}
void Application_Camera_initiate(
  Camera camera[static 1],
  const float phi,
  const float theta) {
  camera->angles.components[0] = clamp(theta);
  camera->angles.components[1] = clamp(phi);
  const float cosTheta = cos(camera->angles.components[0]);
  const float sinTheta = sin(camera->angles.components[0]);
  const float cosPhi = cos(camera->angles.components[1]);
  const float sinPhi = sin(camera->angles.components[1]);
  camera->position = Vector_add(
    camera->target,
    Vector_scale(1, Vector3f_make(cosTheta * cosPhi, sinPhi * cosTheta, sinTheta)));
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
    camera->target,
    Vector_scale(norm, Vector3f_make(cosTheta * cosPhi, sinPhi * cosTheta, sinTheta)));
}
void Application_Camera_moveTarget(Camera camera[static 1], Vector3f direction) {
  camera->target = Vector_add(camera->target, Vector_scale(sensitivity, direction));
  camera->position = Vector_add(camera->position, Vector_scale(sensitivity, direction));
}
void Application_Camera_zoom(Camera camera[static 1], float /*x*/, float y) {
  camera->zoom += scrollSensitivity * -y;
  camera->zoom = fmax(0.001, fmin(10.0, camera->zoom));
}

#endif // Camera_H_
