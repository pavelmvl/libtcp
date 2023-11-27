
#include "libtcp.h"

int UpdateBuffer(struct tcpServer *s, void *newBuffer, size_t newBufferSize) {
  static struct timespec prev = {.tv_sec = 0, .tv_nsec = 0};
  int sleepCount = 0;
  int usleepValue = 100;
  while(s->sent < s->clientCount && s->clientCount > 0) {
    sleepCount++;
    usleep(usleepValue);
  }
  //if (sleepCount > 0) {
  struct timespec now;
  int64_t diff = -1;
  if (clock_gettime(CLOCK_MONOTONIC, &now) == 0) {
    diff = now.tv_nsec - prev.tv_nsec + (now.tv_sec - prev.tv_sec) * 1000000000L;
    prev.tv_sec = now.tv_sec;
    prev.tv_nsec = now.tv_nsec;
  }
    fprintf(
      stderr,
      "UpdateBuffer idx: %2d, sent: %2d, clientCount: %2d, waitFor: %9d, diff: %3.9f\n",
      s->sentIdx,
      s->sent,
      s->clientCount,
      sleepCount * usleepValue,
      diff/1e9
    );
  //}
  pthread_mutex_lock(&s->mutex);
  s->sentIdx = (s->sentIdx + 1) % 16;
  s->buffer = newBuffer;
  s->bufferSize = newBufferSize;
  s->sent = 0;
  pthread_mutex_unlock(&s->mutex);
  return 0;
}
