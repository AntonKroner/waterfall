/*
 * lws-minimal-ws-client
 *
 * Written in 2010-2020 by Andy Green <andy@warmcat.com>
 *
 * This file is made available under the Creative Commons CC0 1.0
 * Universal Public Domain Dedication.
 *
 * This demonstrates a ws client that connects by default to libwebsockets.org
 * dumb increment ws server.
 */
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
} Socket;
static Socket mco;
static struct lws_context* context;
static int interrupted;
static int port = 8787;
static int ssl_connection = LCCSCF_ALLOW_INSECURE;
static const char* server_address = "localhost";
static const char* pro = "dumb-increment-protocol";
/*
 * The retry and backoff policy we want to use for our client connections
 */
static const uint32_t backoff_ms[] = { 1000, 2000, 3000, 4000, 5000 };
static const lws_retry_bo_t retry = {
  .retry_ms_table = backoff_ms,
  .retry_ms_table_count = LWS_ARRAY_SIZE(backoff_ms),
  .conceal_count = LWS_ARRAY_SIZE(backoff_ms),
  .secs_since_valid_ping = 0, /* force PINGs after secs idle */
  .secs_since_valid_hangup = 10, /* hangup after secs idle */
  .jitter_percent = 20,
};
/*
 * Scheduled sul callback that starts the connection attempt
 */
static void connect_client(lws_sorted_usec_list_t* sul) {
  Socket* m = lws_container_of(sul, Socket, sul);
  struct lws_client_connect_info i;
  memset(&i, 0, sizeof(i));
  i.context = context;
  i.port = port;
  i.address = server_address;
  i.path = "/";
  i.host = i.address;
  i.origin = i.address;
  i.ssl_connection = ssl_connection;
  i.protocol = pro;
  i.local_protocol_name = "lws-minimal-client";
  i.pwsi = &m->wsi;
  i.retry_and_idle_policy = &retry;
  i.userdata = m;
  if (!lws_client_connect_via_info(&i)) {
    /*
     * Failed... schedule a retry... we can't use the _retry_wsi()
     * convenience wrapper api here because no valid wsi at this
     * point.
     */
    if (lws_retry_sul_schedule(context, 0, sul, &retry, connect_client, &m->retry_count)) {
      lwsl_err("%s: connection attempts exhausted\n", __func__);
      interrupted = 1;
    }
  }
}
static int callback_minimal(
  struct lws* wsi,
  enum lws_callback_reasons reason,
  void* user,
  void* in,
  size_t len) {
  Socket* m = (Socket*)user;
  switch (reason) {
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
      lwsl_err("CLIENT_CONNECTION_ERROR: %s\n", in ? (char*)in : "(null)");
      goto do_retry;
      break;
    case LWS_CALLBACK_CLIENT_RECEIVE:
      Array_push(mco.messages, strdup(in ? (char*)in : "(null)"));
      printf("received data: %s\n", in ? (char*)in : "(null)");
      break;
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
      lwsl_user("%s: established\n", __func__);
      break;
    case LWS_CALLBACK_CLIENT_CLOSED:
      printf("callback retrying\n");
      goto do_retry;
    case LWS_CALLBACK_CLIENT_WRITEABLE:
      if (mco.post) {
	/* TODO: refactor to not use declaration without initialization */
        size_t length = strlen(mco.post);
        unsigned char* message = calloc(LWS_PRE + length, sizeof(*message));
        memcpy(&message[LWS_PRE], mco.post, length * sizeof(*message));
        lws_write(wsi, &message[LWS_PRE], length * sizeof(*message), LWS_WRITE_TEXT);
        free(message);
        memset(mco.post, 0, 256);
        mco.post = 0;
      }
      break;
    case 34:
    case 35:
    case 36:
    case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
      break;
    default:
      printf("callbacking: %i\n", reason);
      break;
  }
  return lws_callback_http_dummy(wsi, reason, user, in, len);
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
  if (lws_retry_sul_schedule_retry_wsi(wsi, &m->sul, connect_client, &m->retry_count)) {
    lwsl_err("%s: connection attempts exhausted\n", __func__);
    interrupted = 1;
  }
  return 0;
}
static const struct lws_protocols protocols[] = {
  {"lws-minimal-client", callback_minimal, 0, 0, 0, NULL, 0},
  LWS_PROTOCOL_LIST_TERM
};
Socket* Socket_initiate(Chat_Messages messages[static 1]) {
  mco.messages = messages;
  struct lws_context_creation_info info = {
    .options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT,
    .port = CONTEXT_PORT_NO_LISTEN,
    .protocols = protocols,
    .fd_limit_per_thread = 3,
  };
  lwsl_user("LWS minimal ws client\n");
#if defined(LWS_WITH_MBEDTLS) || defined(USE_WOLFSSL)
  printf("is defined\n");
  /*
   * OpenSSL uses the system trust store.  mbedTLS has to be told which
   * CA to trust explicitly.
   */
  info.client_ssl_ca_filepath = "./libwebsockets.org.cer";
#endif
  context = lws_create_context(&info);
  if (!context) {
    lwsl_err("lws init failed\n");
    return 0;
  }
  /* schedule the first client connection attempt to happen immediately */
  lws_sul_schedule(context, 0, &mco.sul, connect_client, 1);
  return &mco;
}
void Socket_destroy() {
  lws_context_destroy(context);
}
void Socket_update() {
  lws_service(context, 0);
}
void Socket_enqueue(char* post) {
  mco.post = post;
}
int doTheSocket() {
  struct lws_context_creation_info info = {
    .options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT,
    .port = CONTEXT_PORT_NO_LISTEN,
    .protocols = protocols,
    .fd_limit_per_thread = 3,
  };
  int n = 0;
  lwsl_user("LWS minimal ws client\n");
#if defined(LWS_WITH_MBEDTLS) || defined(USE_WOLFSSL)
  printf("is defined\n");
  /*
   * OpenSSL uses the system trust store.  mbedTLS has to be told which
   * CA to trust explicitly.
   */
  info.client_ssl_ca_filepath = "./libwebsockets.org.cer";
#endif
  context = lws_create_context(&info);
  if (!context) {
    lwsl_err("lws init failed\n");
    return 1;
  }
  /* schedule the first client connection attempt to happen immediately */
  lws_sul_schedule(context, 0, &mco.sul, connect_client, 1);
  while (n >= 0 && !interrupted) {
    n = lws_service(context, 0);
  }
  lws_context_destroy(context);
  lwsl_user("Completed\n");
  return 0;
}

#endif // Socket_H_
