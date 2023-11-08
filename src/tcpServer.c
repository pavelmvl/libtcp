
#include "libtcp.h"

int UpdateBuffer(struct tcpServer *s, void *newBuffer, size_t newBufferSize) {
  pthread_mutex_lock(&s->mutex);
  s->sent = false;
  s->buffer = newBuffer;
  s->bufferSize = newBufferSize;
  pthread_mutex_unlock(&s->mutex);
  return 0;
}

static void* threadTcpServerStart(void *arg) {
  struct tcpServer *s = arg;
  struct sockaddr_in clientAddress;
  socklen_t clientAddressLen = sizeof(clientAddress);
  struct sockaddr *accept_addr = (struct sockaddr *)&clientAddress;
  pthread_mutex_init(&s->mutex, NULL);
  while (true) {
    int clientSocketFd = accept(s->serverSocketFd, accept_addr, &clientAddressLen);
    printf("connected: ");
    print_sockaddr_in(&clientAddress);
    if (clientSocketFd < 0) {
      fprintf(stderr, "accept errno(%d): %s", errno, strerror(errno));
      continue;
    }
    while(true) {
      if (!s->sent) {
        pthread_mutex_lock(&s->mutex);
        send(clientSocketFd, s->buffer, s->bufferSize, 0);
        s->sent = true;
        pthread_mutex_unlock(&s->mutex);
        printf("sent to "); print_sockaddr_in(&clientAddress);
      }
      usleep(1);
    }
  }
  return NULL;
}

int InitTcpServer(pthread_t *pid, struct tcpServer *s, uint32_t host, uint16_t port, int backlog) {
  s->UpdateBuffer = &UpdateBuffer;
  s->sent = true;
  s->addr.sin_family = AF_INET;
  s->addr.sin_addr.s_addr = htonl(host);
  s->addr.sin_port = htons(port);
  s->serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (s->serverSocketFd < 0) {
    return 1;
  }
  struct sockaddr *bind_addr = (struct sockaddr *)&s->addr;
  if (bind(s->serverSocketFd, bind_addr, sizeof(struct sockaddr_in)) < 0) {
    return 2;
  }
  if (listen(s->serverSocketFd, backlog) < 0) {
    return 3;
  }
  int err = pthread_create(pid, NULL, &threadTcpServerStart, (void *)s);
  return err;
}
