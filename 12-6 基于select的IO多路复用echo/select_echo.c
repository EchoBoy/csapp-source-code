//
// Created by ligand on 2019-05-04.
//
// 使用select I/O 多路复用来同时服务 网络I/O 和 标准输入

#include "../csapp.h"

void command(void) {
  char buf[MAXLINE];
  if (!fgets(buf, MAXLINE, stdin))
    exit(0);
  printf("%s", buf);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }

  char *port = argv[1];
  int listenfd, connfd;
  socklen_t clientlen;
  fd_set read_set, ready_set;
  struct sockaddr_storage clientaddr;

  if ((listenfd = open_listenfd(port)) <= 0) {
    fprintf(stderr, "open port %s failed!", port);
    exit(1);
  }
  FD_ZERO(&read_set);
  FD_SET(STDIN_FILENO, &read_set);
  FD_SET(listenfd, &read_set);

  while (1) {
    ready_set = read_set;
    select(listenfd + 1, &ready_set, NULL, NULL, NULL);
    if (FD_ISSET(STDIN_FILENO, &ready_set)) {
      command();
    }
    if (FD_ISSET(listenfd, &ready_set)) {
      clientlen = sizeof(struct sockaddr_storage);
      if ((connfd = accept(listenfd, (SA *) &clientaddr, &clientlen)) <= 0) {
        fprintf(stderr, "connection error");
        exit(0);
      }
      echo(connfd);
      close(connfd);
    }
  }
}
