//
// Created by ligand on 2019-05-06.
// 生产者-消费者模型。使用线程安全的队列来实现。

#include "../csapp.h"


void *producer(void *vargp);
void *consumer(void *vargp);

static char *MSG = "hello?";

int main() {
  // 两个生产者，两个消费者。生产者将字符串 "hello?" 中的字符依次放入队列，消费者取出，并输出到标准输出。
  squeue_t sque;
  squeue_init(&sque, 100);

  pthread_t p1, p2, c1, c2;
  pthread_create(&p1, NULL, producer, &sque);

  pthread_create(&p2, NULL, producer, &sque);
  pthread_create(&c1, NULL, consumer, &sque);
  pthread_create(&c2, NULL, consumer, &sque);

  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  pthread_join(c1, NULL);
  pthread_join(c2, NULL);

  squeue_deinit(&sque);
}

void *producer(void *vargp) {
  squeue_t *sp = (squeue_t *) vargp;
  for (char *p = MSG; (*p) != '\0'; p++) {
    char *cp = malloc(sizeof(char));
    *cp = *p;
    squeue_push(sp, (void *) cp);
  }
  return NULL;
}

void *consumer(void *vargp) {
  squeue_t *sp = (squeue_t *) vargp;
  char *cp;
  while (1) {
    cp = (char *) squeue_pop(sp);
    printf("%c", *cp);
    free(cp);
    fflush(stdout);
  }
  return NULL;
}

