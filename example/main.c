
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
  in_addr_t host = INADDR_ANY; // INADDR_LOOPBACK // inet_network("192.168.100.2")
  InitTcpServer(&tcpServerPid, tcpServerPtr, host, 9999, 50);
  printf("server:    "); print_sockaddr_in(&tcpServerPtr->addr);
  struct timespec us40 = {.tv_sec=0, .tv_nsec=40000000};
  while(true) {
    tcpServerPtr->UpdateBuffer(tcpServerPtr, message, sizeof(message));
    clock_nanosleep(CLOCK_MONOTONIC, 0, &us40, NULL);
  }
  void *retval = NULL;
  pthread_join(tcpServerPid, retval);
  return 0;
}

