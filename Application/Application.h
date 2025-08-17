#ifndef Application_H_
#define Application_H_
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "webgpu.h"
#include "GLFW/glfw3.h"
#include "glfw3webgpu/glfw3webgpu.h"
#include "linear/algebra.h"
#include "./adapter.h"
#include "./device.h"
#include "./RenderPass.h"
#include "./Depth.h"
#include "./Uniform/Uniform.h"
#include "./Lightning.h"
#include "./RenderTarget/RenderTarget.h"
#include "./RenderTarget/Fourareen.h"
#include "./RenderTarget/Mammoth.h"
#include "./RenderTarget/Player.h"
#include "./RenderTarget/BBindGroup.h"
#include "./RenderTarget/BBindGroupLayout.h"
#include "./Camera.h"
#include "./gui/gui.h"
#include "./Queues.h"
#include "./SSocket.h"

#define TARGET_COUNT (10)
#define PLAYERS_MAX (10)
#define PLAYER_UNSET (99)

typedef struct {
    GLFWwindow* window;
    WGPUInstance instance;
    WGPUSurface surface;
    WGPUSurfaceCapabilities capabilities;
    WGPUDevice device;
    WGPUQueue queue;
    Application_Depth depth;
    RenderTarget* targets[TARGET_COUNT];
    size_t playerId;
    struct {
        size_t count;
        Player* players[PLAYERS_MAX];
    } players;
    struct {
        Uniform_Global uniform;
        struct {
            WGPUBindGroupLayout groupLayout;
            WGPUBindGroup group;
        } bind;
    } globals;
    WGPUBuffer uniformBuffer;
    Camera camera;
    Application_Lighting lightning;
    Gui* gui;
    Socket* socket;
} Application;

Application* Application_create(const size_t width, const size_t height, bool inspect);
bool Application_shouldClose(Application application[static 1]);
void Application_render(Application application[static 1]);
void Application_destroy(Application* application);

