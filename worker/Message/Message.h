#ifndef Message_H
#define Message_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum /*: uint32_t compiler is really dumb atm*/ {
  Message_login = 0,
  Message_chat,
  Message_move,
} Message_Type;
typedef struct {
    uint32_t id;
    char name[256];
    float position[3];
} Message_Login;
typedef struct {
    char message[256];
} Message_Chat;
typedef struct {
    uint32_t id;
    float direction[3];
} Message_Move;
typedef struct {
    Message_Type type;
    union {
        Message_Login login;
        Message_Chat chat;
        Message_Move move;
    };
} Message;

void test(Message message) {
  if (message.type == Message_chat) {
    message.chat.message;
  }
}
char* Message_Type_print(Message_Type type) {
  switch (type) {
    case Message_login:
      return "login";
    case Message_chat:
      return "chat";
    case Message_move:
      return "move";
  }
}
void Message_print(const Message message) {
  printf("message: \n\ttype: %s\n", Message_Type_print(message.type));
  switch (message.type) {
    case Message_login:
      printf("\tid: %i\n", message.login.id);
      printf("\tname: %s\n", message.login.name);
      printf(
        "\tposition: [%f, %f, %f]\n",
        message.login.position[0],
        message.login.position[1],
        message.login.position[2]);
      break;
    case Message_chat:
      printf("\tmessage: %s\n", message.chat.message);
      break;
    case Message_move:
      printf("\tid: %i\n", message.move.id);
      printf(
        "\tposition: [%f, %f, %f]\n",
        message.move.direction[0],
        message.move.direction[1],
        message.move.direction[2]);
      break;
    default:
      printf("unknown message type: %d\n", message.type);
      break;
  }
}

#endif // Message_H
