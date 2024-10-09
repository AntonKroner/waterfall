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
WGPUBindGroupLayout BindGroupLayout_make(
  WGPUDevice device,
  size_t lightningBufferSize,
  size_t uniformBufferSize) {
  WGPUBindGroupLayoutEntry bindingLayouts[] = {
    BindGroupLayoutEntry_make(),
    BindGroupLayoutEntry_make(),
    BindGroupLayoutEntry_make(),
    BindGroupLayoutEntry_make(),
  };
  bindingLayouts[0].buffer.type = WGPUBufferBindingType_Uniform;
  bindingLayouts[0].buffer.minBindingSize = uniformBufferSize;
  bindingLayouts[1].binding = 1;
  bindingLayouts[1].visibility = WGPUShaderStage_Fragment;
  bindingLayouts[1].texture.sampleType = WGPUTextureSampleType_Float;
  bindingLayouts[1].texture.viewDimension = WGPUTextureViewDimension_2D;
  bindingLayouts[2].binding = 2;
  bindingLayouts[2].visibility = WGPUShaderStage_Fragment;
  bindingLayouts[2].sampler.type = WGPUSamplerBindingType_Filtering;
  bindingLayouts[3].binding = 3;
  bindingLayouts[3].visibility = WGPUShaderStage_Fragment;
  bindingLayouts[3].buffer.type = WGPUBufferBindingType_Uniform;
  bindingLayouts[3].buffer.minBindingSize = lightningBufferSize;
  WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {
    .nextInChain = 0,
    .entryCount = 4,
    .entries = bindingLayouts,
  };
  return wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDescriptor);
}

#endif // BindGroupLayout_H_
