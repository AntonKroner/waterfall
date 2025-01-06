#ifndef Pipeline_H_
#define Pipeline_H_
#include "webgpu.h"
#include "./Model.h"

WGPURenderPipeline Pipeline_make(
  WGPUDevice device,
  WGPUShaderModule shader,
  size_t bindGroupCount,
  WGPUBindGroupLayout bindGroupLayout[static bindGroupCount],
  WGPUTextureFormat depthFormat);

static WGPUDepthStencilState DepthStencilState_make() {
  WGPUStencilFaceState face = {
    .compare = WGPUCompareFunction_Always,
    .failOp = WGPUStencilOperation_Keep,
    .depthFailOp = WGPUStencilOperation_Keep,
    .passOp = WGPUStencilOperation_Keep,
  };
  WGPUDepthStencilState result = {
    .nextInChain = 0,
    .format = WGPUTextureFormat_Undefined,
    .depthWriteEnabled = false,
    .depthCompare = WGPUCompareFunction_Always,
    .stencilFront = face,
    .stencilBack = face,
    .stencilReadMask = 0,
    .stencilWriteMask = 0,
    .depthBias = 0,
    .depthBiasSlopeScale = 0,
    .depthBiasClamp = 0,
  };
  return result;
}

const WGPUBlendState blendState = {
  .color.srcFactor = WGPUBlendFactor_SrcAlpha,
  .color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
  .color.operation = WGPUBlendOperation_Add,
  .alpha.srcFactor = WGPUBlendFactor_Zero,
  .alpha.dstFactor = WGPUBlendFactor_One,
  .alpha.operation = WGPUBlendOperation_Add
};
WGPURenderPipeline Pipeline_make(
  WGPUDevice device,
  WGPUShaderModule shader,
  size_t bindGroupCount,
  WGPUBindGroupLayout bindGroupLayout[static bindGroupCount],
  WGPUTextureFormat depthFormat) {
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
  WGPUDepthStencilState depthStencilState = DepthStencilState_make();
  depthStencilState.depthCompare = WGPUCompareFunction_Less;
  depthStencilState.depthWriteEnabled = true;
  depthStencilState.format = depthFormat;
  depthStencilState.stencilReadMask = 0;
  depthStencilState.stencilWriteMask = 0;
  WGPUPipelineLayoutDescriptor layoutDescriptor = {
    .nextInChain = 0,
    .label = "Pipeline layout Descriptor.",
    .bindGroupLayoutCount = bindGroupCount,
    .bindGroupLayouts = bindGroupLayout,
  };
  WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(device, &layoutDescriptor);
  WGPUVertexBufferLayout bufferLayout = Model_bufferLayoutMake();
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

#endif // Pipeline_H_
