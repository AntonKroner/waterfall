#ifndef BindGroupLayout_H_
#define BindGroupLayout_H_
#include "webgpu.h"

static WGPUBindGroupLayoutEntry BindGroupLayoutEntry_make() {
  WGPUBindGroupLayoutEntry result = {
    .buffer.nextInChain = 0,
    .buffer.type = WGPUBufferBindingType_Undefined,
    .buffer.minBindingSize = 0,
    .buffer.hasDynamicOffset = false,
    .sampler.nextInChain = 0,
    .sampler.type = WGPUSamplerBindingType_Undefined,
    .storageTexture.nextInChain = 0,
    .storageTexture.access = WGPUStorageTextureAccess_Undefined,
    .storageTexture.format = WGPUTextureFormat_Undefined,
    .storageTexture.viewDimension = WGPUTextureViewDimension_Undefined,
    .texture.nextInChain = 0,
    .texture.multisampled = false,
    .texture.sampleType = WGPUTextureSampleType_Undefined,
    .texture.viewDimension = WGPUTextureViewDimension_Undefined,
    .binding = 0,
    .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
  };
  return result;
}
WGPUBindGroupLayout BindGroupLayout_make(WGPUDevice device, size_t uniformSize) {
  WGPUBindGroupLayoutEntry bindingLayouts[3] = {
    BindGroupLayoutEntry_make(),
    BindGroupLayoutEntry_make(),
    BindGroupLayoutEntry_make(),
  };
  bindingLayouts[0].binding = 0;
  bindingLayouts[0].visibility = WGPUShaderStage_Fragment;
  bindingLayouts[0].texture.sampleType = WGPUTextureSampleType_Float;
  bindingLayouts[0].texture.viewDimension = WGPUTextureViewDimension_2D;
  bindingLayouts[1].binding = 1;
  bindingLayouts[1].visibility = WGPUShaderStage_Fragment;
  bindingLayouts[1].sampler.type = WGPUSamplerBindingType_Filtering;
  bindingLayouts[2].binding = 2;
  bindingLayouts[2].buffer.type = WGPUBufferBindingType_Uniform;
  bindingLayouts[2].buffer.minBindingSize = uniformSize;
  WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {
    .nextInChain = 0,
    .entryCount = 3,
    .entries = bindingLayouts,
  };

  return wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDescriptor);
}
WGPUBindGroupLayout BindGroupLayout_globalMake(
  WGPUDevice device,
  size_t lightningBufferSize,
  size_t uniformBufferSize) {
  WGPUBindGroupLayoutEntry bindingLayouts[2] = {
    BindGroupLayoutEntry_make(),
    BindGroupLayoutEntry_make(),
  };
  bindingLayouts[0].binding = 0;
  bindingLayouts[0].buffer.type = WGPUBufferBindingType_Uniform;
  bindingLayouts[0].buffer.minBindingSize = uniformBufferSize;
  bindingLayouts[1].binding = 1;
  bindingLayouts[1].visibility = WGPUShaderStage_Fragment;
  bindingLayouts[1].buffer.type = WGPUBufferBindingType_Uniform;
  bindingLayouts[1].buffer.minBindingSize = lightningBufferSize;
  WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {
    .nextInChain = 0,
    .entryCount = 2,
    .entries = bindingLayouts,
  };

  return wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDescriptor);
}

#endif // BindGroupLayout_H_
