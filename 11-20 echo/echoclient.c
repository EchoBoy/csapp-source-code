//
// Created by ligand on 2019-05-03.
//

#include "../csapp.h"

int main(int argc, char **argv) {
  int clientfd;
  char *host, *port;
  size_t ccount = MAXLINE;
  char *buf = (char *) malloc(ccount * sizeof(char));
  rio_t rio;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
    exit(0);
  }
  host = argv[1];
  port = argv[2];
  if ((clientfd = open_clientfd(host, port)) <= 0) {
    fprintf(stderr, "open connection to %s:%s failed\n", host, port);
    exit(1);
  }

  rio_readinitb(&rio, clientfd);

  while (getline((char **) &buf, &ccount, stdin) != -1) {
    rio_writen(clientfd, buf, strlen(buf));
    if (rio_readlineb(&rio, buf, MAXLINE) <= 0) {
      printf("server close!\n");
      exit(1);
    }
    fputs(buf, stdout);
  }
  close(clientfd);
  free(buf);
  exit(0);
}
