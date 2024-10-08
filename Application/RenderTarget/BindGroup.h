#ifndef BindGroup_H
#define BindGroup_H
#include "webgpu.h"

WGPUBindGroup BindGroup_create(WGPUDevice device) {
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
     .textureView = result->texture.view,
     },
    {
     .nextInChain = 0,
     .binding = 2,
     .sampler = result->texture.sampler,
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
    .entryCount = bindGroupLayoutDescriptor.entryCount,
    .entries = bindings,
  };
  return wgpuDeviceCreateBindGroup(device, &bindGroupDescriptor);
}

#endif // BindGroup_H
