#include "../csapp.h"

sigjmp_buf buf;
int counter = 0;

void sig_handler(int sig)
{
  siglongjmp(buf, sig);
}
  

int main()
{
  signal(SIGINT, sig_handler);
  signal(SIGTSTP, sig_handler);
  switch(sigsetjmp(buf, 1)) {
  case SIGINT:
    fprintf(stdout, "Ctrl+C: restart...\n");
    break;
  case SIGTSTP:
    fprintf(stdout, "Ctrl+Z: exit\n");
    exit(0);
    break;
  default:
    fprintf(stdout, "unkown return\n");      
  }
  fprintf(stdout, "hellooooooooooooooooo\n");
  while (1){
    fprintf(stdout, "counter: %d\n", counter);
    ++counter;
    sleep(1);
  }
  return 0;
}
