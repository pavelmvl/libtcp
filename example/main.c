
#include "libtcp.h"

#define ARGS(fmt, ...)  __VA_ARGS__
#define FMT(fmt, ...) fmt

#define info(...) \
          fprintf(stderr, "%s, %d: INFO: " FMT(__VA_ARGS__, 0)"%s\n", __func__, __LINE__, ARGS(__VA_ARGS__, ""))

#define error(...) \
          fprintf(stderr, "%s, %d: ERROR: " FMT(__VA_ARGS__, 0)"%s\n", __func__, __LINE__, ARGS(__VA_ARGS__, ""))

#define alert(...) \
        fprintf(stderr, "%s, %d: ALERT: " FMT(__VA_ARGS__, 0)"%s\n", __func__, __LINE__, ARGS(__VA_ARGS__, "")); exit(1)

int main(void) {
  struct tcpServer tcpServerInst;
  struct tcpServer *tcpServerPtr = &tcpServerInst;
  uint32_t message[1536000];
  message[0] = 0x00010002;
  pthread_t tcpServerPid;
  InitTcpServer(&tcpServerPid, tcpServerPtr, INADDR_LOOPBACK, 9999, 5);
  printf("server:    ");
  print_sockaddr_in(&tcpServerPtr->addr);
  while(true) {
    tcpServerPtr->UpdateBuffer(tcpServerPtr, message, sizeof(message));
    usleep(2500L);
  }
  void *retval = NULL;
  pthread_join(tcpServerPid, retval);
  return 0;
}

