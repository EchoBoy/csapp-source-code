#include "../csapp.h"

int g = 0;


void sigint_handler(int sig)
{
  g += 1;
}
int main() {
  if (signal(SIGTSTP, sigint_handler) == SIG_ERR) {
      unix_error("signal error");
    }
  fprintf(stdout, "hell?\n");
  while (1) {
    fprintf(stdout, "g: %d\n", g);
    sleep(3);
  }
  return 0;
}
