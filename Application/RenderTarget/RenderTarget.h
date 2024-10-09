#ifndef RenderTarget_H_
#define RenderTarget_H_
#include <stdlib.h>
#include "webgpu.h"
#include "linear/algebra.h"
#include "../device.h"
#include "./Model.h"
#include "./Texture.h"
#include "./BindGroup.h"
#include "./BindGroupLayout.h"
#include "./Pipeline.h"

typedef struct {
    WGPUShaderModule shader;
    Texture texture;
    struct {
        WGPUBuffer buffer;
        size_t count;
    } vertex;
    WGPURenderPipeline pipeline;
    WGPUBindGroup bindGroup;
} RenderTarget;

#define RenderTarget_EXTEND(T) \
  struct {                     \
      struct {                 \
          RenderTarget super;  \
      };                       \
      struct T;                \
  }

static void buffers_attach(
  RenderTarget target[static 1],
  WGPUDevice device,
  size_t vertexCount) {
  WGPUBufferDescriptor descriptor = {
    .nextInChain = 0,
    .label = "vertex buffer",
    .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
    .mappedAtCreation = false,
    .size = vertexCount * sizeof(Model_Vertex),
  };
  target->vertex.buffer = wgpuDeviceCreateBuffer(device, &descriptor);
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
  WGPUBuffer lightningBuffer,
  size_t lightningBufferSize,
  WGPUBuffer uniformBuffer,
  size_t uniformBufferSize,
  Vector3f offset,
  const char* const shaderPath,
  const char* const modelPath,
  const char* const texturePath) {
  if (result || (result = calloc(1, sizeof(*result)))) {
    result->shader = Application_device_ShaderModule(device, shaderPath);
    result->texture = Texture_create(device, texturePath);
    Model model = Model_load(modelPath, offset);
    result->vertex.count = model.vertexCount;
    buffers_attach(result, device, result->vertex.count);
    wgpuQueueWriteBuffer(
      queue,
      result->vertex.buffer,
      0,
      model.vertices,
      result->vertex.count * sizeof(Model_Vertex));
    Model_unload(&model);
    // pipeline
    WGPUBindGroupLayout bindGroupLayout =
      BindGroupLayout_make(device, lightningBufferSize, uniformBufferSize);
    result->pipeline = Pipeline_make(
      device,
      result->shader,
      lightningBufferSize,
      uniformBufferSize,
      depthFormat);
    // bind group
    result->bindGroup = BindGroup_make(
      device,
      bindGroupLayout,
      result->texture,
      lightningBuffer,
      lightningBufferSize,
      uniformBuffer,
      uniformBufferSize);
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
  wgpuRenderPassEncoderSetBindGroup(renderPass, 0, target->bindGroup, 0, 0);
  wgpuRenderPassEncoderDraw(renderPass, target->vertex.count, 1, 0, 0);
}

#endif // RenderTarget_H_
