//
// Created by ligand on 2019-05-02.
//

#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
    exit(0);
  }

  struct addrinfo *p, *listp, hints;

  char addr_buf[1024];
  char port_buf[1024];
  int result_code, flags;

  memset(&hints, 0, sizeof(struct addrinfo));
//  hints.ai_family = AF_INET;  // IPv4 only;
//  hints.ai_socktype = SOCK_STREAM; // Connections only;
  if ((result_code = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(result_code));
    exit(1);
  }

  flags = NI_NUMERICHOST; // display address string instead of domain name;
  for (p = listp; p != NULL; p = p->ai_next) {
    getnameinfo(p->ai_addr, p->ai_addrlen, addr_buf, 1024, port_buf, 1024, flags);
    printf("addr: %s\tport:%s\n", addr_buf, port_buf);
  }

  freeaddrinfo(listp);
  exit(0);
}
