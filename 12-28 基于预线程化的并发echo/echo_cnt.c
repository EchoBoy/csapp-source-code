//
// Created by ligand on 2019-05-06.
//

#include "../csapp.h"

static rk_sema_t mutex;
static long byte_cnt;

static void init_echo_cnt(void) {
  rk_sema_init(&mutex, 1);
  byte_cnt = 0;
}

void echo_cnt(int connfd) {
  static pthread_once_t once = PTHREAD_ONCE_INIT;
  pthread_once(&once, init_echo_cnt);

  rio_t rio;
  char buf[MAXLINE];
  int n;

  rio_readinitb(&rio, connfd);
  while ((n = rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    rk_P(&mutex);
    byte_cnt += n;
    printf("server received: %s\n", buf);
    rk_V(&mutex);
    rio_writen(connfd, buf, n);
  }
}
