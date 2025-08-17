#ifndef RenderTarget_H_
#define RenderTarget_H_
#include <stdlib.h>
#include "webgpu.h"
#include "linear/algebra.h"
#include "../device.h"
#include "./Model.h"
#include "./TTexture.h"
#include "./BBindGroup.h"
#include "./BBindGroupLayout.h"
#include "./PPipeline.h"

typedef struct {
    WGPUShaderModule shader;
    Texture texture;
    struct {
        WGPUBuffer buffer;
        size_t count;
    } vertex;
    struct {
        WGPUBuffer buffer;
        size_t size;
    } uniform;
    WGPUBindGroup bindGroup;
    WGPURenderPipeline pipeline;
} RenderTarget;

#define RenderTarget_EXTEND(T) \
  struct {                     \
      struct {                 \
          RenderTarget super;  \
      };                       \
      struct T;                \
  }

static void buffers_detach(RenderTarget target[static 1]) {
  wgpuBufferDestroy(target->vertex.buffer);
  wgpuBufferRelease(target->vertex.buffer);
}
RenderTarget* RenderTarget_create(
  RenderTarget* result,
  WGPUDevice device,
  WGPUQueue queue,
  WGPUTextureFormat depthFormat,
  WGPUBindGroupLayout globalBindGroup,
  size_t uniformSize,
  WGPUBuffer uniformBuffer,
  Vector3f offset,
  const char* const shaderPath,
  const char* const modelPath,
  const char* const texturePath) {
  if (result || (result = calloc(1, sizeof(*result)))) {
    result->shader = Application_device_ShaderModule(device, shaderPath);
    result->texture = Texture_create(device, texturePath);
    result->vertex.buffer =
      Model_bufferCreate(device, queue, modelPath, offset, &result->vertex.count);
    result->uniform.buffer = uniformBuffer;
    result->uniform.size = uniformSize;
    WGPUBindGroupLayout bindGroupLayouts[] = {
      globalBindGroup,
      BindGroupLayout_make(device, uniformSize),
    };
    result->bindGroup = BindGroup_make(
      device,
      bindGroupLayouts[1],
      result->texture,
      uniformBuffer,
      uniformSize);
    result->pipeline =
      Pipeline_make(device, result->shader, 2, bindGroupLayouts, depthFormat);
  }
  return result;
}
void RenderTarget_destroy(RenderTarget* target) {
  buffers_detach(target);
  Texture_destroy(target->texture);
  wgpuBindGroupRelease(target->bindGroup);
  wgpuRenderPipelineRelease(target->pipeline);
  wgpuShaderModuleRelease(target->shader);
  free(target);
}
void RenderTarget_render(RenderTarget target[static 1], WGPURenderPassEncoder renderPass) {
  wgpuRenderPassEncoderSetPipeline(renderPass, target->pipeline);
  wgpuRenderPassEncoderSetVertexBuffer(
    renderPass,
    0,
    target->vertex.buffer,
    0,
    target->vertex.count * sizeof(Model_Vertex));
  wgpuRenderPassEncoderSetBindGroup(renderPass, 1, target->bindGroup, 0, 0);
  wgpuRenderPassEncoderDraw(renderPass, target->vertex.count, 1, 0, 0);
}

#endif // RenderTarget_H_
