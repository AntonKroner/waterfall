#ifndef BindGroup_H
#define BindGroup_H
#include "webgpu.h"
#include "./TTexture.h"

WGPUBindGroup BindGroup_make(
  WGPUDevice device,
  WGPUBindGroupLayout bindGroupLayout,
  Texture texture,
  WGPUBuffer lightningBuffer,
  size_t lightningBufferSize,
  WGPUBuffer uniformBuffer,
  size_t uniformBufferSize) {
  WGPUBindGroupEntry bindings[] = {
    {
     .nextInChain = 0,
     .binding = 0,
     .buffer = uniformBuffer,
     .offset = 0,
     .size = uniformBufferSize,
     },
    {
     .nextInChain = 0,
     .binding = 1,
     .textureView = texture.view,
     },
    {
     .nextInChain = 0,
     .binding = 2,
     .sampler = texture.sampler,
     },
    {
     .nextInChain = 0,
     .binding = 3,
     .buffer = lightningBuffer,
     .offset = 0,
     .size = lightningBufferSize,
     }
  };
  WGPUBindGroupDescriptor bindGroupDescriptor = {
    .nextInChain = 0,
    .layout = bindGroupLayout,
    .entryCount = 4,
    .entries = bindings,
  };
  return wgpuDeviceCreateBindGroup(device, &bindGroupDescriptor);
}

#endif // BindGroup_H
