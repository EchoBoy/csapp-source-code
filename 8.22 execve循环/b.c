#include <stdio.h>
#include <unistd.h>
#include "../csapp.h"

int main() {
  fprintf(stdout, "start b\n");
  fprintf(stdout, "fork b\n");

  Fork();
  fprintf(stdout, "execve a.out\n");
  char *argv[] = {"a.out",0};
  char *envp[] = {0};
  execve("a.out", argv, envp);

  fprintf(stdout, "fork b again\n");
  Fork();
  fprintf(stdout, "end\n");
  return 0;
}
