#ifndef Chat_H_
#define Chat_H_

#include <stdio.h>
#include <string.h>
#include "cimgui/cimgui.h"
#include "./Socket.h"
#include "./Messages.h"

typedef struct {
    Chat_Messages* messages;
    char input[256];
} Chat;
static Chat chat = { 0 };

void Chat_initiate();
void Chat_render();
static void table();
static bool input();
void Chat_initiate() {
  if (!chat.messages) {
    chat.messages = calloc(sizeof(*chat.messages), 1);
    Array_init(chat.messages);
  }
  Socket_initiate(chat.messages);
}
void Chat_render() {
  Socket_update();
  ImGui_Begin(
    "Chat",
    0,
    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
      | ImGuiWindowFlags_NoCollapse);
  ImVec2 size = { 0, -25 };
  ImGui_BeginChild(
    "chat message pane",
    size,
    ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX,
    0);
  table();
  ImGui_EndChild();
  if (input()) {
    Socket_enqueue(chat.input);
  }
  ImGui_End();
}
static bool input() {
  bool field = ImGui_InputTextWithHint(
    "chat text input",
    "type to chat...",
    chat.input,
    256,
    ImGuiInputTextFlags_EnterReturnsTrue);
  bool hasText = strlen(chat.input) > 0;
  ImGui_SameLine();
  return (field && hasText) || (hasText && ImGui_Button("post"));
}
static void table() {
  if (ImGui_BeginTable("Chat messages", 1, 0)) {
    ImGui_TableNextRow();
    ImGui_TableNextColumn();
    ImGui_Text("message");
    for (size_t i = 0; chat.messages->length > i; i++) {
      ImGui_TableNextRow();
      ImGui_TableNextColumn();
      ImGui_Text("%s", chat.messages->data[i]);
    }
    ImGui_EndTable();
  }
}

#endif // Chat_H_
