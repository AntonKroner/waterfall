#ifndef Player_H_
#define Player_H_
#include <stdlib.h>
#include "webgpu.h"
#include "linear/algebra.h"
#include "./RenderTarget.h"

typedef struct {
    Vector3f position;
    float _pad;
} Player_Uniforms;
typedef RenderTarget_EXTEND({ Vector3f position; }) Player;

Player* Player_Create(
  Player* result,
  WGPUDevice device,
  WGPUQueue queue,
  WGPUTextureFormat depthFormat,
  WGPUBindGroupLayout globalBindGroup,
  Vector3f offset,
  Vector3f position) {
  if (result || (result = calloc(1, sizeof(*result)))) {
    WGPUBufferDescriptor descriptor = {
      .nextInChain = 0,
      .label = "Player uniform buffer",
      .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
      .mappedAtCreation = false,
      .size = sizeof(Player_Uniforms),
    };
    RenderTarget_create(
      &result->super,
      device,
      queue,
      depthFormat,
      globalBindGroup,
      descriptor.size,
      wgpuDeviceCreateBuffer(device, &descriptor),
      offset,
      RESOURCE_DIR "/player/player.wgsl",
      RESOURCE_DIR "/mammoth/mammoth.obj",
      RESOURCE_DIR "/fourareen/fourareen2K_albedo.jpg");
    result->position = position;
  }
  return result;
}
void Player_move(Player player[static 1], WGPUQueue queue, const Vector3f direction) {
  const float speed = 0.1f;
  player->position = Vector_add(player->position, Vector_scale(speed, direction));
  wgpuQueueWriteBuffer(
    queue,
    player->super.uniform.buffer,
    0,
    &player->position,
    sizeof(Player_Uniforms));
}

#endif // Player_H_
