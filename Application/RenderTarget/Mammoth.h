#ifndef Mammoth_H_
#define Mammoth_H_
#include <stdlib.h>
#include "webgpu.h"
#include "linear/algebra.h"
#include "./RenderTarget.h"

typedef RenderTarget_EXTEND({ int placeholder; }) Mammoth;

Mammoth* Mammoth_Create(
  Mammoth* result,
  WGPUDevice device,
  WGPUQueue queue,
  WGPUTextureFormat depthFormat,
  WGPUBindGroupLayout globalBindGroup,
  Vector3f offset) {
  if (result || (result = calloc(1, sizeof(*result)))) {
    RenderTarget_create(
      &result->super,
      device,
      queue,
      depthFormat,
      globalBindGroup,
      offset,
      RESOURCE_DIR "/lightning/specularity.wgsl",
      RESOURCE_DIR "/mammoth/mammoth.obj",
      RESOURCE_DIR "/fourareen/fourareen2K_albedo.jpg");
  }
  return result;
}

#endif // Mammoth_H_
