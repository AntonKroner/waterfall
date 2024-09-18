#ifndef Socket_H_
#define Socket_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libwebsockets.h"
#include "rxi/Array.h"
#include "./Messages.h"

typedef struct {
    lws_sorted_usec_list_t sul; /* schedule connection retry */
    struct lws* wsi; /* related wsi if any */
    uint16_t retry_count; /* count of consequetive retries */
    Chat_Messages* messages;
    Chat_Message post;
    char authentication[256];
    char error[256];
    struct lws_context* context;
} Socket;

static int onCallback(
  struct lws* wsi,
  enum lws_callback_reasons reason,
  void* user,
  void* in,
  size_t size);
static void onConnect(lws_sorted_usec_list_t* sul);

static int interrupted;
static int port = 8787;
static int ssl_connection = LCCSCF_ALLOW_INSECURE;
static const char* server_address = "localhost";
static const uint32_t backoff_ms[] = { 1000, 2000, 3000, 4000, 5000 };
static const lws_retry_bo_t retry = {
  .retry_ms_table = backoff_ms,
  .retry_ms_table_count = LWS_ARRAY_SIZE(backoff_ms),
  .conceal_count = LWS_ARRAY_SIZE(backoff_ms),
  .secs_since_valid_ping = 0, /* force PINGs after secs idle */
  .secs_since_valid_hangup = 10, /* hangup after secs idle */
  .jitter_percent = 20,
};
static const struct lws_protocols protocols[] = {
  {"waterfall-client", onCallback, 0, 0, 0, NULL, 0},
  LWS_PROTOCOL_LIST_TERM
};
static void onConnect(lws_sorted_usec_list_t* sul) {
  Socket* m = lws_container_of(sul, Socket, sul);
  struct lws_client_connect_info i = {
    .context = m->context,
    .port = port,
    .address = server_address,
    .path = "/",
    .host = i.address,
    .origin = i.address,
    .ssl_connection = ssl_connection,
    // .protocol = m->authentication,
    .pwsi = &m->wsi,
    .retry_and_idle_policy = &retry,
    .userdata = m,
  };
  if (!lws_client_connect_via_info(&i)) {
    if (lws_retry_sul_schedule(m->context, 0, sul, &retry, onConnect, &m->retry_count)) {
      lwsl_err("%s: connection attempts exhausted\n", __func__);
      interrupted = 1;
    }
  }
}
static bool handleError(Socket socket[static 1], size_t size, void* data) {
  bool result;
  printf("error: %s\n", (char*)data);
  if (!strcmp("HS: ws upgrade unauthorized", (char*)data)) {
    strcpy(socket->error, "Unable to login!");
    result = false;
  }
  else if (!strcmp("HS: ws upgrade response not 101", (char*)data)) {
    unsigned int status = lws_http_client_http_response(socket->wsi);
    if (status == 403) {
      strcpy(socket->error, "Forbidden!");
    }
    else if (status == 404) {
      strcpy(socket->error, "Not found!");
    }
    else if (status >= 500) {
      strcpy(socket->error, "Server error!");
    }
    else {
      strcpy(socket->error, "Unknown error!");
      printf("status: %u\n", status);
    }
    result = false;
  }
  else {
    printf(
      "Unhandled ws error: %s\nStatus: %u\nRetrying...\n",
      (char*)data,
      lws_http_client_http_response(socket->wsi));
    result = true;
  }
  return result;
}
static int onCallback(
  struct lws* wsi,
  enum lws_callback_reasons reason,
  void* user,
  void* in,
  size_t size) {
  Socket* m = (Socket*)user;
  switch (reason) {
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
      lwsl_err("CLIENT_CONNECTION_ERROR: %s\n", in ? (char*)in : "(null)");
      if (handleError(m, size, in)) {
        goto do_retry;
      }
      break;
    case LWS_CALLBACK_CLIENT_RECEIVE:
      Array_push(m->messages, strdup(in ? (char*)in : "(null)"));
      printf("received data: %s\n", in ? (char*)in : "(null)");
      break;
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
      lwsl_user("%s: established\n", __func__);
      break;
    case LWS_CALLBACK_CLIENT_CLOSED:
      printf("LWS_CALLBACK_CLIENT_CLOSED: retrying...\n");
      goto do_retry;
    case LWS_CALLBACK_CLIENT_WRITEABLE:
      if (m->post) {
        // TODO: use a fixed length buffer for messages
        size_t length = strlen(m->post);
        unsigned char* message = calloc(LWS_PRE + length, sizeof(*message));
        memcpy(&message[LWS_PRE], m->post, length * sizeof(*message));
        lws_write(wsi, &message[LWS_PRE], length * sizeof(*message), LWS_WRITE_TEXT);
        free(message);
        memset(m->post, 0, 256);
        m->post = 0;
      }
      break;
    case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
      return lws_add_http_header_by_token(
        wsi,
        WSI_TOKEN_HTTP_AUTHORIZATION,
        m->authentication,
        strlen(m->authentication),
        in,
        (*(char**)in) + size);
      break;
    case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
    case LWS_CALLBACK_LOCK_POLL:
    case LWS_CALLBACK_UNLOCK_POLL:
    case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
    case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
    case LWS_CALLBACK_VHOST_CERT_AGING:
    case LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP:
    case LWS_CALLBACK_WSI_CREATE:
    case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
    case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
    case LWS_CALLBACK_CONNECTING:
    case LWS_CALLBACK_ADD_POLL_FD:
    case LWS_CALLBACK_GET_THREAD_ID:
    case LWS_CALLBACK_PROTOCOL_INIT:
    case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:
      break;
    default:
      printf("callbacking: %i\n", reason);
      break;
  }
  return lws_callback_http_dummy(wsi, reason, user, in, size);
do_retry:
  /*
   * retry the connection to keep it nailed up
   *
   * For this example, we try to conceal any problem for one set of
   * backoff retries and then exit the app.
   *
   * If you set retry.conceal_count to be LWS_RETRY_CONCEAL_ALWAYS,
   * it will never give up and keep retrying at the last backoff
   * delay plus the random jitter amount.
   */
  if (lws_retry_sul_schedule_retry_wsi(wsi, &m->sul, onConnect, &m->retry_count)) {
    lwsl_err("%s: connection attempts exhausted\n", __func__);
    interrupted = 1;
  }
  return 0;
}
Socket* Socket_create(Chat_Messages messages[static 1], char* username, char* password) {
  struct lws_context_creation_info info = {
    .options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT,
    .port = CONTEXT_PORT_NO_LISTEN,
    .protocols = protocols,
    .fd_limit_per_thread = 3,
  };
  lwsl_user("LWS minimal ws client\n");
#if defined(LWS_WITH_MBEDTLS) || defined(USE_WOLFSSL)
  info.client_ssl_ca_filepath = "./libwebsockets.org.cer";
#endif
  Socket* socket = calloc(1, sizeof(*socket));
  if (!socket) {
    perror("Unable to allocate socket.");
  }
  else if (lws_http_basic_auth_gen(username, password, socket->authentication, 256)) {
    perror("Unable generate authentication header.");
    free(socket);
    socket = 0;
  }
  else if (!(socket->context = lws_create_context(&info))) {
    lwsl_err("lws init failed\n");
    free(socket);
    socket = 0;
  }
  else {
    socket->messages = messages;
    socket->context = lws_create_context(&info);
    lws_sul_schedule(socket->context, 0, &socket->sul, onConnect, 1);
  }
  return socket;
}
void Socket_destroy(Socket* socket) {
  lws_context_destroy(socket->context);
  free(socket);
}
void Socket_update(Socket socket[static 1]) {
  lws_service(socket->context, 0);
}
void Socket_enqueue(Socket socket[static 1], char* post) {
  socket->post = post;
}

#endif // Socket_H_
