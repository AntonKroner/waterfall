#ifndef debug_H
#define debug_H
// #include <stdio.h>
// #include <string.h>
#include <stdbool.h>
#include "cimgui/cimgui.h"
#include <tgmath.h>
#include "../Camera.h"

void gui_debug_render(Camera camera[static 1]) {
  if (ImGui_Begin("Debug", 0, 0)) {
    ImGui_Text("Camera");
    ImGui_Text(
      "Position: %f, %f, %f",
      camera->position.components[0],
      camera->position.components[1],
      camera->position.components[2]);
    ImGui_Text(
      "Angles: %f, %f",
      180 * camera->angles.components[0] / M_PI,
      180 * camera->angles.components[1] / M_PI);
    ImGui_Text(
      "Target: %f, %f, %f",
      camera->target.components[0],
      camera->target.components[1],
      camera->target.components[2]);
    ImGui_Text("Zoom: %f", camera->zoom);
  }
  ImGui_End();
}

#endif // debug_H
