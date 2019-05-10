//
// Created by ligand on 2019-05-04.
// 基于 I/O 多路复用的并发事件驱动服务器

#include "../csapp.h"

typedef struct {
  char addr[MAXLINE];
  char port[MAXLINE];
} str_addr;

// 文件描述符池，用来管理多个文件描述符
typedef struct {
  int maxfd;                       // read_set 中最大的文件描述符
  fd_set read_set;                 // 所有的文件描述符
  fd_set ready_set;                // 能读的文件描述符，是 read_set 的子集
  int nready;                      // select 返回的能读的文件描述符的个数
  int maxi;                        // clientfd 最大使用下标
  int clientfd[FD_SETSIZE];        // 所有的文件描述符，-1 表空
  rio_t clientrio[FD_SETSIZE];     // 所有的文件描述符 RIO 包装，与 clientfd 对应。
  str_addr clientaddr[FD_SETSIZE]; //客户端地址
} pool;

void init_pool(int listenfd, pool *p);
void add_client(int connfd, pool *p, SA *clientaddr, int clientlen);
void check_clients(pool *p);

int byte_cnt = 0; // 服务器接受的数据的总字节数

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }
  char *port = argv[1];
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  static pool pool;

  if ((listenfd = open_listenfd(port)) <= 0) {
    fprintf(stderr, "open port %s failed!\n", port);
    exit(1);
  }
  init_pool(listenfd, &pool);

  while (1) {
    pool.ready_set = pool.read_set;
    pool.nready = select(pool.maxfd + 1, &(pool.ready_set), NULL, NULL, NULL);
    // 处理监听描述符
    if (FD_ISSET(listenfd, &(pool.ready_set))) {
      clientlen = sizeof(struct sockaddr_storage);
      connfd = accept(listenfd, (SA *) &clientaddr, &clientlen);
      add_client(connfd, &pool, (SA *) &clientaddr, clientlen);
    }

    // 处理链接描述符
    check_clients(&pool);
  }
}

void init_pool(int listenfd, pool *p) {
  int i;
  p->maxi = -1;
  for (i = 0; i < FD_SETSIZE; i++) {
    p->clientfd[i] = -1;
  }

  p->maxfd = listenfd;
  FD_ZERO(&(p->read_set));
  FD_SET(listenfd, &(p->read_set));
}

void add_client(int connfd, pool *p, SA *clientaddr, int clientlen) {
  int i;
  // 去掉监听描述符
  p->nready--;
  for (i = 0; i < FD_SETSIZE; i++) {
    if (p->clientfd[i] < 0) {
      p->clientfd[i] = connfd;
      rio_readinitb(&(p->clientrio[i]), connfd);
      getnameinfo(clientaddr,
                  clientlen,
                  p->clientaddr[i].addr,
                  MAXLINE,
                  p->clientaddr[i].port,
                  MAXLINE,
                  NI_NUMERICHOST | NI_NUMERICSERV);

      FD_SET(connfd, &(p->read_set));

      if (connfd > p->maxfd)
        p->maxfd = connfd;
      if (i > p->maxi)
        p->maxi = i;
      break;
    }
  }
  if (i == FD_SETSIZE) {
    fprintf(stderr, "add_client error: Too many clients.\n");
    exit(1);
  } else {
    printf("new connection from : %s:%s\n", p->clientaddr[i].addr, p->clientaddr[i].port);
  }
}

void check_clients(pool *p) {
  int i, connfd, n;
  char buf[MAXLINE];
  rio_t *rp;
  str_addr *clientaddrp;
  char *reply = "copy\n";

  for (i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
    connfd = p->clientfd[i];
    rp = &(p->clientrio[i]);
    clientaddrp = &(p->clientaddr[i]);
    if ((connfd > 0) && (FD_ISSET(connfd, &(p->ready_set)))) {
      p->nready--;
      if ((n = rio_readlineb(rp, buf, MAXLINE)) != 0) {
        byte_cnt += n;
        printf("MSG from : %s:%s\n\t%s\n", clientaddrp->addr, clientaddrp->port, buf);
        rio_writen(connfd, reply, strlen(reply));
      } else {
        close(connfd);
        printf("Close connection %s:%s\n", clientaddrp->addr, clientaddrp->port);
        FD_CLR(connfd, &(p->read_set));
        p->clientfd[i] = -1;
      }
    }
  }
}
