//
// Created by ligand on 2019-05-05.
//


#include "../csapp.h"

volatile int cnt = 0;

void *thread(void *vargp) {
  int i, n = *(int *) vargp;
  for (i = 0; i < n; i++) {
    cnt++;
  }
  return NULL;
}

int main() {
  int n = 100000;
  pthread_t tid1, tid2;
  pthread_create(&tid1, NULL, thread, &n);
  pthread_create(&tid2, NULL, thread, &n);
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  if (cnt != 2 * n) {
    printf("ops cnt is %d", cnt);
  } else {
    printf("you are luck! cnt is %d", cnt);
  }
}
