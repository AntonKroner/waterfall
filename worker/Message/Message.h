#ifndef Message_H
#define Message_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum: uint32_t {
  Message_login = 0,
  Message_chat,
  Message_move,
} Message_Type;
typedef struct {
    char name[256];
} Message_Login;
typedef struct {
    char message[256];
} Message_Chat;
typedef struct {
    float direction;
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

#endif // Message_H
