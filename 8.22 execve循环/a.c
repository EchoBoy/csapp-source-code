#include <stdio.h>
#include <unistd.h>
int main() {
  int t = 3;
  fprintf(stdout,"start a and sleep %ds\n", t);

  sleep(t);
  fprintf(stdout, "end a.\n");

  return 0;
}
