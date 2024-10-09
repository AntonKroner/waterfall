#ifndef Player_H_
#define Player_H_
#include <stdlib.h>
#include "webgpu.h"
#include "linear/algebra.h"
#include "./RenderTarget.h"

typedef struct {
    Vector3f position;
} Player_Uniforms;
typedef RenderTarget_EXTEND({ Vector3f position; }) Player;

Player* Player_Create(
  Player* result,
  WGPUDevice device,
  WGPUQueue queue,
  WGPUTextureFormat depthFormat,
  WGPUBuffer lightningBuffer,
  size_t lightningBufferSize,
  WGPUBuffer uniformBuffer,
  size_t uniformBufferSize,
  Vector3f offset,
  Vector3f position) {
  if (result || (result = calloc(1, sizeof(*result)))) {
    RenderTarget_create(
      &result->super,
      device,
      queue,
      depthFormat,
      lightningBuffer,
      lightningBufferSize,
      uniformBuffer,
      uniformBufferSize,
      offset,
      RESOURCE_DIR "/player/player.wgsl",
      RESOURCE_DIR "/mammoth/mammoth.obj",
      RESOURCE_DIR "/fourareen/fourareen2K_albedo.jpg");
    result->position = position;
  }
  return result;
}
void Player_move(Player* player, Vector3f direction) {
  const float speed = 0.1f;
  player->position = Vector3f_add(player->position, Vector_scale(speed, direction));
}

#endif // Player_H_
