#ifndef Color_H
#define Color_H
#include "cimgui/cimgui.h"
#include "../Lightning.h"

void gui_color_render(Application_Lighting* lightning) {
  static float f = 0.0f;
  static int counter = 0;
  static bool show_demo_window = true;
  static bool show_another_window = false;
  static ImVec4 clear_color = { .x = 0.45f, .y = 0.55f, .z = 0.60f, .w = 1.00f };
  ImGui_Begin("Hello, world!", 0, 0);
  ImGui_Text("This is some useful text.");
  ImGui_Checkbox("Demo Window", &show_demo_window);
  ImGui_Checkbox("Another Window", &show_another_window);
  ImGui_SliderFloat("float", &f, 0.0f, 1.0f);
  ImGui_ColorEdit3("clear color", (float*)&clear_color, 0);
  if (ImGui_Button("Button")) {
    counter++;
  }
  ImGui_SameLine();
  ImGui_Text("counter = %d", counter);
  ImGuiIO* io = ImGui_GetIO();
  ImGui_Text(
    "Application average %.3f ms/frame (%.1f FPS)",
    1000.0f / io->Framerate,
    io->Framerate);
  bool update = false;
  update =
    ImGui_ColorEdit3("Color #0", lightning->uniforms.colors[0].components, 0) || update;
  update = ImGui_DragFloat3("Direction #0", lightning->uniforms.directions[0].components)
           || update;
  update =
    ImGui_ColorEdit3("Color #1", lightning->uniforms.colors[1].components, 0) || update;
  update = ImGui_DragFloat3("Direction #1", lightning->uniforms.directions[1].components)
           || update;
  update =
    ImGui_SliderFloat("Hardness", &lightning->uniforms.hardness, 1.0f, 100.0f) || update;
  update = ImGui_SliderFloat("Diffusivity", &lightning->uniforms.diffusivity, 0.0f, 1.0f)
           || update;
  update = ImGui_SliderFloat("Specularity", &lightning->uniforms.specularity, 0.0f, 1.0f)
           || update;
  lightning->update = update;
  ImGui_End();
}
#endif // Color_H
