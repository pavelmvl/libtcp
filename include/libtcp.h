
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#include <pthread.h>

#ifndef _LIBTCP_LIBTCP_H_
#define _LIBTCP_LIBTCP_H_

struct tcpServer {
  struct sockaddr_in addr;
  int serverSocketFd;
  unsigned int clientCount;
  pthread_mutex_t mutex;
  void *buffer;
  size_t bufferSize;
  int sentIdx;               // round index of buffer, use for detect that buffer sent
  unsigned int sent;
  bool stop;
  int (*UpdateBuffer)(struct tcpServer*, void *, size_t);
};

int UpdateBuffer(struct tcpServer *s, void *newBuffer, size_t newBufferSize);

void print_sockaddr_in(struct sockaddr_in *addr);
void fprint_sockaddr_in(FILE *f, struct sockaddr_in *addr);

static void* threadTcpServerStart(void *arg);

int InitTcpServer(pthread_t *pid, struct tcpServer *s, uint32_t host, uint16_t port, int backlog);

#endif // _LIBTCP_LIBTCP_H_
