
#include <netinet/in.h>
#include <stdio.h>

void fprint_sockaddr_in(FILE *f, struct sockaddr_in *addr) {
  fprintf(stderr, "{ sin_family: ");
  switch(addr->sin_family) {
    case AF_INET:
      fprintf(f, "AF_INET");
      break;
    default:
      fprintf(f, "UNKNOWN");
      break;
  }
  uint32_t ip = ntohl(addr->sin_addr.s_addr);
  fprintf(f, ", sin_addr: { s_addr: %d.%d.%d.%d }",
    ((ip >> 24) & 0xFF),
    ((ip >> 16) & 0xFF),
    ((ip >>  8) & 0xFF),
    ((ip      ) & 0xFF)
  );
  fprintf(f, ", sin_port: %d", ntohs(addr->sin_port));
  fprintf(f, "}\n");
}

void print_sockaddr_in(struct sockaddr_in *addr) {
  fprint_sockaddr_in(stdout, addr);
}
