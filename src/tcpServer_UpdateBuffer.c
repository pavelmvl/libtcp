
#include "libtcp.h"

int UpdateBuffer(struct tcpServer *s, void *newBuffer, size_t newBufferSize) {
  while(s->sent < s->clientCount) {
    usleep(1000);
  }
  pthread_mutex_lock(&s->mutex);
  s->sentIdx = (s->sentIdx + 1) % 16;
  s->buffer = newBuffer;
  s->bufferSize = newBufferSize;
  s->sent = 0;
  pthread_mutex_unlock(&s->mutex);
  return 0;
}
