#include <fcntl.h>
#include "libtcp.h"

struct workerArg {
  struct tcpServer *s;
  int clientSocketFd;
  struct sockaddr_in clientAddress;
};

struct workerArg* NewWorkerArg(struct tcpServer *s, int clientSocketFd, struct sockaddr_in *clientAddress) {
  struct workerArg* w = (struct workerArg*)malloc(sizeof(struct workerArg));
  w->s = s;
  w->clientSocketFd = clientSocketFd;
  memcpy(&w->clientAddress, clientAddress, sizeof(struct sockaddr_in));
  return w;
}

static void* threadTcpServerWorker(void *arg) {
  struct workerArg *w = (struct workerArg *)arg;
  struct tcpServer *s = w->s;
  struct sockaddr_in *clientAddress = &w->clientAddress;
  int clientSocketFd = w->clientSocketFd;
  //int quickackValue = 1;
  //if (setsockopt(clientSocketFd, IPPROTO_TCP, TCP_QUICKACK, &quickackValue, sizeof(quickackValue)) < 0) {
  //  fprintf(stderr, "setsockopt TCP_QUICKACK failure\n");
  //}
  pthread_mutex_lock(&s->mutex);
  s->clientCount++;
  pthread_mutex_unlock(&s->mutex);
  ssize_t size = 0;
  int error = 0;
  socklen_t errorLen = sizeof(error);
  int storedSentIdx = s->sentIdx - 1;
  int sendFlag = 0; //MSG_DONTROUTE; //MSG_DONTWAIT;
  while(!s->stop && size > -1) {
    if (storedSentIdx != s->sentIdx) {
      if (getsockopt(clientSocketFd, SOL_SOCKET, SO_ERROR, &error, &errorLen) == 0 && error == 0) {
        /// Socket is OK
        size = send(clientSocketFd, s->buffer, s->bufferSize, sendFlag);
        // size = write(clientSocketFd, s->buffer, s->bufferSize);
        if (size != s->bufferSize) {
          fprintf(stderr, "thread %d incorrect data sent: %ld, should be %ld\n",
            clientSocketFd,
            size,
            s->bufferSize
          );
        }
        pthread_mutex_lock(&s->mutex);
        s->sent++;
        storedSentIdx = s->sentIdx;
        pthread_mutex_unlock(&s->mutex);
        // printf("%d sent to ", storedSentIdx); print_sockaddr_in(clientAddress);
      } else {
        /// Socket is broken
        break;
      }
    } // storedSentIdx != s->sentId
    //usleep(1);
  }
  pthread_mutex_lock(&s->mutex);
  s->clientCount--;
  pthread_mutex_unlock(&s->mutex);
  shutdown(clientSocketFd, SHUT_RDWR);
  printf("disconnected: "); print_sockaddr_in(clientAddress);
  free(w);
  return NULL;
}

static void* threadTcpServerStart(void *arg) {
  struct tcpServer *s = arg;
  struct sockaddr_in clientAddress;
  socklen_t clientAddressLen = sizeof(clientAddress);
  struct sockaddr *accept_addr = (struct sockaddr *)&clientAddress;
  pthread_mutex_init(&s->mutex, NULL);
  while (true) {
    int clientSocketFd = accept(s->serverSocketFd, accept_addr, &clientAddressLen);
    fprintf(stderr, "connected: "); print_sockaddr_in(&clientAddress);
    if (clientSocketFd < 0) {
      fprintf(stderr, "accept errno(%d): %s", errno, strerror(errno));
      continue;
    }
    pthread_t workerPid;
    int err = pthread_create(
      &workerPid,
      NULL,
      &threadTcpServerWorker,
      (void*)NewWorkerArg(s, clientSocketFd, &clientAddress)
    );
    if (pthread_detach(workerPid) != 0) {
      fprintf(stderr, "pthread_detach %ld error\n", workerPid);
    }
    if (err == 0) {
    } else {
      fprintf(stderr, "threadTcpServerStart: pthread_create() return error: %d\n", err);
    }
  }
  fprintf(stderr, "tcpServer exit");
  return NULL;
}

void tcpServerExit(int status, void *arg) {
  printf("exit status %d\n", status);
}

int InitTcpServer(pthread_t *pid, struct tcpServer *s, uint32_t host, uint16_t port, int backlog) {
  s->UpdateBuffer = &UpdateBuffer;
  s->sentIdx = 0;
  s->clientCount = 0;
  s->addr.sin_family = AF_INET;
  s->addr.sin_addr.s_addr = htonl(host);
  s->addr.sin_port = htons(port);
  s->serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (s->serverSocketFd < 0) {
    return 1;
  }
  int reuseAddrValue = 1;
  if (setsockopt(s->serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &reuseAddrValue, sizeof(reuseAddrValue)) < 0) {
    fprintf(stderr, "setsockopt SO_REUSEADDR failure\n");
  }
  //int corkValue = 1;
  //if (setsockopt(s->serverSocketFd, IPPROTO_TCP, TCP_CORK, &corkValue, sizeof(corkValue)) < 0) {
  //  fprintf(stderr, "setsockopt TCP_CORK failure\n");
  //}
  int nodelayValue = 1;
  if (setsockopt(s->serverSocketFd, IPPROTO_TCP, TCP_NODELAY, &nodelayValue, sizeof(nodelayValue)) < 0) {
    fprintf(stderr, "setsockopt TCP_NODELAY failure\n");
  }
  //int quickackValue = 1;
  //if (setsockopt(s->serverSocketFd, IPPROTO_TCP, TCP_QUICKACK, &quickackValue, sizeof(quickackValue)) < 0) {
  //  fprintf(stderr, "setsockopt TCP_QUICKACK failure\n");
  //}
//  int flags = fcntl(s->serverSocketFd, F_GETFL, 0);
//  if (flags == -1) {
//    fprintf(stderr, "fcntl get socket flags error\n");
//  } else {
//    if (fcntl(s->serverSocketFd, F_SETFL, flags | O_NONBLOCK) < 0) {
//      fprintf(stderr, "fcntl set socket flag NONBLOCK error\n");
//    }
//  }
  //int sndbufValue = 1024*1024; // default 208*1024, max 1024*1024
  //if (setsockopt(s->serverSocketFd, SOL_SOCKET, SO_SNDBUF, &sndbufValue, sizeof(sndbufValue)) < 0) {
  //  fprintf(stderr, "setsockopt SO_SNDBUF set failure\n");
  //}
  struct sockaddr *bind_addr = (struct sockaddr *)&s->addr;
  if (bind(s->serverSocketFd, bind_addr, sizeof(struct sockaddr_in)) < 0) {
    return 2;
  }
  if (listen(s->serverSocketFd, backlog) < 0) {
    return 3;
  }
  s->stop = false;
  int err = pthread_create(pid, NULL, &threadTcpServerStart, (void *)s);
  on_exit(&tcpServerExit, NULL);
  return err;
}
