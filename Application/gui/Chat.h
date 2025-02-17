#ifndef Chat_H_
#define Chat_H_
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cimgui/cimgui.h"
#include "./SSocket.h"
#include "./Messages.h"

typedef struct {
    Chat_Messages* messages;
    Socket* socket;
    char input[256];
    char username[256];
    char password[256];
    bool open;
} Chat;
static Chat chat = { 0 };

void Chat_initiate();
void Chat_render();
static void Chat_login();
static void error();
static void window();
static void table();
static bool input();

void Chat_initiate() {
  if (!chat.messages) {
    chat.messages = calloc(sizeof(*chat.messages), 1);
    Array_init(chat.messages);
  }
  chat.open = true;
}
void Chat_render() {
  if (!chat.socket) {
    Chat_login();
  }
  else if (strlen(chat.socket->error)) {
    error();
  }
  else {
    Socket_update(chat.socket);
    window();
  }
}
static void error() {
  const int flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
                    | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
  ImGui_Begin("Login", 0, flags);
  ImGui_Text("error: %s\n", chat.socket->error);
  if (ImGui_Button("Retry")) {
    Socket_destroy(chat.socket);
    chat.socket = 0;
  }
  ImGui_End();
}
static void window() {
  const int flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
                    | ImGuiWindowFlags_NoCollapse;
  if (ImGui_Begin("Chat", &chat.open, flags)) {
    ImVec2 size = { 0, -25 };
    ImGuiWindowFlags flags = ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX;
    ImGui_BeginChild("chat message pane", size, flags, 0);
    table();
    ImGui_EndChild();
    if (input()) {
      Message message = { .type = Message_chat, .chat = { 0 } };
      strcpy(message.chat.message, chat.input);
      Queue_push(&Queue_outgoing, message);
      memset(chat.input, 0, strlen(chat.input));
    }
    if (!chat.open) {
      Message message = { .type = Message_chat, .chat = { "logout" } };
      Queue_push(&Queue_outgoing, message);
    }
  }
  ImGui_End();
}
static bool input() {
  bool field = ImGui_InputTextWithHint(
    "##chat text input",
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
    for (size_t i = 0; chat.messages->length > i; i++) {
      ImGui_TableNextRow();
      ImGui_TableNextColumn();
      ImGui_Text("%s", chat.messages->data[i]);
    }
    ImGui_EndTable();
  }
}
static void Chat_login() {
  ImVec2 size = { 200, 100 };
  ImGui_SetNextWindowSize(size, 0);
  const int flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
                    | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
  ImGui_Begin("Login", 0, flags);
  bool username = ImGui_InputTextWithHint(
    "##username input",
    "username",
    chat.username,
    256,
    ImGuiInputTextFlags_EnterReturnsTrue);
  bool password = ImGui_InputTextWithHint(
    "##password input",
    "password",
    chat.password,
    256,
    ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password);
  if (
    strlen(chat.username) > 0 && strlen(chat.password) > 0
    && (password || ImGui_Button("Login"))) {
    Chat_initiate();
  }
  ImGui_End();
}

#endif // Chat_H_
