#ifndef Queues_H_
#define Queues_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../worker/Message/Message.h"

#define QUEUE_MESSAGES_MAX 100

typedef struct {
    struct {
        size_t front;
        size_t back;
    } index;
    size_t count;
    Message messages[QUEUE_MESSAGES_MAX];
} Queue;

static Queue Queue_incoming = { 0 };
static Queue Queue_outgoing = { 0 };

void Queue_push(Queue queue[const static 1], const Message message);
bool Queue_pop(Queue queue[const static 1], Message message[const static 1]);
void Queue_print(const Queue queue);

void Queue_push(Queue queue[const static 1], const Message message) {
  if (queue->count >= QUEUE_MESSAGES_MAX) {
    fprintf(stderr, "Max queue size exceeded\n");
    abort();
  }
  else {
    // printf("trying to psuh quuee\n");
    queue->count++;
    queue->messages[queue->index.back++] = message;
    if (queue->index.back > QUEUE_MESSAGES_MAX) {
      queue->index.back = 0;
    }
  }
}
bool Queue_pop(Queue queue[const static 1], Message message[const static 1]) {
  bool result = false;
  if (queue->count) {
    // printf("trying to pop quuee\n");
    *message = queue->messages[queue->index.front];
    if (!(--queue->count)) {
      queue->index.front = 0;
      queue->index.back = 0;
    }
    else if (++queue->index.front > QUEUE_MESSAGES_MAX) {
      queue->index.front = 0;
    }
    result = true;
  }
  return result;
}
void Queue_print(const Queue queue) {
  printf("queue:\n");
  printf("\tfront: %zu\n", queue.index.front);
  printf("\tback: %zu\n", queue.index.back);
  printf("\tcount: %zu\n", queue.count);
}

#endif // Queues_H_
