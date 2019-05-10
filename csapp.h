
#ifndef CSAPP_CSAPP_H_
#define CSAPP_CSAPP_H_

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#include <sys/socket.h>
#include <netdb.h>

#include <sys/select.h>

#include <pthread.h>
#include <semaphore.h>

#define MAXLINE 1024

void unix_error(char *msg);

pid_t Fork(void);


/*-------- Robust I/O -------------*/
#define RIO_BUFSIZE 8192

// robust io 的数据结构
typedef struct {
  int rio_fd;                 // 代理的unix文件描述符
  int rio_cnt;                // 内部 buffer 使用量
  char *rio_bufptr;           // 内部 buffer 的指针
  char rio_buf[RIO_BUFSIZE];  // 内部 buffer
} rio_t;

// b 表示 buffer
void rio_readinitb(rio_t *rp, int fd); // 初始化 buffer
ssize_t rio_writen(int fd, void *usrbuf, size_t n); // 无缓存写
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);  // 读文本行，包括换行符。
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t maxlen);     // 读二进制
/*-------- END Robust I/O ----------*/



/*-------- Socket -------------*/
typedef struct sockaddr SA;
// 建立一个到 hostname:port 的链接。
int open_clientfd(char *hostname, char *port);
// 监听本地 port 端口
int open_listenfd(char *port);
void echo(int connfd);
/*-------- END Socket ---------*/



/*----------- 并发编程 ----------------*/
// Apple 专属信号量兼容版本
#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif

typedef struct {
#ifdef __APPLE__
  dispatch_semaphore_t sem;
#else
  sem_t                   sem;
#endif
} rk_sema_t;

void rk_sema_init(rk_sema_t *s, uint32_t value);
void rk_sema_wait(rk_sema_t *s);
void rk_sema_post(rk_sema_t *s);

void rk_P(rk_sema_t *s);
void rk_V(rk_sema_t *s);

/**
 * 基于信号量的线程安全、泛型、循环队列。
 * 使用 void * 来实现泛型，也就是存的是 void * 数组，具体 malloc/free 以及 类型转换用户负责。
 */
typedef struct {
  int n;           // 容量
  void **buf;      // 实际缓存
  int front;       // 循环队列头
  int rear;        // 循环队列尾
  rk_sema_t mutex; // 访问互斥锁
  rk_sema_t slots; // 空余槽数。使用信号量来计数，也就不存在判断空/满的问题了。
  rk_sema_t items; // 可用物品数。
} squeue_t;

/***
 * 队列初始化
 * @param sp 队列指针
 * @param n  容量
 * @return 成功返回 0，不成功返回 -1
 */
int squeue_init(squeue_t *sp, int n);

/**
 * 入队列，简单的将地址入队列
 * @param sp 队列指针
 * @param item 元素指针。
 */
void squeue_push(squeue_t *sp, void *item);

/**
 * 出队列
 * @param sp 队列指针
 * @return 元素地址
 */
void *squeue_pop(squeue_t *sp);

/**
 * 擦屁股
 * @param sp 队列指针
 */
void squeue_deinit(squeue_t *sp);

/*----------- END 并发编程 ----------------*/


#endif
