#ifndef Pipeline_H
#define Pipeline_H
#include "webgpu.h"
#include "./DepthStencilState.h"
#include "./BindGroupLayout.h"
#include "./Model.h"

WGPURenderPipeline Pipeline_make(
  WGPUDevice device,
  WGPUShaderModule shader,
  size_t lightningBufferSize,
  size_t uniformBufferSize,
  WGPUTextureFormat depthFormat);

WGPURenderPipeline Pipeline_make(
  WGPUDevice device,
  WGPUShaderModule shader,
  size_t lightningBufferSize,
  size_t uniformBufferSize,
  WGPUTextureFormat depthFormat) {
  WGPUBlendState blendState = {
    .color.srcFactor = WGPUBlendFactor_SrcAlpha,
    .color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
    .color.operation = WGPUBlendOperation_Add,
    .alpha.srcFactor = WGPUBlendFactor_Zero,
    .alpha.dstFactor = WGPUBlendFactor_One,
    .alpha.operation = WGPUBlendOperation_Add
  };
  WGPUColorTargetState colorTarget = {
    .nextInChain = 0,
    .format = WGPUTextureFormat_BGRA8Unorm,
    .blend = &blendState,
    .writeMask = WGPUColorWriteMask_All,
  };
  WGPUFragmentState fragmentState = {
    .nextInChain = 0,
    .module = shader,
    .entryPoint = "fs_main",
    .constantCount = 0,
    .constants = 0,
    .targetCount = 1,
    .targets = &colorTarget,
  };
  WGPUDepthStencilState depthStencilState = Application_DepthStencilState_make();
  depthStencilState.depthCompare = WGPUCompareFunction_Less;
  depthStencilState.depthWriteEnabled = true;
  depthStencilState.format = depthFormat;
  depthStencilState.stencilReadMask = 0;
  depthStencilState.stencilWriteMask = 0;
  WGPUBindGroupLayout bindGroupLayout =
    BindGroupLayout_make(device, lightningBufferSize, uniformBufferSize);
  WGPUPipelineLayoutDescriptor layoutDescriptor = {
    .nextInChain = 0,
    .bindGroupLayoutCount = 1,
    .bindGroupLayouts = &bindGroupLayout,
  };
  WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(device, &layoutDescriptor);
  WGPUVertexAttribute vertexAttributes[] = {
    {
     // position
      .shaderLocation = 0,
     .format = WGPUVertexFormat_Float32x3,
     .offset = 0,
     },
    {
     // normal
      .shaderLocation = 1,
     .format = WGPUVertexFormat_Float32x3,
     .offset = offsetof(Model_Vertex, normal),
     },
    {
     // color
      .shaderLocation = 2,
     .format = WGPUVertexFormat_Float32x3,
     .offset = offsetof(Model_Vertex, color),
     },
    {
     // uv coordinates
      .shaderLocation = 3,
     .format = WGPUVertexFormat_Float32x2,
     .offset = offsetof(Model_Vertex, uv),
     }
  };
  WGPUVertexBufferLayout bufferLayout = {
    .attributeCount = 4,
    .attributes = vertexAttributes,
    .arrayStride = sizeof(Model_Vertex),
    .stepMode = WGPUVertexStepMode_Vertex,
  };
  WGPURenderPipelineDescriptor pipelineDesc = {
    .nextInChain = 0,
    .fragment = &fragmentState,
    .vertex.bufferCount = 1,
    .vertex.buffers = &bufferLayout,
    .vertex.module = shader,
    .vertex.entryPoint = "vs_main",
    .vertex.constantCount = 0,
    .vertex.constants = 0,
    .primitive.topology = WGPUPrimitiveTopology_TriangleList,
    .primitive.stripIndexFormat = WGPUIndexFormat_Undefined,
    .primitive.frontFace = WGPUFrontFace_CCW,
    .primitive.cullMode = WGPUCullMode_None,
    .depthStencil = &depthStencilState,
    .multisample.count = 1,
    .multisample.mask = ~0u,
    .multisample.alphaToCoverageEnabled = false,
    .layout = layout,
  };
  return wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);
}

#endif // Pipeline_H
