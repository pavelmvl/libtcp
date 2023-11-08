
#include <netinet/in.h>
#include <stdio.h>

void print_sockaddr_in(struct sockaddr_in *addr) {
  printf("{ sin_family: ");
  switch(addr->sin_family) {
    case AF_INET:
      printf("AF_INET");
      break;
    default:
      printf("UNKNOWN");
      break;
  }
  uint32_t ip = ntohl(addr->sin_addr.s_addr);
  printf(", sin_addr: { s_addr: %d.%d.%d.%d }",
    ((ip >> 24) & 0xFF),
    ((ip >> 16) & 0xFF),
    ((ip >>  8) & 0xFF),
    ((ip      ) & 0xFF)
  );
  printf(", sin_port: %d", ntohs(addr->sin_port));
  printf("}\n");
}
