//
// Created by ligand on 2019-05-05.
//

#include "../csapp.h"

void *thread(void *vargp);

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>", argv[0]);
    exit(1);
  }
  char *port = argv[1];
  int listenfd, *connfdp;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  pthread_t tid;

  if ((listenfd = open_listenfd(port)) <= 0) {
    fprintf(stderr, "open port %s failed!", port);
    exit(1);
  }

  while (1) {
    clientlen = sizeof(struct sockaddr_storage);
    connfdp = (int *) malloc(sizeof(int));
    *connfdp = accept(listenfd, (SA *) &clientaddr, &clientlen);
    pthread_create(&tid, NULL, thread, connfdp);
  }
}

void *thread(void *vargp) {
  int connfd = *((int *) vargp);
  pthread_detach(pthread_self());
  free(vargp);
  echo(connfd);
  close(connfd);
  return NULL;
}
