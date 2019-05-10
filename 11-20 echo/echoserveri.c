//
// Created by ligand on 2019-05-03.
//

#include "../csapp.h"

int main(int argc, char **argv) {
//  if (argc != 2) {
//    fprintf(stderr, "usage: %s <port>\n", argv[0]);
//    exit(0);
//}
//  char *port = argv[1];

  char *port = "1234";
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  char client_hostname[MAXLINE], client_port[MAXLINE];

  if ((listenfd = open_listenfd(port)) <= 0) {
    fprintf(stderr, "listen port %s failed\n", port);
    exit(1);
  }

  fprintf(stdout, "listening port %s\n", port);

  while (1) {
    clientlen = sizeof(struct sockaddr_storage);
    connfd = accept(listenfd, (SA *) &clientaddr, &clientlen);
    getnameinfo((SA
    *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
    printf("Connect to %s:%s\n", client_hostname, client_port);
    echo(connfd);
    close(connfd);
  }
  exit(0);
}
