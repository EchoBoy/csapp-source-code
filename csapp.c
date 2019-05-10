//
// Created by ligand on 2019-03-17.
//

// 工具的实现

#include "csapp.h"

void unix_error(char *msg) {
  fprintf(stderr, "%s: %s\n", msg, strerror(errno));
  exit(0);
}

pid_t Fork(void) {
  pid_t pid;
  if ((pid = fork()) < 0)
    unix_error("Fork error");
  return pid;
}

// 普通的 write 函数带中断保护。
ssize_t rio_writen(int fd, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nwritten;
  char *bufp = usrbuf;
  while (nleft > 0) {
    if ((nwritten = write(fd, bufp, nleft)) <= 0) {
      if (errno == EINTR)
        nwritten = 0;
      else
        return -1; // writer error;
    }
    nleft -= nwritten;
    bufp += nwritten;
  }
  return n;
}

void rio_readinitb(rio_t *rp, int fd) {
  rp->rio_fd = fd;
  rp->rio_cnt = 0;
  rp->rio_bufptr = rp->rio_buf;
}

static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n) {
  int cnt;

  // 当前已经没有缓存
  while (rp->rio_cnt <= 0) {
    rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
    if (rp->rio_cnt < 0) {
      if (errno != EINTR) // 信号处理程序
        return -1;
    } else if (rp->rio_cnt == 0) // EOF
      return 0;
    else
      rp->rio_bufptr = rp->rio_buf; // Reset buffer ptr
  }

  cnt = n;
  if (rp->rio_cnt < n)
    cnt = rp->rio_cnt;
  memcpy(usrbuf, rp->rio_bufptr, cnt);
  rp->rio_bufptr += cnt;
  rp->rio_cnt -= cnt;
  return cnt;
}

// 每次读一个，直到找到 换行符 或者 EOF 或者 maxlen。
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) {
  int n;
  char c, *bufp = usrbuf;
  ssize_t rc;
  for (n = 1; n < maxlen; n++) {
    if ((rc = rio_read(rp, &c, 1)) == 1) {
      *bufp = c;
      bufp++;
      if (c == '\n') {
        n++;
        break;
      }
    } else if (rc == 0) {
      if (n == 1)
        return 0; // EOF, no data read;
      else
        break;
    } else
      return -1; //Error
  }
  *bufp = 0;
  return n - 1;
}

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nread;
  char *bufp = usrbuf;
  while (nleft > 0) {
    if ((nread = rio_read(rp, bufp, nleft)) < 0)
      return -1; // Error
    else if (nread == 0)
      break; // EOF
    nleft -= nread;
    bufp += nread;
  }
  return (n - nleft);
}

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

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int));

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

void echo(int connfd) {
  ssize_t n;
  char buf[MAXLINE];
  rio_t rio;

  rio_readinitb(&rio, connfd);
  // not EOF
  while ((n = rio_readlineb(&rio, buf, MAXLINE)) > 0) {
    fprintf(stdout, "Echo Server received : %s", buf);
    // echo
    rio_writen(connfd, buf, n);
  }
}

void rk_sema_init(rk_sema_t *s, uint32_t value) {
#ifdef __APPLE__
  dispatch_semaphore_t *sem = &s->sem;
  *sem = dispatch_semaphore_create(value);
#else
  sem_init(&s->sem, 0, value);
#endif
}

void rk_sema_wait(rk_sema_t *s) {

#ifdef __APPLE__
  dispatch_semaphore_wait(s->sem, DISPATCH_TIME_FOREVER);
#else
  int r;

  do {
          r = sem_wait(&s->sem);
  } while (r == -1 && errno == EINTR);
#endif
}

void rk_sema_post(rk_sema_t *s) {

#ifdef __APPLE__
  dispatch_semaphore_signal(s->sem);
#else
  sem_post(&s->sem);
#endif
}

inline void rk_P(rk_sema_t *s) {
  rk_sema_wait(s);
}

inline void rk_V(rk_sema_t *s) {
  rk_sema_post(s);
}

int squeue_init(squeue_t *sp, int n) {
  if (n <= 0) return -1;
  sp->n = n;
  sp->buf = malloc(n * sizeof(void *));
  if (sp->buf == NULL) return -1;

  sp->front = sp->rear = 0;

  rk_sema_init(&sp->mutex, 1);
  rk_sema_init(&sp->slots, n);
  rk_sema_init(&sp->items, 0);
  return 0;
}

void squeue_push(squeue_t *sp, void *item) {
  rk_P(&sp->slots);
  rk_P(&sp->mutex);

  sp->buf[sp->rear] = item;
  sp->rear = (sp->rear + 1) % (sp->n);

  rk_V(&sp->mutex);
  rk_V(&sp->items);
}

void *squeue_pop(squeue_t *sp) {
  void *res;
  rk_P(&sp->items);
  rk_P(&sp->mutex);

  res = sp->buf[sp->front];
  sp->front = (sp->front + 1) % (sp->n);

  rk_V(&sp->mutex);
  rk_V(&sp->slots);
  return res;
}

void squeue_deinit(squeue_t *sp) {
  free(sp->buf);
}


