//
// Created by ligand on 2019-05-04.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
  char *buf, *p;
  long n1 = 0, n2 = 0, r = 0;
  if ((buf = getenv("QUERY_STRING")) != NULL) {
    p = strchr(buf, '&');
    *p = '\0';
    char *end;
    n1 = strtol(buf, &end, 10);
    n2 = strtol(p + 1, &end, 10);
    r = n1 + n2;
  }
  printf("%ld", r);
}
