#ifndef Application_gui_H_
#define Application_gui_H_
#include "webgpu.h"
#include "GLFW/glfw3.h"
#include "cimgui/cimgui.h"
#include "cimgui/cimgui_impl_wgpu.h"
#include "cimgui/cimgui_impl_glfw.h"
#include "./Lightning.h"
#include "./gui/Chat.h"
#include "./gui/color.h"
#include "./gui/debug.h"

typedef struct {
    ImGuiContext* context;
    ImGuiIO* io;
    Chat* chat;
} Gui;
static Gui gui = { 0 };

bool Application_gui_attach(
  GLFWwindow* window,
  WGPUDevice device,
  WGPUTextureFormat depthFormat) {
  gui.context = ImGui_CreateContext(0);
  gui.io = ImGui_GetIO();
  gui.chat = &chat;
  cImGui_ImplGlfw_InitForOther(window, true);
  struct ImGui_ImplWGPU_InitInfo initInfo = {
    .Device = device,
    .NumFramesInFlight = 3,
    .RenderTargetFormat = WGPUTextureFormat_BGRA8Unorm,
    .DepthStencilFormat = depthFormat,
    .PipelineMultisampleState.count = 1,
    .PipelineMultisampleState.mask = UINT32_MAX,
    .PipelineMultisampleState.alphaToCoverageEnabled = false,
  };
  return cImGui_ImplWGPU_Init(&initInfo);
}
void Application_gui_detach() {
  cImGui_ImplGlfw_Shutdown();
  cImGui_ImplWGPU_Shutdown();
}
void Application_gui_render(
  WGPURenderPassEncoder renderPass,
  Application_Lighting* lightning,
  Camera camera[static 1]) {
  cImGui_ImplWGPU_NewFrame();
  cImGui_ImplGlfw_NewFrame();
  ImGui_NewFrame();
  gui_color_render(lightning);
  Chat_render();
  gui_debug_render(camera);
  ImGui_EndFrame();
  ImGui_Render();
  cImGui_ImplWGPU_RenderDrawData(ImGui_GetDrawData(), renderPass);
}
bool Application_gui_isCapturing() {
  return gui.io->WantCaptureMouse;
}

#endif // Application_gui_H_
