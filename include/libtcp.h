
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

typedef void* (*TcpWorker)(void*);
typedef void* (*TcpServerStart)(void*);

struct TcpServer {
  struct sockaddr_in addr;
  int serverSocketFd;
  unsigned int clientCount;
  pthread_mutex_t mutex;
  void *buffer;
  size_t bufferSize;
  int sentIdx;               // round index of buffer, use for detect that buffer sent
  unsigned int sent;
  bool stop;
  int (*UpdateBuffer)(struct TcpServer*, void *, size_t);
  TcpServerStart ServerStart;
  TcpWorker ServerWorker;
};

int UpdateBuffer(struct TcpServer *s, void *newBuffer, size_t newBufferSize);

void print_sockaddr_in(struct sockaddr_in *addr);
void fprint_sockaddr_in(FILE *f, struct sockaddr_in *addr);

int InitTcpServer(pthread_t *pid, struct TcpServer *s, uint32_t host, uint16_t port, int backlog, TcpServerStart serverStart);

void* threadTcpServerStart(void *arg);

int SetTcpServerSocketReuseAddr(struct TcpServer *s, int value);
int SetTcpServerSocketCork(struct TcpServer *s, int value);
int SetTcpServerSocketNodelay(struct TcpServer *s, int value);
int SetTcpServerSocketQuickAck(struct TcpServer *s, int value);

#endif // _LIBTCP_LIBTCP_H_
