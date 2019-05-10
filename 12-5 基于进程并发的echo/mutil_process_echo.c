//
// Created by ligand on 2019-05-04.
//

#include "../csapp.h"

void sigchld_handler(int sig) {
  // 循环回收子进程。
  while (waitpid(-1, 0, WNOHANG) > 0);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  signal(SIGCHLD, sigchld_handler);

  char *port = argv[1];
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  if ((listenfd = open_listenfd(port)) <= 0) {
    fprintf(stderr, "open port: %s failed!", port);
    exit(1);
  }

  while (1) {
    clientlen = sizeof(struct sockaddr_storage);
    connfd = accept(listenfd, (SA *)&clientaddr, &clientlen);
    if (Fork() == 0) {
      // child
      close(listenfd);
      echo(connfd);
      close(connfd);
      exit(0);
    }
    close(connfd);
  }
}
