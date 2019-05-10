//
// Created by ligand on 2019-05-02.
//

#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int open_clientfd(char *hostname, char *port) {
  int clientfd;
  struct addrinfo hints, *listp, *p;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;
  getaddrinfo(hostname, port, &hints, &listp);

  for (p = listp; p != NULL; p = p->ai_next) {
    if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
      continue;
    if ((connect(clientfd, p->ai_addr, p->ai_addrlen)) != -1)
      break;
    close(clientfd);
  }

  freeaddrinfo(listp);

  if (p == NULL)
    clientfd = -1;

  return clientfd;
}

int open_listenfd(char *port) {
  struct addrinfo hints, *listp, *p;
  int listenfd = -1, optval = 1;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;
  getaddrinfo(NULL, port, &hints, &listp);

  for (p = listp; p != NULL; p = p->ai_next) {
    if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
      continue;

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    if ((bind(listenfd, p->ai_addr, p->ai_addrlen)) == 0)
      break;
    close(listenfd);
  }

  freeaddrinfo(listp);
  if (p == NULL)
    return -1;

  if (listen(listenfd, 1024) < 0) {
    close(listenfd);
    return -1;
  }

  return listenfd;

}
