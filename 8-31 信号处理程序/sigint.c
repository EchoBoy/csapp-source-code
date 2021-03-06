#include "../csapp.h"

void sig_hanlder(int sig)
{
  switch(sig) {
  case SIGINT:
    fprintf(stdout, "Ctrl + c\npeeding...");
    pause();
    fprintf(stdout, "end Ctrl + c\n");
    break;
  case SIGTSTP:
    fprintf(stdout, "Ctrl + z\n");
    fprintf(stdout, "end Ctrl + z\n");
    break;
  }
}

int main(int argc, char *argv[], char *evnp[]) {
  if (signal(SIGINT, sig_hanlder) == SIG_ERR)
    unix_error("signal error");
  if (signal(SIGTSTP, sig_hanlder) == SIG_ERR)
    unix_error("signal error");
  while (1)
    pause();
}
