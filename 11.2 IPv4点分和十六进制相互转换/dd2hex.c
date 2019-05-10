//
// Created by ligand on 2019-04-30.
//
#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("usage: dd2hex <dd_num>\n");
    return 0;
  }
  char *src = argv[1];
  uint32_t dest;
  int r = inet_pton(AF_INET, src, &dest);
  if (r == 0) {
    printf("illegal dd_num: %s\n", src);
    return 0;
  } else if (r == -1) {
    printf("error");
    return -1;
  }
  uint32_t hr = ntohl(dest);
  printf("0x%x\n", hr);
  return 0;
}
