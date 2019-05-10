//
// Created by ligand on 2019-05-06.
// 基于预线程化的并发echo。使用 生产者-消费者模型

#include "../csapp.h"

const static int BUFSIZE = 1024;
const static int THREADNUM = 8;

void *thread(void *arg);
void echo_cnt(int connfd);

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  // 初始化网络
  char *port = argv[1];
  int listenfd;
  struct sockaddr_storage clientaddr;
  socklen_t clientlen;

  listenfd = open_listenfd(port);
  if (listenfd <= 0) {
    fprintf(stderr, "open port %s failed!", port);
    exit(1);
  }

  // 初始化队列
  squeue_t sque;
  squeue_init(&sque, BUFSIZE);

  // 初始化预线程
  pthread_t tid;
  for (int i = 0; i < THREADNUM; i++)
    pthread_create(&tid, NULL, thread, &sque);


  // 主线程作为生产者开始无限循环
  int *connfdp;
  while (1) {
    connfdp = malloc(sizeof(int));
    *connfdp = accept(listenfd, (SA *)&clientaddr, &clientlen);
    if (*connfdp <= 0) {
      fprintf(stderr, "error accept!");
      break;
    }
    squeue_push(&sque, connfdp);
  }
}

void *thread(void *arg) {
  squeue_t *sp = (squeue_t *)arg;
  int *connfdp = NULL;
  while(1) {
    connfdp = (int *)squeue_pop(sp);
    // 实际服务函数。
    echo_cnt(*connfdp);
    free(connfdp);
    connfdp = NULL;
  }
}
