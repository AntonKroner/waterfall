#ifndef Texture_H_
#define Texture_H_
#include "webgpu.h"
#include "../device.h"

typedef struct {
    WGPUSampler sampler;
    WGPUTexture texture;
    WGPUTextureView view;
} Texture;

Texture Texture_create(WGPUDevice device, const char* path);
void Texture_destroy(Texture texture);

Texture Texture_create(WGPUDevice device, const char* path) {
  Texture result = { 0 };
  result.texture = Application_device_Texture_load(device, path, &result.view);
  WGPUSamplerDescriptor samplerDescriptor = {
    .addressModeU = WGPUAddressMode_ClampToEdge,
    .addressModeV = WGPUAddressMode_ClampToEdge,
    .addressModeW = WGPUAddressMode_ClampToEdge,
    .magFilter = WGPUFilterMode_Linear,
    .minFilter = WGPUFilterMode_Linear,
    .mipmapFilter = WGPUMipmapFilterMode_Linear,
    .lodMinClamp = 0.0f,
    .lodMaxClamp = 1.0f,
    .compare = WGPUCompareFunction_Undefined,
    .maxAnisotropy = 1,
  };
  result.sampler = wgpuDeviceCreateSampler(device, &samplerDescriptor);
  return result;
}
void Texture_destroy(Texture texture) {
  wgpuTextureDestroy(texture.texture);
  wgpuTextureRelease(texture.texture);
  wgpuTextureViewRelease(texture.view);
  wgpuSamplerRelease(texture.sampler);
}

#endif // Texture_H_
