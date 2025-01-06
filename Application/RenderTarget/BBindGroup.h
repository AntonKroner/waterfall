#ifndef BindGroup_H
#define BindGroup_H
#include "webgpu.h"
#include "./TTexture.h"

WGPUBindGroup BindGroup_make(
  WGPUDevice device,
  WGPUBindGroupLayout bindGroupLayout,
  Texture texture,
  WGPUBuffer uniformBuffer,
  size_t uniformSize) {
  WGPUBindGroupEntry bindings[] = {
    {
     .nextInChain = 0,
     .binding = 0,
     .textureView = texture.view,
     },
    {
     .nextInChain = 0,
     .binding = 1,
     .sampler = texture.sampler,
     },
    {
     .nextInChain = 0,
     .binding = 2,
     .buffer = uniformBuffer,
     .offset = 0,
     .size = uniformSize,
     }
  };
  WGPUBindGroupDescriptor bindGroupDescriptor = {
    .nextInChain = 0,
    .layout = bindGroupLayout,
    .entryCount = 3,
    .entries = bindings,
  };
  return wgpuDeviceCreateBindGroup(device, &bindGroupDescriptor);
}
WGPUBindGroup BindGroup_globalMake(
  WGPUDevice device,
  WGPUBindGroupLayout bindGroupLayout,
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
     .buffer = lightningBuffer,
     .offset = 0,
     .size = lightningBufferSize,
     }
  };
  WGPUBindGroupDescriptor bindGroupDescriptor = {
    .nextInChain = 0,
    .layout = bindGroupLayout,
    .entryCount = 2,
    .entries = bindings,
  };
  return wgpuDeviceCreateBindGroup(device, &bindGroupDescriptor);
}

#endif // BindGroup_H