static void surface_attach(Application application[static 1], size_t width, size_t height) {
  const WGPUSurfaceConfiguration configuration = {
    .nextInChain = 0,
    .width = width,
    .height = height,
    .format =
      23, // meant to be: application->capabilities.formats[0], but for some reason it doesn't work for every adapter type
    .viewFormatCount = 0,
    .viewFormats = 0,
    .usage = WGPUTextureUsage_RenderAttachment,
    .device = application->device,
    .presentMode = WGPUPresentMode_Fifo,
    .alphaMode = WGPUCompositeAlphaMode_Auto,
  };
  wgpuSurfaceConfigure(application->surface, &configuration);
}
static void uniform_attach(Application application[static 1], double width, double height) {
  Application_Camera_initiate(&application->camera, M_PI * 45 / 180, M_PI * 45 / 180);
  application->camera.zoom = 1.0f;
  Uniform_Global uniform = {
    .matrices.model = Matrix4f_transpose(Matrix4f_diagonal(1.0)),
    .matrices.view = Matrix4f_transpose(Application_Camera_viewGet(application->camera)),
    .matrices.projection =
      Matrix4f_transpose(Matrix4f_perspective(45, width / height, 0.01f, 100.0f)),
    .time = 0.0f,
    .cameraPosition = application->camera.position,
    .color = Vector4f_make(0.0f, 1.0f, 0.4f, 1.0f),
  };
  application->globals.uniform = uniform;
  WGPUBufferDescriptor descriptor = {
    .nextInChain = 0,
    .label = "uniform buffer",
    .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
    .mappedAtCreation = false,
    .size = sizeof(Uniform_Global),
  };
  application->uniformBuffer = wgpuDeviceCreateBuffer(application->device, &descriptor);
  application->globals.bind.groupLayout = BindGroupLayout_globalMake(
    application->device,
    sizeof(Application_Lighting_Uniforms),
    sizeof(Uniform_Global));
  application->globals.bind.group = BindGroup_globalMake(
    application->device,
    application->globals.bind.groupLayout,
    application->lightning.buffer,
    sizeof(Application_Lighting_Uniforms),
    application->uniformBuffer,
    sizeof(Uniform_Global));
}
static void uniform_detach(Application application[static 1]) {
  wgpuBufferDestroy(application->uniformBuffer);
  wgpuBufferRelease(application->uniformBuffer);
}
static void onResize(GLFWwindow* window, int width, int height) {
  Application* application = (Application*)glfwGetWindowUserPointer(window);
  if (application) {
    wgpuSurfaceUnconfigure(application->surface);
    surface_attach(application, width, height);
    Application_Depth_detach(application->depth);
    application->depth = Application_Depth_attach(application->device, width, height);
    application->globals.uniform.matrices.projection = Matrix4f_transpose(
      Matrix4f_perspective(45, ((float)width / (float)height), 0.01f, 100.0f));
  }
}
static void onMouseMove(GLFWwindow* window, double x, double y) {
  if (Application_gui_isCapturing()) {
    return;
  }
}
static void onMouseButton(GLFWwindow* window, int button, int action, int /* mods*/) {
  if (Application_gui_isCapturing()) {
    return;
  }
}
static void onMouseScroll(GLFWwindow* window, double x, double y) {
  Application* application = (Application*)glfwGetWindowUserPointer(window);
  if (application) {
    Application_Camera_zoom(&application->camera, (float)x, (float)y);
    application->globals.uniform.matrices.view =
      Matrix4f_transpose(Application_Camera_viewGet(application->camera));
    application->globals.uniform.cameraPosition = application->camera.position;
  }
}
static void onKeyPress(GLFWwindow* window, int key, int /*s*/, int /*a*/, int /*m*/) {
  Application* application = (Application*)glfwGetWindowUserPointer(window);
  Vector3f direction = Vector3f_make(0, 0, 0);
  if (application && !application->gui->io->WantCaptureKeyboard) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
      case GLFW_KEY_W:
        direction.components[0] = 1;
        Application_Camera_moveTarget(&application->camera, direction);
        break;
      case GLFW_KEY_S:
        direction.components[0] = -1;
        Application_Camera_moveTarget(&application->camera, direction);
        break;
      case GLFW_KEY_A:
        direction.components[1] = -1;
        Application_Camera_moveTarget(&application->camera, direction);
        break;
      case GLFW_KEY_D:
        direction.components[1] = 1;
        Application_Camera_moveTarget(&application->camera, direction);
        break;
      case GLFW_KEY_UP:
        direction.components[2] = 1;
        Application_Camera_moveTarget(&application->camera, direction);
        break;
      case GLFW_KEY_DOWN:
        direction.components[2] = -1;
        Application_Camera_moveTarget(&application->camera, direction);
        break;
      case GLFW_KEY_LEFT:
        Application_Camera_rotate(&application->camera, -1);
        break;
      case GLFW_KEY_RIGHT:
        Application_Camera_rotate(&application->camera, 1);
        break;
      case GLFW_KEY_SPACE:
        Application_Camera_initiate(&application->camera, M_PI * 45 / 180, M_PI * 45 / 180);
        application->camera.zoom = 1;
        break;
    }
    if (application->gui->chat->socket && key != GLFW_KEY_LEFT_ALT && key != GLFW_KEY_TAB) {
      Message message = { .type = Message_move, .move = { .id = application->playerId } };
      memcpy(message.move.direction, &direction, sizeof(Vector3f));
      Queue_push(&Queue_outgoing, message);
    }
    application->globals.uniform.matrices.view =
      Matrix4f_transpose(Application_Camera_viewGet(application->camera));
    application->globals.uniform.cameraPosition = application->camera.position;
  }
}
static WGPUTextureView nextView(WGPUSurface surface) {
  WGPUTextureView result = 0;
  WGPUSurfaceTexture surfaceTexture = { 0 };
  wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);
  if (surfaceTexture.status == WGPUSurfaceGetCurrentTextureStatus_Success) {
    WGPUTextureViewDescriptor viewDescriptor = {
      .nextInChain = 0,
      .label = "Surface texture view",
      .format = wgpuTextureGetFormat(surfaceTexture.texture),
      .dimension = WGPUTextureViewDimension_2D,
      .baseMipLevel = 0,
      .mipLevelCount = 1,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .aspect = WGPUTextureAspect_All,
    };
    result = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);
  }
  return result;
}
static bool setWindowHints() {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  return true;
}
static void attachCallbacks(Application application[static 1]) {
  glfwSetWindowUserPointer(application->window, application);
  glfwSetFramebufferSizeCallback(application->window, onResize);
  glfwSetCursorPosCallback(application->window, onMouseMove);
  glfwSetMouseButtonCallback(application->window, onMouseButton);
  glfwSetScrollCallback(application->window, onMouseScroll);
  glfwSetKeyCallback(application->window, onKeyPress);
}
Application* Application_create(const size_t width, const size_t height, bool inspect) {
  WGPUInstanceDescriptor descriptor = { .nextInChain = 0 };
  Application* result = calloc(1, sizeof(*result));
  if (!result) {
    perror("Application allocation failed.");
    result = 0;
  }
  else if (!glfwInit()) {
    glfwTerminate();
    free(result);
    perror("Could not initialize GLFW.");
    result = 0;
  }
  else if (!(result->instance = wgpuCreateInstance(&descriptor))) {
    glfwTerminate();
    free(result);
    perror("Could not initialize WebGPU!");
    result = 0;
  }
  else if (
    setWindowHints()
    && !(result->window = glfwCreateWindow(width, height, "Application", NULL, NULL))) {
    wgpuInstanceRelease(result->instance);
    glfwTerminate();
    free(result);
    perror("Could not open window!");
    result = 0;
  }
  else if (!(result->surface = glfwGetWGPUSurface(result->instance, result->window))) {
    perror("Could not get surface!");
    result = 0;
  }
  else {
    attachCallbacks(result);
    WGPURequestAdapterOptions adapterOptions = {
      .nextInChain = 0,
      .compatibleSurface = result->surface,
      .powerPreference = WGPUPowerPreference_HighPerformance,
    };
    WGPUAdapter adapter = Application_adapter_request(result->instance, &adapterOptions);
    result->device = Application_device_request(adapter);
    if (inspect) {
      Application_device_inspect(result->device);
      Application_adapter_inspect(adapter);
    }
    wgpuSurfaceGetCapabilities(result->surface, adapter, &result->capabilities);
    surface_attach(result, width, height);
    wgpuAdapterRelease(adapter);
    result->queue = wgpuDeviceGetQueue(result->device);
    result->depth = Application_Depth_attach(result->device, width, height);
    result->lightning = Application_Lightning_create(result->device);
    uniform_attach(result, width, height);
    for (size_t i = 0; 4 - 2 > i; i++) {
      result->targets[i] = (RenderTarget*)Fourareen_Create(
        0,
        result->device,
        result->queue,
        result->depth.format,
        result->globals.bind.groupLayout,
        Vector3f_make(i * 5, 0, 0));
    }
    result->targets[4 - 2] = (RenderTarget*)Mammoth_Create(
      0,
      result->device,
      result->queue,
      result->depth.format,
      result->globals.bind.groupLayout,
      Vector3f_make(0, 3, 0));
    result->playerId = PLAYER_UNSET;
    if (!Application_gui_attach(result->window, result->device, result->depth.format)) {
      printf("gui problem!!\n");
    }
    result->gui = &gui;
    Application_Lightning_update(&result->lightning, result->queue);
  }
  return result;
}
bool Application_shouldClose(Application application[static 1]) {
  return glfwWindowShouldClose(application->window);
}
void Application_render(Application application[static 1]) {
  glfwPollEvents();
  if (application->socket) {
    Socket_update(application->socket);
  }
  else if (application->gui->chat->open) {
    application->socket =
      Socket_create(application->gui->chat->username, application->gui->chat->password);
    application->gui->chat->socket = application->socket;
  }
  Message message = { 0 };
  if (Queue_pop(&Queue_incoming, &message)) {
    switch (message.type) {
      case Message_chat:
        Array_push(application->gui->chat->messages, strdup(message.chat.message));
        printf("render message chat\n");
        break;
      case Message_login:
        printf("message login\n");
        if (application->playerId == PLAYER_UNSET) {
          application->playerId = message.login.id;
        }
        application->players.players[message.login.id] = Player_Create(
          0,
          application->device,
          application->queue,
          application->depth.format,
          application->globals.bind.groupLayout,
          Vector3f_fill(0),
          Vector3f_from(message.login.position));
        application->targets[3 + message.login.id] =
          (RenderTarget*)application->players.players[message.login.id];
        application->players.count++;
        char loginMessage[] = "Player 1 has logged in";
        loginMessage[7] = message.login.id + '0';
        Array_push(application->gui->chat->messages, strdup(loginMessage));
        break;
      case Message_move:
        printf("message move\n");
        Player_move(
          application->players.players[message.login.id],
          application->queue,
          Vector3f_from(message.move.direction));
        break;
      default:
        printf("unhandled message type: %i\n", message.type);
        break;
    }
  }
  Application_Lightning_update(&application->lightning, application->queue);
  WGPUTextureView nextTexture = nextView(application->surface);
  if (!nextTexture) {
    perror("Cannot acquire next swap chain texture\n");
  }
  else {
    application->globals.uniform.time = (float)glfwGetTime();
    wgpuQueueWriteBuffer(
      application->queue,
      application->uniformBuffer,
      0,
      &application->globals.uniform,
      sizeof(Uniform_Global));
    WGPUCommandEncoderDescriptor commandEncoderDescriptor = {
      .nextInChain = 0,
      .label = "Command Encoder",
    };
    WGPUCommandEncoder encoder =
      wgpuDeviceCreateCommandEncoder(application->device, &commandEncoderDescriptor);
    WGPURenderPassEncoder renderPass =
      Application_RenderPassEncoder_make(encoder, nextTexture, application->depth.view);
    wgpuRenderPassEncoderSetBindGroup(renderPass, 0, application->globals.bind.group, 0, 0);
    for (size_t i = 0; TARGET_COUNT > i; i++) {
      if (application->targets[i]) {
        RenderTarget_render(application->targets[i], renderPass);
      }
    }
    Application_gui_render(renderPass, &application->lightning, &application->camera);
    wgpuRenderPassEncoderEnd(renderPass);
    wgpuTextureViewRelease(nextTexture);
    WGPUCommandBufferDescriptor cmdBufferDescriptor = {
      .nextInChain = 0,
      .label = "command buffer",
    };
    WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
    wgpuCommandEncoderRelease(encoder);
    wgpuQueueSubmit(application->queue, 1, &command);
    wgpuCommandBufferRelease(command);
  }
  wgpuSurfacePresent(application->surface);
  wgpuDeviceTick(application->device);
}
void Application_destroy(Application* application) {
  Application_Lightning_destroy(application->lightning);
  Application_gui_detach();
  Application_Depth_detach(application->depth);
  for (size_t i = 0; TARGET_COUNT > i; i++) {
    RenderTarget_destroy(application->targets[i]);
  }
  uniform_detach(application);
  wgpuSurfaceUnconfigure(application->surface);
  wgpuSurfaceRelease(application->surface);
  wgpuQueueRelease(application->queue);
  wgpuDeviceRelease(application->device);
  wgpuInstanceRelease(application->instance);
  wgpuSurfaceRelease(application->surface);
  glfwDestroyWindow(application->window);
  glfwTerminate();
  free(application);
}

#endif // Application_H_
