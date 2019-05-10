//
// Created by ligand on 2019-04-29.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mm.h"


// 吞吐量测试
void rate(int times) {
  size_t t;
  int max = 1024 * 1024;
  for (int i = 0; i < times; i++) {
    t = rand() % max;
//    char *p = malloc(t);
//    free(p);
    char *p = mm_malloc(t);
    mm_free(p);
  }
}

int main(void) {
  srand((unsigned)time(0));
  rate(100000);
  return 0;
}
