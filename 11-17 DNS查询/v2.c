//
// Created by ligand on 2019-05-02.
//

#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {

  if (argc != 2) {
    fprintf(stderr, "usage: %s <domain name>\n", argv[1]);
    exit(0);
  }

  // host to socket address
  struct addrinfo *p, *listp, hints;
  int rc;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
    exit(1);
  }

  char res[128];
  for (p = listp; p != NULL; p = p->ai_next) {
    inet_ntop(p->ai_family, p->ai_addr->sa_data+2, res, 128);
    printf("%s\n", res);
  }
  freeaddrinfo(listp);
  exit(0);
}
