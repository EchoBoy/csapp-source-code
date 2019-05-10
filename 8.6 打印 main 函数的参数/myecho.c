/**
 * 打印 命令行参数 和 环境变量
 */

#include <stdio.h>
#define nullptr 0

int main(int argc, char *argv[], char *envp[]) {
  fprintf(stdout, "Command-line arguments:\n");
  int i = 0;
  for (char **p = argv; (*p) != nullptr; p++){
    fprintf(stdout, "argv[%d]: %s\n", i, *p);
    i++;
  }
  fprintf(stdout, "Environment variables:\n");
  int j = 0;
  for (char **p = envp; (*p) != nullptr; p++) {
    fprintf(stdout, "envp[%d]: %s\n", j, *p);
    j++;
  }

  return 0;
}
